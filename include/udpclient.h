#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QtNetwork>

class UDPClient : public QObject
{
    Q_OBJECT
private slots:
    void slot_readyToRead();
public slots:
    void slot_btnSaveDir_clicked(QString saveDir);
    void slot_portEdited(qint16 listenPort, bool tcpMode);
    void slot_bind(qint16 listenPort);
    void slot_unbind();

signals:
    void signal_warning_failedFile();
    void signal_info_receiving(uint maxProgress);
    void signal_info_receive_progress(uint progress);
    void signal_receive_finished(QString filename, qint64 filesize);
    void signal_warning_incompleteFile();

public:
    UDPClient();
    ~UDPClient();

private:
    QUdpSocket *udpsocket;
    QFile *file;
    QString savepath;
    QString filename;
    qint64 filesize;
    qint64 receivedSize;
    qint16 boundPort;
    bool isFile;
    void processHeader(QByteArray *datagram);
    void processDatagram(QByteArray *datagram);
    void finish_receiving();

};

#endif // UDPCLIENT_H
