#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRegularExpression>
#include <QDateTime>

#define VALID_IP_ADDRESS  "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
#define BROWSER_TERMINAL  "background-color: black; color: white; font-family: 'Courier New'; font-size: 12px;"
#define STATUS_BAR_ERROR  "QStatusBar{color: rgb(255, 255, 255); background-color: rgb(200, 0, 0)}"
#define STATUS_BAR_TRANSM "QStatusBar{color: rgb(0, 0, 0); background-color: rgb(152, 251, 152)}"
#define STATUS_BAR_STOP   "QStatusBar{color: rgb(0, 0, 0); background-color: rgb(255, 165, 0)}"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataIsSending = false;
    ui->statusbar->setStyleSheet(STATUS_BAR_STOP);
    ui->statusbar->showMessage("Данные не отправляются");
    ui->textBrowser_log->setStyleSheet(BROWSER_TERMINAL);

    connect(this, SIGNAL(signalBindSocket()),   &navdata, SLOT(bindReceiveSocket()));
    connect(this, SIGNAL(signalBindSocket()),   &navdata, SLOT(bindMulticastSocket()));
    connect(this, SIGNAL(signalSendData(bool)), &navdata, SLOT(slotCnabgeStateSending(bool)));
    connect(&navdata, SIGNAL(signalSendLog(QString)), this, SLOT(slotWriteLog(QString)));
    connect(&navdata, SIGNAL(signalWriteData(CodNavData)), this, SLOT(slotWriteData(CodNavData)));
    connect(&navdata, SIGNAL(signalWriteCodData(CodDataToFile)), &logger, SLOT(slotWriteCodData(CodDataToFile)));
    connect(&navdata, SIGNAL(signalWriteData(CodNavDataToFile)), &logger, SLOT(slotWriteNavData(CodNavDataToFile)));
    //
    connect(this, SIGNAL(signalWriteNavData(bool)), &logger, SLOT(slotSwitchStateRecord(bool)));
    connect(&logger, SIGNAL(signalSendLog(QString)), this, SLOT(slotWriteLog(QString)));
    //
    connect(ui->pushButton_sendCtrl, SIGNAL(clicked()), this, SLOT(slotSenderController()));
    connect(ui->pushButton_recordCtrl, SIGNAL(clicked()), this, SLOT(slotRecordController()));
    //
    connect(&logger, SIGNAL(failInitFile()), this, SLOT(slotRecordController()));
    //
    ui->lineEdit_ip->setText(navdata.getGroupAddress());
    ui->lineEdit_port->setText(QString::number(navdata.getMulticastDstPort()));
    ui->lineEdit_ip->setReadOnly(true);
    ui->lineEdit_port->setReadOnly(true);
    //
    qRegisterMetaType<CodNavDataToFile>("CodNavDataToFile");
    qRegisterMetaType<CodDataToFile>("CodDataToFile");
    qRegisterMetaType<CodNavData>("CodNavData");
    qRegisterMetaType<NavData>("NavData");
    qRegisterMetaType<CodData>("CodData");
}

MainWindow::~MainWindow()
{
    conn.quit();
    conn.wait();
    navdata.quit();
    navdata.wait();
    logger.quit();
    logger.wait();
    delete ui;
}

void MainWindow::slotSenderController()
{
    if(!dataIsSending)
    {
        bool ok;
        QRegularExpression ipRegex(VALID_IP_ADDRESS);

        QString address = ui->lineEdit_ip->text();
        QString  port   = ui->lineEdit_port->text();
        quint32 _port   = port.toUInt(&ok);

        if(ok && ipRegex.match(address).hasMatch())
        {
            //navdata.setIpAddress(address);
            //navdata.setDstPort(_port);
            //
            ui->statusbar->setStyleSheet(STATUS_BAR_TRANSM);
            ui->statusbar->showMessage(QString("Отправка данных на %1:%2").arg(address).arg(port));
            //ui->lineEdit_ip->setEnabled(false);
            //ui->lineEdit_port->setEnabled(false);
            ui->pushButton_sendCtrl->setText("Стоп передачи");
            dataIsSending = true;

            emit signalSendData(true);
        }
        else
        {
            ui->statusbar->setStyleSheet(QString(STATUS_BAR_ERROR));
            ui->statusbar->showMessage(QString("Ошибка адреса %1 %2").arg(address).arg(port));
        }
    }
    else
    {
        ui->statusbar->setStyleSheet(STATUS_BAR_STOP);
        ui->statusbar->showMessage("Данные не отправляются");
        //ui->lineEdit_ip->setEnabled(true);
        //ui->lineEdit_port->setEnabled(true);
        ui->pushButton_sendCtrl->setText("Старт передачи");
        dataIsSending = false;

        emit signalSendData(false);
    }
}

void MainWindow::slotRecordController()
{
    if(!dataIsRecording)
    {
        dataIsRecording  = true;
        emit signalWriteNavData(true);
        ui->pushButton_recordCtrl->setText("Стоп запись");
    }
    else
    {
        dataIsRecording  = false;
        emit signalWriteNavData(false);
        ui->pushButton_recordCtrl->setText("Старт запись");
    }
}

void MainWindow::slotWriteLog(QString log)
{
    ui->textBrowser_log->moveCursor(QTextCursor::Start);
    ui->textBrowser_log->insertPlainText(log);
}

void MainWindow::slotWriteData(CodNavData data)
{
    ui->lineEdit_altitude->setText(QString::number(data.altitude, 'f', 3));
    ui->lineEdit_roll->setText(QString::number(data.roll, 'f', 3));
    ui->lineEdit_pitch->setText(QString::number(data.pitch, 'f', 3));
    ui->lineEdit_latitude->setText(QString::number(data.latitude, 'f', 3));
    ui->lineEdit_longitude->setText(QString::number(data.longitude, 'f', 3));
    ui->lineEdit_yaw->setText(QString::number(data.yaw, 'f', 3));
    ui->lineEdit_gpsCourse->setText(QString::number(data.gpsCourse, 'f', 3));
    ui->lineEdit_gpsSpeed->setText(QString::number(data.gpsSpeed, 'f', 3));
    ui->lineEdit_aerialSpeed->setText(QString::number(data.aerialSpeed, 'f', 3));
    ui->lineEdit_aerialAltitude->setText(QString::number(data.aerialAlt, 'f', 3));
    ui->lineEdit_verticalSpeed->setText(QString::number(data.verticalSpeed, 'f', 3));
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    slotWriteLog(QDateTime::currentDateTime().toString("[hh:mm:ss] Начало работы"));
    emit signalBindSocket();
}

