#include "udpserver.h"

UDPServer::UDPServer()
{
    udpsocket = new QUdpSocket(this);
    file = new QFile();
    timer = new QTimer();
}

UDPServer::~UDPServer()
{
    delete udpsocket;
    delete file;
}

void UDPServer::slot_btnFilepath_clicked(QString filepath, bool tcpMode)
{
    if(!tcpMode)
    {
    file->setFileName(filepath);
    qDebug() << "UDP SERVER: file path set: " << filepath;
    if(file->open(QIODevice::ReadOnly) == false)
    {
        emit signal_warning_openFileFailed();
        qDebug() << "UDP SERVER: Failed to open file";
    }
    else {
        emit signal_fileOpen(filepath);
        qDebug() << "UDP SERVER: File opened successfully";

        filename = "";
        filesize = 0;
        sendsize = 0;
        QFileInfo info(filepath);
        filename = info.fileName();
        filesize = info.size();
    }
    }
}

void UDPServer::slot_btnSend_clicked(QString ip, qint16 portOut)
{
    QHostAddress ipAdd;
    peerIp = ip;
    peerPort = portOut;

    ipAdd.setAddress(peerIp);
    QByteArray datagram = QString("head#%1#%2").arg(filename).arg(filesize).toUtf8();
    // Сначала отправим заголовок с именем и размером файла
    qDebug() << "UDP SERVER: Sending header: filename: " << filename << "file size: " << filesize;
    // Начинаем отправку
    qint64 len = udpsocket->writeDatagram(datagram, ipAdd, peerPort);
    // Сигнал для отображения сообщения о начале отправки файлов
    emit signal_fileSend_started();

    if(len > 0) UDPServer::sendData();
    else file->close();
}


void UDPServer::sendData()
{
    QHostAddress ipAdd;
    QString endMarker = QString("#end1A2B3C4D");
    ipAdd.setAddress(peerIp);
    qDebug() << "UDP SERVER: peer ip:" << peerIp << "peer port:" << peerPort;
    QByteArray datagram;

    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    qint64 len = 0;
    emit signal_sendFilesize(filesize);
    do
    {
        // делаем датаграммы побольше, 16 КБ
        datagram = file->read(1024 * 16);
        len = udpsocket->writeDatagram(datagram, ipAdd, peerPort);
        qint64 bytesPerSecond = 1024 * 1024 * 20; // 20 мбайт/сек
        qint64 sleepTime = (1024 * 16 * 1000) / bytesPerSecond;
        QThread::msleep(sleepTime);
        sendsize += len;
        qDebug() << "UDP SERVER: sending datagram, size" << len;
        emit signal_sendProgress(sendsize);
    } while(len > 0);

    if(sendsize == filesize)
    {
        datagram = endMarker.toUtf8();
        udpsocket->writeDatagram(datagram, ipAdd, peerPort);
        emit signal_fileSent();
        file->close();
        udpsocket->close();
    }

}
