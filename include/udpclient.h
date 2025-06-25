#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QNetworkDatagram>

class UDPClient : public QObject
{
    Q_OBJECT
public slots:
    void slot_btnSend_clicked(QString ip, qint16 portOut);
    void slot_btnFilepath_clicked(QString filepath, bool tcpMode);
private slots:
    void readPendingDatagrams();
    void resendDatagram();
    void readFile();
    void measureSpeed();

signals:
    void signal_warning_openFileFailed();
    void signal_fileOpen(QString filepath);
    void signal_sendFilesize(qint64 fileSize);
    void signal_sendProgress(qint64 sendSize);
    void signal_fileSent();
    void signal_fileSend_started();
    void signal_measuredSpeed(quint64 mbitps);
public:
    UDPClient();
    ~UDPClient();

private:
    QUdpSocket *udpsocket;
    QByteArray currentData;
    QFile *file;
    qint64 prevSendsize;
    qint64 filesize;
    qint64 sendsize;
    QString filename;
    QString peerIp;
    qint16 peerPort;
    QTimer *resendTimer;
    QTimer *measureTimer;
    void sendDatagrams(const QByteArray &datagram);

};

#endif // UDPCLIENT_H
