#include "tcpclient.h"

TCPClient::TCPClient() {
    tcpSocket = new QTcpSocket();
    file = new QFile();
    connect(tcpSocket, &QTcpSocket::connected, this, &TCPClient::slot_Connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &TCPClient::slot_ReadyToRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &TCPClient::slot_disconnected);
    isFile = false;

}

TCPClient::~TCPClient()
{
    delete tcpSocket;
    delete file;
}

void TCPClient::slot_Connected() {
    fileSize = 0;
    fileName = "";
    receivedSize = 0;
    isFile = false;
    emit(onConnected());

}

void TCPClient::slot_ReadyToRead()
{
    QByteArray buf = tcpSocket->readAll();
    if(isFile == false)
    {
    isFile = true;
    qDebug() << "CLIENT: slot Ready to read is called";
    QString pathTmp = savePath;
    pathTmp.append("/");
    fileName = QString(buf).section("#", 1, 1);
    pathTmp.append(fileName);
    fileSize = QString(buf).section("#", 2, 2).toInt();
    qDebug() << "File name: " << fileName << ", file size " << fileSize << " bytes.";

    receivedSize = 0;
    file->setFileName(pathTmp);
    if(file->open(QIODevice::WriteOnly) == false)
    {
        fileName = "";
        fileSize = 0;
        receivedSize = 0;
        emit signal_warning_failedFile();
        qDebug() << "CLIENT: error, failed to open file!";
        return;
    }

    emit signal_info_receiving(fileSize / 1024);
    } else {
    qint64 len = file->write(buf);
    receivedSize += len;
    qDebug() << "Packet received: " << len;
    emit signal_info_receive_progress(receivedSize/1024);
    if(receivedSize == fileSize)
    {
        file->close();
        emit signal_receive_finished(fileName, fileSize);
        tcpSocket->disconnectFromHost();
    }
    }
}

void TCPClient::slot_btnConnect_clicked(QString ip, qint16 port)
{
    tcpSocket->abort();
    tcpSocket->connectToHost(QHostAddress(ip), port);
    qDebug() << "Attempting connection to host";
}

void TCPClient::slot_btndisconnect_clicked()
{
    tcpSocket->disconnectFromHost();
    qDebug() << "Connection terminated";

}

void TCPClient::slot_disconnected()
{
    emit signal_disconnected();
}

void TCPClient::slot_btnSaveDir_clicked(QString saveDir)
{
    savePath = saveDir;
    qDebug() << "Saved file dir:" << savePath;
}
