#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QtNetwork>
#include <QFile>
#include <QObject>

class TCPClient : public QObject
{
    Q_OBJECT
public slots:
    void slot_Connected();
    void slot_ReadyToRead();
    void slot_btnConnect_clicked(QString ip, qint16 port);
    void slot_btndisconnect_clicked();
    void slot_disconnected();
    void slot_btnSaveDir_clicked(QString saveDir);
signals:
    void onConnected();
    void signal_warning_failedFile();
    void signal_info_receiving(uint maxProgress);
    void signal_info_receive_progress(uint progress);
    void signal_receive_finished(QString fileName, qint64 fileSize);
    void signal_disconnected();
private:
    QTcpSocket *tcpSocket;
    QFile *file;
    QString savePath;
    QString fileName;
    qint64 fileSize;
    qint64 receivedSize;
    bool isFile;

public:
    TCPClient();
    ~TCPClient();
};

#endif // TCPCLIENT_H
