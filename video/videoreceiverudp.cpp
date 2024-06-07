
#include <QThread>
#include <QPixelFormat>
#include <chrono>
#include <cmath>

#include "videoreceiverudp.h"
#include "common/structscod.h"

VideoReceiverUdp::VideoReceiverUdp(QObject *parent) : QObject(parent)
{
}

void VideoReceiverUdp::parseVideo(cod::CodFrameData &image)
{
    if(isFirstPack)
    {
        initFrame(imageFrame, image);
        isFirstPack = false;
//        qDebug() << __PRETTY_FUNCTION__ << "1" << imageFrame.countPacksInFrame;
    }

    if(imageFrame.isCompleted())
    {
        //! если предыдущий кадр полный, то инициализируем новый кадр
        initFrame(imageFrame, image);
//        qDebug() << __PRETTY_FUNCTION__ << "1" << imageFrame.countPacksInFrame;
    }else if(imageFrame.currentFrame != image.numberFrame)
    {
//        qDebug() << __PRETTY_FUNCTION__ << "2" << imageFrame.currentFrame << image.numberFrame;
        //! пришел следующий кадр при неполном предыдущем
        int countMissedBytesInImage {image.height*image.width*image.bits/8 - imageFrame.image.size()};

        //! заполняем нулями пропущенные строки в предыдущем кадре
        imageFrame.image.push_back(QByteArray(countMissedBytesInImage, 0));

        //! передаем для отображения не полный кадр
        resultImage = QImage(reinterpret_cast<uint8_t*>(imageFrame.image.data()),
                             image.width, image.height, QImage::Format_Grayscale8);
        emit sendImage(resultImage);
//                emit sendImage(resultImage);

                qDebug() << __PRETTY_FUNCTION__;
        //! подготовка перед следующим кадром
        initFrame(imageFrame, image);
    }

    fillFrameRaws(imageFrame, image);
}

void VideoReceiverUdp::initFrame(VideoReceiverUdp::ImageDataFrame &out, const cod::CodFrameData &in)
{
    out.countReceivedPacks = 0;
    out.countPacksInFrame  = (in.width*in.height*in.bits/8)/countVideoBytesPerPack;
    out.width              = in.width;
    out.height             = in.height;
    out.currentFrame       = in.numberFrame;
    out.image.clear();
    out.image.reserve(in.width * in.height*in.bits/8);
}

void VideoReceiverUdp::fillFrameRaws(VideoReceiverUdp::ImageDataFrame &out, const cod::CodFrameData &in)
{
    int countMissedPacks = in.numberFirstLine/in.countLine - out.countReceivedPacks;

    if(countMissedPacks < 0)
        return;

    if(countMissedPacks == 0)
    {
        //! нет пропущенных пакетов со строками
        ++out.countReceivedPacks;
        out.image.push_back(in.frameArray);
    }else
    {
        //! есть пропуски пакетов со строками

        qDebug() << "!!!!!!!" << countMissedPacks << in.numberFirstLine /in.countLine << out.countReceivedPacks;
        //! заполняем нулями пропущенные строки
        out.countReceivedPacks += countMissedPacks;
        out.image.push_back(QByteArray(countVideoBytesPerPack * countMissedPacks, 0));

        //! заполняем новыми данными
        ++out.countReceivedPacks;
        out.image.push_back(in.frameArray);
    }
}

void VideoReceiverUdp::startTransceiving()
{
    socketVideo.reset(new QUdpSocket());
    connect(socketVideo.get(), &QUdpSocket::readyRead,
            this,              &VideoReceiverUdp::readDeviceDatagrams);
    socketVideo->bind(portVideo);
}

void VideoReceiverUdp::readDeviceDatagrams()
{
    //qDebug() << __PRETTY_FUNCTION__;

    QByteArray inBuffer;

    while (socketVideo->hasPendingDatagrams())
    {
        inBuffer.resize(socketVideo->pendingDatagramSize());
        socketVideo->readDatagram(inBuffer.data(), inBuffer.size());

        emit writeToFile(cod::getDataForWriteToFile(inBuffer, inBuffer.size()));

        if(static_cast<uint8_t>(inBuffer[0]) == static_cast<uint8_t>(0xCD))
        {
            cod::CodFrameData image;
            getDataFromByteArray(image, inBuffer);
            parseVideo(image);

            static uint16_t countLines {};

//            qDebug() << __PRETTY_FUNCTION__ << image.numberFirstLine;

//            if(std::abs(countLines - image.numberFirstLine) > 3)
//                qDebug() << __PRETTY_FUNCTION__ << countLines << image.numberFirstLine;

            countLines = image.numberFirstLine;

            if(imageFrame.isCompleted())
            {
                resultImage = QImage(reinterpret_cast<uint8_t*>(imageFrame.image.data()),
                                     image.width, image.height, QImage::Format_Grayscale8);
                //qDebug() << __PRETTY_FUNCTION__;

                emit sendImage(resultImage);

            }
        }
    }
}
