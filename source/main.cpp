#include "mainwindow.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "udpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    TCPClient tcpclient;
    TCPServer tcpserver;
    UDPServer udpserver;
    UDPClient udpclient;

    // tcpclient -> mainwindow
    MainWindow::connect(&tcpclient, &TCPClient::onConnected, &w, &MainWindow::onConnect);
    MainWindow::connect(&tcpclient, &TCPClient::signal_warning_failedFile, &w, &MainWindow::slot_warning_failedFile);
    MainWindow::connect(&tcpclient, &TCPClient::signal_info_receiving, &w, &MainWindow::slot_info_receiving);
    MainWindow::connect(&tcpclient, &TCPClient::signal_info_receive_progress, &w, &MainWindow::slot_info_receive_progress);
    MainWindow::connect(&tcpclient, &TCPClient::signal_receive_finished, &w, &MainWindow::slot_receive_finished);
    MainWindow::connect(&tcpclient, &TCPClient::signal_disconnected, &w, &MainWindow::slot_disconnected);
    // mainwindow -> tcpclient
    MainWindow::connect(&w, &MainWindow::signal_btnConnect, &tcpclient, &TCPClient::slot_btnConnect_clicked);
    MainWindow::connect(&w, &MainWindow::signal_btnDisconnect, &tcpclient, &TCPClient::slot_btndisconnect_clicked);
    MainWindow::connect(&w, &MainWindow::signal_btnSavePath_clicked, &tcpclient, &TCPClient::slot_btnSaveDir_clicked);

    // tcpserver -> mainwindow
    MainWindow::connect(&tcpserver, &TCPServer::signal_warning_listeningFailed, &w, &MainWindow::slot_warning_listeningFailed);
    MainWindow::connect(&tcpserver, &TCPServer::signal_connectionSuccess, &w, &MainWindow::slot_connectionSuccess);
    MainWindow::connect(&tcpserver, &TCPServer::signal_fileOpen, &w, &MainWindow::slot_fileOpen);
    MainWindow::connect(&tcpserver, &TCPServer::signal_fileSend_started, &w, &MainWindow::slot_fileSend_started);
    MainWindow::connect(&tcpserver, &TCPServer::signal_warning_openFileFailed, &w, &MainWindow::slot_warning_failedFile);
    MainWindow::connect(&tcpserver, &TCPServer::signal_fileSent, &w, &MainWindow::slot_fileSent);
    MainWindow::connect(&tcpserver, &TCPServer::signal_disconnected, &w, &MainWindow::slot_disconnected);
    MainWindow::connect(&tcpserver, &TCPServer::signal_fileSent, &w, &MainWindow::slot_disconnected);
    MainWindow::connect(&tcpserver, &TCPServer::signal_sendFilesize, &w, &MainWindow::slot_info_receiving);
    MainWindow::connect(&tcpserver, &TCPServer::signal_sendProgress, &w, &MainWindow::slot_info_receive_progress);
    MainWindow::connect(&tcpserver, &TCPServer::signal_error, &w, &MainWindow::slot_error);
    MainWindow::connect(&tcpserver, &TCPServer::signal_measuredSpeed, &w, &MainWindow::slot_measuredSpeed);
    // mainwindow -> tcpserver
    MainWindow::connect(&w, &MainWindow::signal_portEdited, &tcpserver, &TCPServer::slot_portEdited);
    MainWindow::connect(&w, &MainWindow::signal_btnFilepath_clicked, &tcpserver, &TCPServer::slot_btnFilepath_clicked);
    MainWindow::connect(&w, &MainWindow::signal_btnSend_clicked_TCP, &tcpserver, &TCPServer::slot_btnSend_clicked);
    MainWindow::connect(&w, &MainWindow::signal_btnDisconnect, &tcpserver, &TCPServer::slot_btnDisconnect_clicked);
    MainWindow::connect(&w, &MainWindow::signal_stop_listening_TCP, &tcpserver, &TCPServer::slot_stop_listening);
    MainWindow::connect(&w, &MainWindow::signal_start_listening_TCP, &tcpserver, &TCPServer::slot_start_listening);

    // UDPClient -> mainwindow
    MainWindow::connect(&udpclient, &UDPClient::signal_warning_openFileFailed, &w, &MainWindow::slot_warning_failedFile);
    MainWindow::connect(&udpclient, &UDPClient::signal_fileOpen, &w, &MainWindow::slot_fileOpen);
    MainWindow::connect(&udpclient, &UDPClient::signal_sendFilesize, &w, &MainWindow::slot_info_receiving);
    MainWindow::connect(&udpclient, &UDPClient::signal_fileSend_started, &w, &MainWindow::slot_fileSend_started);
    MainWindow::connect(&udpclient, &UDPClient::signal_sendProgress, &w, &MainWindow::slot_info_receive_progress);
    MainWindow::connect(&udpclient, &UDPClient::signal_fileSent, &w, &MainWindow::slot_fileSent);
    MainWindow::connect(&udpclient, &UDPClient::signal_measuredSpeed, &w, &MainWindow::slot_measuredSpeed);
    // mainwindow -> UDPServer
    MainWindow::connect(&w, &MainWindow::signal_btnFilepath_clicked, &udpclient, &UDPClient::slot_btnFilepath_clicked);
    MainWindow::connect(&w, &MainWindow::signal_btnSend_clicked_UDP, &udpclient, &UDPClient::slot_btnSend_clicked);

    // UDPServer -> mainwindow
    MainWindow::connect(&udpserver, &UDPServer::signal_warning_failedFile, &w, &MainWindow::slot_warning_failedFile);
    MainWindow::connect(&udpserver, &UDPServer::signal_info_receive_progress, &w, &MainWindow::slot_info_receive_progress);
    MainWindow::connect(&udpserver, &UDPServer::signal_info_receiving, &w, &MainWindow::slot_info_receiving);
    MainWindow::connect(&udpserver, &UDPServer::signal_receive_finished, &w, &MainWindow::slot_receive_finished);
    MainWindow::connect(&udpserver, &UDPServer::signal_warning_incompleteFile, &w, &MainWindow::slot_warning_incompleteFile);

    // mainwindow -> UDPServer1
    MainWindow::connect(&w, &MainWindow::signal_btnSavePath_clicked, &udpserver, &UDPServer::slot_btnSaveDir_clicked);
    MainWindow::connect(&w, &MainWindow::signal_portEdited, &udpserver, &UDPServer::slot_portEdited);
    MainWindow::connect(&w, &MainWindow::signal_bind_UDP, &udpserver, &UDPServer::slot_bind);
    MainWindow::connect(&w, &MainWindow::signal_unbind_UDP, &udpserver, &UDPServer::slot_unbind);

    w.show();
    return a.exec();
}
