#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // инициализируем шкалу прогресса
    ui->progressBar->setValue(0);
    setWindowTitle("Packet");
    ui->btnSend->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    tcpMode = true;
    helpMenu = new QMenu();
    helpAction = new QAction(tr("Справка"), this);
    setupMenu();
    connect(helpAction, &QAction::triggered, this, &MainWindow::slot_helpMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete helpMenu;
    delete helpAction;
}

void MainWindow::setupMenu()
{
    helpMenu = ui->menuBar->addMenu(tr("&Помощь"));
    helpMenu->addAction(helpAction);
}

void MainWindow::slot_helpMenu()
{
    QMessageBox helpMenu;
    helpMenu.setWindowTitle("Справка");
    helpMenu.setText("Справка по программе Packet");
    helpMenu.setInformativeText("Добро пожаловать в программу Packet!\n\n"
                                "Для начала работы с TCP протоколом достаточно ввести IP адрес и открытый входящий порт собеседника в "
                                "ваше поле 'исходящий порт',"
                                "после чего нажать 'Подключиться'.\n\n Таким образом вы открываете собеседнику возможность отправить вам файл.\n"
                                "Если вы хотели отправить файл сами, то запросите у собеседника подключение к вам, сообщив ему свой"
                                "входящий порт.\n\n"
                                "После успешного подключения принимающая сторона должна выбрать место сохранения файла, а отправляющая"
                                "файл, который нужно отправить. Ограничения на тип файла не налагаются.\n\n"
                                "При передаче по UDP возможность подключиться недоступна, существующее соединение разорвётся при включении этого режима.\n\n"
                                "Для передачи по UDP нет необходимости предварительно устанавливать соединение, только вводить порты и адрес."
                                "\n\nПосле каждой отправки файла файл нужно выбрать заново, даже если отправляется тот же файл.");
    helpMenu.exec();
}

void MainWindow::on_btnConnect_clicked()
{
    QString ip = ui->ipEdit->text();
    qint16 port = ui->portOutEdit->text().toInt();
    if(ip.isEmpty() == true)
    {
        QMessageBox::warning(this, "Предупреждение", "IP не может быть пустым");
        return;
    }
    emit signal_btnConnect(ip, port);
}

void MainWindow::on_btnDisconnect_clicked()
{
    qint16 listenPort = ui->portInEdit->text().toInt();
    emit signal_btnDisconnect(listenPort);
    ui->progressBar->setValue(0);
}

void MainWindow::onConnect()
{
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
    ui->statusInfoText->append("Соединение с хостом успешно");
    ui->btnSend->setEnabled(true);
    ui->progressBar->setValue(0);
}


void MainWindow::slot_warning_failedFile()
{
    ui->statusInfoText->append("Ошибка открытия файла");
}

void MainWindow::slot_info_receiving(qint64 maxProgress)
{
    ui->progressBar->setMaximum(maxProgress);
}

void MainWindow::slot_info_receive_progress(qint64 progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::slot_receive_finished(QString filename, qint64 filesize)
{
    QMessageBox::information(this, "Готово", "Файл получен");
    ui->statusInfoText->append("Файл получен");
    float filesizeKb = filesize / 1000;
    float filesizeMb = filesize / (1000*1000);
    if(filesize <= 50000)
    ui->statusInfoText->append(QString("Имя файла: %1, размер: %2 байт").arg(filename).arg(filesize));
    else if(filesize > 50000 && filesize <= 5000000)
    ui->statusInfoText->append(QString("Имя файла: %1, размер: %2 килобайт").arg(filename).arg(filesizeKb));
    else if (filesize > 5000000)
    ui->statusInfoText->append(QString("Имя файла: %1, размер: %2 мегабайт").arg(filename).arg(filesizeMb));
}

void MainWindow::on_btn_filepath_clicked()
{
    ui->progressBar->setValue(0);
    QString filepath = QFileDialog::getOpenFileName(this, "Выберите файл", "../");
    if(!filepath.isEmpty())
    {
        qDebug() << "Filepath: " << filepath;
        emit signal_btnFilepath_clicked(filepath, tcpMode);
    }
    else QMessageBox::warning(this, "Ошибка", "Неправильный путь к файлу!");
}


void MainWindow::slot_fileOpen(QString filepath)
{
    ui->statusInfoText->append("Будет отправлен файл: ");
    ui->statusInfoText->append(filepath);
}

void MainWindow::slot_fileSend_started()
{
    ui->statusInfoText->append("Отправляется файл...");
}

void MainWindow::slot_connectionSuccess(QString peerIp, qint16 peerPort)
{
    ui->statusInfoText->append(QString("[%1:%2] успешно подключились").arg(peerIp).arg(peerPort));
    ui->btnSend->setEnabled(true);
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
}

void MainWindow::slot_warning_listeningFailed()
{
    ui->statusInfoText->append("Прослушиваемый порт недоступен");
}

void MainWindow::slot_fileSent()
{
    ui->statusInfoText->append("Файл успешно отправлен");
}

void MainWindow::on_btnSend_clicked()
{
    if(tcpMode) emit signal_btnSend_clicked_TCP();
    else emit signal_btnSend_clicked_UDP(ui->ipEdit->text(), ui->portOutEdit->text().toInt());
}


void MainWindow::on_portInEdit_editingFinished()
{
    emit signal_portEdited(ui->portInEdit->text().toInt(), tcpMode);
}

void MainWindow::slot_disconnected()
{
    ui->statusInfoText->append("Соединение разорвано");
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);
}


void MainWindow::on_btnSavePath_clicked()
{
    QString saveDir = QFileDialog::getExistingDirectory(this, "Сохранить в", "../");
    ui->label_savePath->setText(saveDir);
    emit signal_btnSavePath_clicked(saveDir);
}


void MainWindow::on_btn_TCP_clicked()
{
    emit signal_start_listening_TCP(ui->portInEdit->text().toInt());
    emit signal_unbind_UDP();
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);
    tcpMode = true;
}


void MainWindow::on_btn_UDP_clicked()
{
    emit signal_btnDisconnect(ui->portInEdit->text().toInt());
    emit signal_stop_listening_TCP();
    emit signal_bind_UDP(ui->portInEdit->text().toInt());
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(true);
    tcpMode = false;
}

void MainWindow::slot_error(QString error)
{
    ui->statusInfoText->append(error);
}

void MainWindow::slot_warning_incompleteFile()
{
    ui->statusInfoText->append("Предупреждение: файл получен не полностью!");
}


void MainWindow::slot_measuredSpeed(quint64 mbitps)
{
    ui->statusbar->showMessage(QString("Скорость отправки, Мбит/с: %1").arg(mbitps));
}
