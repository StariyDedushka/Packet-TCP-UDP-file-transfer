#include "udpserver.h"

UDPServer::UDPServer()
{
    udpsocket = new QUdpSocket(this);
    file = new QFile();
    connect(udpsocket, &QUdpSocket::readyRead, this, &UDPServer::slot_readyToRead);
    isFile = false;
    udpsocket->setReadBufferSize(128 * 1024 * 1024);
}

UDPServer::~UDPServer()
{
    delete udpsocket;
    delete file;
}

void UDPServer::slot_portEdited(qint16 listenPort, bool tcpMode)
{
    if(!tcpMode)
    {
    udpsocket->close();
    udpsocket->bind(listenPort);
    boundPort = listenPort;
    qDebug() << "UDP socket bound to port [" << listenPort << "]";
    }
}

void UDPServer::slot_readyToRead()
{
    if(isFile == false)
    {
        QNetworkDatagram datagram = udpsocket->receiveDatagram();
        QByteArray data = datagram.data();
        qDebug() << "Receiving datagram, size" << data.size();

        // фильтруем пустые датаграммы
        if(data.size() == 0) return;
        respondACK(&data, datagram.senderAddress(), datagram.senderPort());

        // Первое чтение будет заголовка и части файла, оно должно произойти единожды
        // Читаем заголовок, он не является частью файла и не будет записан
        processHeader(&data);
        if(file->open(QIODevice::WriteOnly) == false) emit signal_warning_failedFile();
    }
    else
    processPendingDatagrams();
}

// Тут обрабатываем заголовок файла с именем и размером
void UDPServer::processHeader(QByteArray *data)
{
    isFile = true;
    filename = QString(*data).section("#", 1, 1);
    filesize = QString(*data).section("#", 2, 2).toInt();
    QString pathTmp = savepath;
    pathTmp.append("/");
    pathTmp.append(filename);
    file->setFileName(pathTmp);

    emit signal_info_receiving(filesize/1024);

    qDebug() << "-----------------------------------------------------------------------------";
    qDebug() << "UDP SERVER: receiving header: File name: " << filename << ", file size " << filesize << " bytes.";
    qDebug() << "UDP SERVER: received file full path:" << pathTmp;
    qDebug() << "UDP SERVER: received header datagram size:" << data->size();

}

void UDPServer::processPendingDatagrams()
{
    while(udpsocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = udpsocket->receiveDatagram();
        QByteArray data = datagram.data();
        respondACK(&data, datagram.senderAddress(), datagram.senderPort());

        if(checkForEndingMarker(data) == true) finish_receiving();

        qint64 len = file->write(data);
        receivedSize += len;
        // qDebug() << "UDP SERVER: Written len == datagram->size()?:" << (len == data.size());
        emit signal_info_receive_progress(receivedSize/1024);
    }

}

bool UDPServer::checkForEndingMarker(const QByteArray &data)
{
    int markerPos = data.indexOf('#');
    qDebug() << "ACK marker position num.:" << markerPos;
    QByteArray marker;
    if(markerPos != -1)
    {
        marker = data.mid(markerPos + 1, 11);
        // проверяем наличие маркера конца файла (magic number) и при наличии закрываем файл,
        // сокет, биндим сокет заново и ставим флаг чтобы следующие датаграммы читались как
        // часть заголовка
        // qDebug() << "QString marker =" << marker;
        // qDebug() << "Marker is null?" << marker.isNull();
        if(marker == "end1A2B3C4D") return true;
        else return false;
    } else return false;
}

void UDPServer::respondACK(QByteArray *data, QHostAddress senderAddress, quint16 senderPort)
{
    // qDebug() << "Received from" << senderAddress << ":" << senderPort
    //          << "data:" << data;

    // Отправляем подтверждение обратно клиенту
    QByteArray ack = "ACK#" + *data;
    udpsocket->writeDatagram(ack, senderAddress, senderPort);
    // qDebug() << "Sending ACK back:" << ack;
}

void UDPServer::finish_receiving()
{
    if(file->size() != filesize)
    {
        qDebug() << "UDP SERVER: WARNING! File is received partially";
        emit signal_warning_incompleteFile();
    }
    qDebug() << "Received 'file end' marker, closing file and socket!";
    file->close();
    udpsocket->close();
    udpsocket->bind(boundPort);
    isFile = false;
    emit signal_receive_finished(filename, filesize);
}

void UDPServer::slot_btnSaveDir_clicked(QString saveDir)
{
    savepath = saveDir;
    qDebug() << "Saved file dir:" << savepath;
}

void UDPServer::slot_bind(qint16 listenPort)
{
    udpsocket->close();
    boundPort = listenPort;
    udpsocket->bind(listenPort);
    qDebug() << "UDP socket bound to port [" << listenPort << "]";
}


void UDPServer::slot_unbind()
{
    udpsocket->close();
    qDebug() << "UDP socket closed";
}
