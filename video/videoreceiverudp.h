#ifndef VIDEORECEIVERUDP_H
#define VIDEORECEIVERUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <QImage>
#include <memory>
#include <video/structseth.h>
#include "common/structscod.h"

class VideoReceiverUdp : public QObject
{
    Q_OBJECT
public:
    explicit VideoReceiverUdp(QObject *parent = nullptr);

private:

struct ImageDataFrame
{
    uint32_t currentFrame;          //!< текущий кадр данных
    uint32_t countPacksInFrame;     //!< всего пакетов в кадре
    uint16_t countReceivedPacks {}; //!< количество полученных пакетов
    uint16_t width;                 //!< ширина кадра в пикселях
    uint16_t height;                //!< высота кадра в пикселях
    QByteArray image;               //!< пиксели изображения построчно
    bool isCompleted() { return countReceivedPacks == countPacksInFrame; }
};

    ImageDataFrame imageFrame;  //!< принимаемое тепловизионное изображение

    std::unique_ptr<QUdpSocket> socketVideo {nullptr};
    int portVideo {50801};
//    QString ipAddressVideo {"192.168.137.150"};

    const uint16_t countVideoBytesPerPack {1280}; //!< количество байт видеоданных в одном пакете

    void parseVideo(cod::CodFrameData &image);

    QByteArray buff;

    template <class T>
    void getDataFromByteArray(T& data, const QByteArray& source)
    {
        QDataStream in(source);
        in.setByteOrder(QDataStream::BigEndian);
        in >> data;
    }

    void initFrame(ImageDataFrame &out, const cod::CodFrameData &in);
    void fillFrameRaws(ImageDataFrame &out, const cod::CodFrameData &in);

    QImage resultImage;
    bool isFirstPack {true};

public slots:
    void startTransceiving();

private slots:
    void readDeviceDatagrams();

signals:
    void sendImage(QImage);
    void writeToFile(QByteArray);

};

#endif // VIDEORECEIVERUDP_H
