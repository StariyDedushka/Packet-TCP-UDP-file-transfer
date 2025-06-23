#include "udpclient.h"

UDPClient::UDPClient()
{
    udpsocket = new QUdpSocket(this);
    file = new QFile();
    connect(udpsocket, &QUdpSocket::readyRead, this, &UDPClient::slot_readyToRead);
    isFile = false;
    udpsocket->setReadBufferSize(128 * 1024 * 1024);
}

UDPClient::~UDPClient()
{
    delete udpsocket;
    delete file;
}

void UDPClient::slot_portEdited(qint16 listenPort, bool tcpMode)
{
    if(!tcpMode)
    {
    udpsocket->close();
    udpsocket->bind(listenPort);
    boundPort = listenPort;
    qDebug() << "UDP socket bound to port [" << listenPort << "]";
    }
}

void UDPClient::slot_readyToRead()
{
    QByteArray datagram;
    datagram.resize(udpsocket->pendingDatagramSize());
    udpsocket->readDatagram(datagram.data(), datagram.size());
    qDebug() << "Receiving datagram, size" << datagram.size();
    // фильтруем пустые датаграммы
    if(datagram.size() == 0) return;
    // Первое чтение будет заголовка и части файла, оно должно произойти единожды
    if(isFile == false)
    {
    // Читаем заголовок, он не является частью файла и не будет записан
    processHeader(&datagram);
    if(file->open(QIODevice::WriteOnly) == false) emit signal_warning_failedFile();
    }
    else
    processDatagram(&datagram);
}

// Тут обрабатываем заголовок файла с именем и размером
void UDPClient::processHeader(QByteArray *datagram)
{
    isFile = true;
    filename = QString(*datagram).section("#", 1, 1);
    filesize = QString(*datagram).section("#", 2, 2).toInt();
    if(filesize == 0) isFile = true;
    QString pathTmp = savepath;
    pathTmp.append("/");
    pathTmp.append(filename);
    file->setFileName(pathTmp);

    emit signal_info_receiving(filesize/1024);

    qDebug() << "-----------------------------------------------------------------------------";
    qDebug() << "UDP CLIENT: receiving header: File name: " << filename << ", file size " << filesize << " bytes.";
    qDebug() << "UDP CLIENT: received file full path:" << pathTmp;
    qDebug() << "UDP CLIENT: received header datagram size:" << datagram->size();

}

void UDPClient::processDatagram(QByteArray *datagram)
{
    // QString marker;
    // marker = QString(*datagram).section("#", 1, 1);
    int markerPos = datagram->indexOf('#');
    QByteArray marker;
    if(markerPos != -1)
    {
        marker = datagram->mid(markerPos + 1, 11);
        // проверяем наличие маркера конца файла (magic number) и при наличии закрываем файл,
        // сокет, биндим сокет заново и ставим флаг чтобы следующие датаграммы читались как
        // часть заголовка
        qDebug() << "QString marker =" << marker;
    if(marker == "end1A2B3C4D")
    {
        finish_receiving();
        return;
    }
    }
    qint64 len = file->write(*datagram);
    qDebug() << "UDP CLIENT: Written len == datagram->size()?:" << (len == datagram->size());
    receivedSize += len;
    emit signal_info_receive_progress(receivedSize/1024);

    // входим в цикл чтения из очереди только если накопилась очередь
    // UDP шлёт пакеты так, что очередь не всегда формируется
    while(udpsocket->hasPendingDatagrams())
    {
    datagram->resize(udpsocket->pendingDatagramSize());
    qint64 bytesRead = udpsocket->readDatagram(datagram->data(), datagram->size());
    if(bytesRead != datagram->size())
    {
        qDebug() << "UDP CLIENT: warning! Datagram is not read entirely";
    }
    markerPos = datagram->indexOf('#');
    if(markerPos != -1)
    {
    marker = datagram->mid(markerPos + 1, 11);
    qDebug() << "QString marker =" << marker;
    if(marker == "end1A2B3C4D")
    {
        finish_receiving();
        return;
    }
    }
    len = file->write(*datagram);
    qDebug() << "UDP CLIENT: Written len == datagram->size()?:" << (len == datagram->size());
    qDebug() << "Receiving datagram from queue, size" << datagram->size();
    receivedSize += len;
    emit signal_info_receive_progress(receivedSize/1024);

    }

}

void UDPClient::finish_receiving()
{
    if(file->size() != filesize)
    {
        qDebug() << "UDP CLIENT: WARNING! File is received partially";
        emit signal_warning_incompleteFile();
    }
    qDebug() << "Received 'file end' marker, closing file and socket!";
    file->close();
    udpsocket->close();
    udpsocket->bind(boundPort);
    isFile = false;
    emit signal_receive_finished(filename, filesize);
}

void UDPClient::slot_btnSaveDir_clicked(QString saveDir)
{
    savepath = saveDir;
    qDebug() << "Saved file dir:" << savepath;
}

void UDPClient::slot_bind(qint16 listenPort)
{
    udpsocket->close();
    boundPort = listenPort;
    udpsocket->bind(listenPort);
    qDebug() << "UDP socket bound to port [" << listenPort << "]";
}


void UDPClient::slot_unbind()
{
    udpsocket->close();
    qDebug() << "UDP socket closed";
}
