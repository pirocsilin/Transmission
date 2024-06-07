#ifndef VIDEORECEIVER_H
#define VIDEORECEIVER_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QImage>
#include <memory>
#include "spb.h"

class VideoReceiver : public QObject
{
    Q_OBJECT
public:
    explicit VideoReceiver(QObject *parent = nullptr);

private:

    int sizeFullFrame {100000};
    bool isInitedFrameSize {false};
    uint32_t lastReceivedFrameNumber {0};

    std::unique_ptr<QTcpSocket> socketRecvVideo {nullptr};
//    QString ipAddressVideo {"127.0.0.1"};
    QString ipAddressVideo {"192.168.137.85"};
//    int portRecvVideo {50103};
    int portRecvVideo {22225};

    std::unique_ptr<QTimer> timerCheckWork {nullptr};
    bool isWorking {false};

    unsigned int ByteToUshort(unsigned char byte1, unsigned char byte2);
    int indexStart = 0;
    bool goodFrame = false;

    ReceivedFrame recvFrame;
    QByteArray buff;
    QImage resultImage;

    int  findStart();

    void parseVideo(QByteArray &in);

    template <class T>
    void getDataFromByteArray(T& data, const QByteArray& source)
    {
        QDataStream in(source);
        in.setByteOrder(QDataStream::LittleEndian);
        in >> data;
    }

public slots:
    void start();

private slots:
    void checkWork();
    void slotReadyRead();

signals:
    void sendImage(QImage);

};

#endif // VIDEORECEIVER_H
