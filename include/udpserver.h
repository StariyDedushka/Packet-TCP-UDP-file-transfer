#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>

class UDPServer : public QObject
{
    Q_OBJECT
public slots:
    void slot_btnSend_clicked(QString ip, qint16 portOut);
    void slot_btnFilepath_clicked(QString filepath, bool tcpMode);

signals:
    void signal_warning_openFileFailed();
    void signal_fileOpen(QString filepath);
    void signal_sendFilesize(qint64 fileSize);
    void signal_sendProgress(qint64 sendSize);
    void signal_fileSent();
    void signal_fileSend_started();

public:
    UDPServer();
    ~UDPServer();

private:
    QUdpSocket *udpsocket;
    QFile *file;
    qint64 filesize;
    qint64 sendsize;
    QString filename;
    QString peerIp;
    qint16 peerPort;
    void sendData();
    QTimer *timer;

};

#endif // UDPSERVER_H
