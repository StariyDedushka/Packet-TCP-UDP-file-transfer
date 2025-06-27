#include "tcpserver.h"

TCPServer::TCPServer()
{
    tcpServer = new QTcpServer();
    tcpSocket = new QTcpSocket();
    timer = new QTimer();
    measureTimer = new QTimer();
    measureTimer->setInterval(500);
    file = new QFile();


    bool isOk = tcpServer->listen(QHostAddress::Any, 4242);
    if(isOk == false) emit signal_warning_listeningFailed();
    connect(tcpServer, &QTcpServer::newConnection, this, &TCPServer::slot_newConnection);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &TCPServer::slot_disconnected);
//    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &TCPServer::slot_error);
    connect(timer, &QTimer::timeout, this, &TCPServer::slot_timerStart);
    connect(measureTimer, &QTimer::timeout, this, &TCPServer::measureSpeed);
}

TCPServer::~TCPServer()
{
    delete tcpServer;
    delete tcpSocket;
    delete file;
    delete timer;
}

void TCPServer::slot_btnFilepath_clicked(QString filepath, bool tcpMode)
{
    if(tcpMode)
    {
    file->setFileName(filepath);
    qDebug() << "TCP SERVER: file path set: " << filepath;
    if(file->open(QIODevice::ReadOnly) == false)
    {
        emit signal_warning_openFileFailed();
        qDebug() << "TCP SERVER: Failed to open file";
    } else {
        emit signal_fileOpen(filepath);
        qDebug() << "TCP SERVER: File opened successfully";

        filename = "";
        fileSize = 0;
        sendSize = 0;
        QFileInfo info(filepath);
        filename = info.fileName();
        fileSize = info.size();
    }
    }
}

void TCPServer::slot_btnSend_clicked()
{
    measureTimer->start();
    // Сначала отправим заголовок с именем и размером файла
    QString buffer = QString("head#%1#%2").arg(filename).arg(fileSize);
    qDebug() << "TCP SERVER: Sending header: filename: " << filename << "file size: " << fileSize;
    // Начинаем отправку
    qint64 len = tcpSocket->write(buffer.toUtf8().data());
    // Ждём пока байты запишутся у клиента
    tcpSocket->waitForBytesWritten();
    // Сигнал для отображения сообщения о начале отправки файлов
    emit signal_fileSend_started();

    if(len > 0) this->timer->start(10);
    else file->close();
}

void TCPServer::sendData()
{
    qint64 len = 0;
    // qDebug() << "TCP SERVER: sendData() called!";
    emit signal_sendFilesize(fileSize);
    do
    {
        // буфер 1024 знака в кодировке utf-8 (2 байта/знак)
        char buf[1024 * 2] = {0};
        len = file->read(buf, sizeof(buf));
        len = tcpSocket->write(buf, len);
        // qDebug() << "TCP SERVER: writing data to socket, length:" << len;
        sendSize += len;
        emit signal_sendProgress(sendSize);

    } while(len > 0);

    qDebug() << "TCP SERVER: sendSize =" << sendSize << "fileSize =" << fileSize;
    if(sendSize == fileSize)
    {
        emit signal_fileSent();
        file->close();
        tcpSocket->close();
        measureTimer->stop();
    }
}

void TCPServer::slot_timerStart()
{
    this->timer->stop();
    TCPServer::sendData();
}

void TCPServer::slot_newConnection()
{

    tcpSocket = tcpServer->nextPendingConnection();
    qDebug() << "TCP SERVER: client connected: ip:" << tcpSocket->peerAddress() << "port: " << tcpSocket->peerPort();
    QString ip = tcpSocket->peerAddress().toString();
    qint16 port = tcpSocket->peerPort();
    emit signal_connectionSuccess(ip, port);
}

void TCPServer::slot_portEdited(qint16 listenPort, bool tcpMode)
{
    if(tcpMode) {
    tcpServer->close();
    bool isOk = tcpServer->listen(QHostAddress::Any, listenPort);
    if(isOk == false) emit signal_warning_listeningFailed();
    }
}

void TCPServer::slot_disconnected()
{
    qDebug() << "TCP SERVER: Emitting 'disconnect' signal!";
    emit signal_disconnected();
}

void TCPServer::slot_btnDisconnect_clicked(qint16 listenPort)
{
    qDebug() << "TCP SERVER: Disconnecting server/socket due to a button press";
    tcpServer->close();
    tcpSocket->abort();
    emit signal_disconnected();
    // Принудительно сбрасываем соединение и перезапускаем сервер на порте
    qDebug() << "TCP server is now listening on port [" << listenPort << "]";
    tcpServer->listen(QHostAddress::Any, listenPort);
}

void TCPServer::slot_stop_listening()
{
    tcpServer->close();
    qDebug() << "TCP server has stopped listening";
}

void TCPServer::slot_start_listening(qint16 listenPort)
{
    tcpServer->close();
    tcpServer->listen(QHostAddress::Any, listenPort);
    qDebug() << "TCP server is now listening on port [" << listenPort << "]";
}

void TCPServer::slot_error(QAbstractSocket::SocketError error)
{
    QString errorStr;
    switch(error)
    {
    case QAbstractSocket::HostNotFoundError:
    errorStr = "Ошибка: хост не найден";
    break;
    case QAbstractSocket::RemoteHostClosedError:
    errorStr = "Ошибка: клиент закрыл соединение";
    break;
    }
    emit signal_error(errorStr);
}


void TCPServer::measureSpeed()
{
    quint64 mbitps = (sendSize - prevSendSize) / (1024 * 1024) * 8; // мбиты в секунду
    prevSendSize = sendSize;
    qDebug() << "TCP SERVER: measureSpeed() timer timeout! speed:" << mbitps;
    emit signal_measuredSpeed(mbitps);
}
