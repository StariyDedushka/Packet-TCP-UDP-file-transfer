#include "udpclient.h"

#define DATAGRAM_SIZE 1024 * 16
#define END_MARKER
UDPClient::UDPClient()
{
    udpsocket = new QUdpSocket(this);
    file = new QFile();
    resendTimer = new QTimer;
    measureTimer = new QTimer;
    connect(udpsocket, &QUdpSocket::readyRead, this, &UDPClient::readPendingDatagrams);

    connect(resendTimer, &QTimer::timeout, this, &UDPClient::resendDatagram);
    connect(measureTimer, &QTimer::timeout, this, &UDPClient::measureSpeed);

    measureTimer->setInterval(500);

    resendTimer->setInterval(1000);
    resendTimer->setSingleShot(true);

    prevSendsize = 0;
    sendsize = 0;
}

UDPClient::~UDPClient()
{
    delete udpsocket;
    delete file;
    delete resendTimer;
    delete measureTimer;
}

void UDPClient::slot_btnFilepath_clicked(QString filepath, bool tcpMode)
{
    if(!tcpMode)
    {
    file->setFileName(filepath);
    qDebug() << "UDP CLIENT: file path set: " << filepath;
    if(file->open(QIODevice::ReadOnly) == false)
    {
        emit signal_warning_openFileFailed();
        qDebug() << "UDP CLIENT: Failed to open file";
    }
    else {
        emit signal_fileOpen(filepath);
        qDebug() << "UDP CLIENT: File opened successfully";

        filename = "";
        filesize = 0;
        sendsize = 0;
        prevSendsize = 0;
        QFileInfo info(filepath);
        filename = info.fileName();
        filesize = info.size();
    }
    }
}

void UDPClient::slot_btnSend_clicked(QString ip, qint16 portOut)
{
    QHostAddress ipAdd;
    peerIp = ip;
    peerPort = portOut;

    ipAdd.setAddress(peerIp);
    QByteArray datagram = QString("head#%1#%2").arg(filename).arg(filesize).toUtf8();
    currentData = datagram;
    // Сначала отправим заголовок с именем и размером файла
    qDebug() << "UDP CLIENT: Sending header: filename: " << filename << "file size: " << filesize;
    measureTimer->start();
    // Начинаем отправку
    qint64 len = udpsocket->writeDatagram(datagram, ipAdd, peerPort);
    // Сигнал для отображения сообщения о начале отправки файлов
    emit signal_fileSend_started();
    emit signal_sendFilesize(filesize);

    if(len > 0) readFile();
    else file->close();
}

void UDPClient::readPendingDatagrams()
{
    QNetworkDatagram datagram;
    QHostAddress ipAdd;
    ipAdd.setAddress(peerIp);
    while(udpsocket->hasPendingDatagrams())
    {
        datagram = udpsocket->receiveDatagram();
        QByteArray data = datagram.data();
        // qDebug() << "UDP CLIENT: received response datagram for:" << data;
        // qDebug() << "UDP CLIENT: current data:" << currentData;
        if(data.startsWith("ACK#"))
        {
            QByteArray originalData = data.mid(4);
            if(originalData == currentData)
            {
                resendTimer->stop();
                // qDebug() << "UDP CLIENT: received ACK for:" << originalData;
                sendsize += originalData.size();
                // qDebug() << "Sendsize =" << sendsize << ", filesize =" << filesize;
                readFile();
            }
        }
    }
}

void UDPClient::readFile()
{
    QByteArray datagram = file->read(DATAGRAM_SIZE);
    currentData = datagram;
    // qDebug() << "UDP CLIENT: sending datagram, size" << datagram.size();
    sendDatagrams(datagram);

}


void UDPClient::sendDatagrams(const QByteArray &datagram)
{
    QHostAddress ipAdd;
    ipAdd.setAddress(peerIp);
    // устанавливаем маркер, обозначающий конец файла
    QString endMarker = QString("#end1A2B3C4D");

    udpsocket->writeDatagram(datagram, ipAdd, peerPort);
    emit signal_sendProgress(sendsize);

    if(sendsize == filesize)
    {
        QByteArray endDatagram = endMarker.toUtf8();
        currentData = endDatagram;
        udpsocket->writeDatagram(endDatagram, ipAdd, peerPort);
        emit signal_fileSent();
        file->close();
        udpsocket->close();
        resendTimer->stop();
        measureTimer->stop();
        return;
    }
    resendTimer->start();
}

void UDPClient::resendDatagram()
{
    qDebug() << "No ACK received, resending...";
    QHostAddress ipAdd;
    ipAdd.setAddress(peerIp);
    udpsocket->writeDatagram(currentData, ipAdd, peerPort);
    resendTimer->start();
}

void UDPClient::measureSpeed()
{
    quint64 mbitps = (sendsize - prevSendsize) / (1024 * 1024) * 8; // мбиты в секунду
    prevSendsize = sendsize;
    qDebug() << "UDP CLIENT: measureSpeed() timer timeout! speed:" << mbitps;
    emit signal_measuredSpeed(mbitps);
}
