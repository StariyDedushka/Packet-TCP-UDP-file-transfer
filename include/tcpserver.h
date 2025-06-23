#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QFile>
#include <QFileInfo>

class TCPServer : public QObject
{
    Q_OBJECT

public slots:
    void slot_btnSend_clicked();
    void slot_btnFilepath_clicked(QString filepath, bool tcpMode);
    void slot_newConnection();
    void slot_disconnected();
    void slot_portEdited(qint16 listenPort, bool tcpMode);
    void slot_btnDisconnect_clicked(qint16 listenPort);
    void slot_stop_listening();
    void slot_start_listening(qint16 listenPort);
    void slot_error(QAbstractSocket::SocketError error);

private slots:
    void slot_timerStart();

signals:
    void signal_warning_listeningFailed();
    void signal_warning_openFileFailed();
    void signal_fileSend_started();
    void signal_fileOpen(QString filepath);
    void signal_fileSent();
    void signal_connectionSuccess(QString ip, qint16 port);
    void signal_disconnected();
    void signal_sendProgress(qint64 sendSize);
    void signal_sendFilesize(qint64 fileSize);
    void signal_error(QString error);
public:
    TCPServer();
    ~TCPServer();

private:
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QFile *file;
    QString filename;
    qint64 fileSize;
    qint64 sendSize;

    void sendData();
    QTimer *timer;

};

#endif // TCPSERVER_H
