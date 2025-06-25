#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool tcpMode;
    QMenu *helpMenu;
    QAction *helpAction;
    void setupMenu();

signals:
    void signal_btnConnect(QString ip, qint16 port);
    void signal_btnDisconnect(qint16 listenPort);
    void signal_btnFilepath_clicked(QString filepath, bool tcpMode);
    void signal_btnSend_clicked_UDP(QString ip, qint16 portOut);
    void signal_btnSend_clicked_TCP();
    void signal_portEdited(qint16 listenPort, bool tcpMode);
    void signal_btnSavePath_clicked(QString saveDir);
    void signal_stop_listening_TCP();
    void signal_start_listening_TCP(qint16 listenPort);
    void signal_bind_UDP(qint16 listenPort);
    void signal_unbind_UDP();

public slots:
    void on_btnConnect_clicked();
    void onConnect();
    void slot_warning_failedFile();
    void slot_info_receiving(qint64 maxProgress);
    void slot_info_receive_progress(qint64 progress);
    void slot_receive_finished(QString filename, qint64 filesize);
    void slot_fileSend_started();
    void slot_fileOpen(QString filepath);
    void slot_connectionSuccess(QString ip, qint16 port);
    void slot_warning_listeningFailed();
    void slot_fileSent();
    void slot_disconnected();
    void slot_error(QString error);
    void slot_warning_incompleteFile();
    void slot_helpMenu();
    void slot_measuredSpeed(quint64 mbitps);

private slots:
    void on_btnDisconnect_clicked();
    void on_btn_filepath_clicked();
    void on_btnSend_clicked();
    void on_portInEdit_editingFinished();
    void on_btnSavePath_clicked();
    void on_btn_TCP_clicked();
    void on_btn_UDP_clicked();
};
#endif // MAINWINDOW_H
