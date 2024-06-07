#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "transmission.h"
#include "navidata.h"
#include "common/structscod.h"
#include "common/common.h"
#include "videoreceiverudp.h"
#include "myopenglwidget.h"
#include "logger.h"
using namespace cod;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Transmission conn;
    NaviData navdata;
    Logger logger;

    bool dataIsSending;
    bool dataIsRecording {false};

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;

    QThread videoReceiverThread;
    std::unique_ptr<VideoReceiverUdp> videoReceiver {nullptr};

    MyOpenGLWidget cameraWidget;

    void prepareVideoThread();

public slots:
    void slotSenderController();
    void slotRecordController();
    void slotWriteLog(QString log);
    void slotWriteData(CodNavData data);
    void recvMipiImage(QImage image);

signals:
    void signalSendData(bool sign);
    void signalBindSocket();
    void signalWriteNavData(bool sign);

};
#endif // MAINWINDOW_H
