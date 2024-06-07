#include "videoreceiver.h"
#include <QThread>
#include <QPixelFormat>
#include <chrono>

VideoReceiver::VideoReceiver(QObject *parent) : QObject(parent)
{
}

unsigned int VideoReceiver::ByteToUshort(unsigned char byte1, unsigned char byte2)
{
    unsigned int rez = byte1 * 0x100 + byte2;
    return rez;
}

void VideoReceiver::checkWork()
{
    if(!isWorking)
    {
        socketRecvVideo->abort();
        qDebug() << "Connecting to Host Video";
        socketRecvVideo->connectToHost(ipAddressVideo, portRecvVideo);
    }
    isWorking = false;
}

int VideoReceiver::findStart()
{
    for (int i = 0; i < buff.size() - 16; i++)
    {
        if (buff[i + 0] == static_cast<char>(0xFF) && buff[i + 1] == static_cast<char>(0xFF)
                && buff[i + 2] == static_cast<char>(0xAA) && buff[i + 3] == static_cast<char>(0x55))
            return i;
    }
    return -1;
}


void VideoReceiver::parseVideo(QByteArray &in)
{
    isWorking = true;
    QByteArray dataHeader;
    dataHeader.resize(16);

    while (in.size() >= sizeFullFrame)
    {
        if(goodFrame)
        {
            if(in.size() >= recvFrame.sizeFrame)
            {
                recvFrame.frame.replace(0, recvFrame.sizeFrame, in, recvFrame.sizeFrame);
                in.remove(0, recvFrame.sizeFrame);
            }else
            {
                return;
            }

            if(recvFrame.frame.size() == recvFrame.sizeFrame)
            {
                goodFrame = false;

                 resultImage = QImage(reinterpret_cast<uint8_t*>(recvFrame.frame.data()),
                         recvFrame.x, recvFrame.y,// QImage::Format_Grayscale16);
#if 1
                         QImage::toImageFormat(QPixelFormat(QPixelFormat::Grayscale,
                                            /*GRAY*/               16,
                                            /*SECOND*/             0,
                                            /*THIRD*/              0,
                                            /*FOURTH*/             0,
                                            /*FIFTH*/              0,
                                            /*ALPHA*/              0,
                                            /*ALPHA USAGE*/       QPixelFormat::IgnoresAlpha,
                                            /*ALPHA POSITION*/    QPixelFormat::AtBeginning,
                                            /*PREMULTIPLIED*/     QPixelFormat::NotPremultiplied,
                                            /*INTERPRETATION*/    QPixelFormat::UnsignedShort,
                                            /*BYTE ORDER*/        QPixelFormat::CurrentSystemEndian)
                                       ));
#endif
                 if((recvFrame.numFrame - lastReceivedFrameNumber) > 1)
                     qDebug() << "!!![Error] Missed frame: cur =" << recvFrame.numFrame
                              << ", last = " << lastReceivedFrameNumber;

                lastReceivedFrameNumber = recvFrame.numFrame;

                emit sendImage(resultImage.convertToFormat(QImage::Format_Grayscale8).scaled(640, 512));
            }
        }else
        {
            for(int i = 0; i < in.size(); i++)
            {
                if(in[i] == static_cast<char>(0xFF) && in[i + 1] == static_cast<char>(0xFF)
                        && in[i + 2] == static_cast<char>(0xAA) && in[i + 3] == static_cast<char>(0x55))
                {
                    in.remove(0, i);
                    break;
                }
            }

            if(in[0] == static_cast<char>(0xFF) && in[1] == static_cast<char>(0xFF)
                    && in[2] == static_cast<char>(0xAA) && in[3] == static_cast<char>(0x55))
            {
                dataHeader.replace(0, dataHeader.size(), in, dataHeader.size());
                in.remove(0, dataHeader.size());

                getDataFromByteArray(recvFrame, dataHeader);

                if(recvFrame.bitFrame == 8)
                    recvFrame.sizeFrame = recvFrame.x * recvFrame.y;
                else if (recvFrame.bitFrame > 8 && recvFrame.bitFrame < 17)
                    recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 2;
                else if (recvFrame.bitFrame > 16 && recvFrame.bitFrame < 33)
                    recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 4;

                recvFrame.frame.reserve(recvFrame.sizeFrame);
                if(!isInitedFrameSize)
                {
                    sizeFullFrame = recvFrame.headerSize + recvFrame.sizeFrame;
                    isInitedFrameSize = true;
                }

                if(in.size() >= recvFrame.headerSize - dataHeader.size())
                {
                    in.remove(0, recvFrame.headerSize - dataHeader.size());
                    goodFrame = true;
                }
            }
        }
    }
}

void VideoReceiver::slotReadyRead()
{
//    qDebug() << __PRETTY_FUNCTION__ << socketRecvVideo->bytesAvailable()
//             << socketRecvVideo->state()
//             << socketRecvVideo->error();

    QByteArray ba = socketRecvVideo->readAll();
    //! копируем все считанное в буфер

    buff.append(ba);

    if(buff.size() < sizeFullFrame)
        return;

    parseVideo(buff);
}

void VideoReceiver::start()
{
    socketRecvVideo.reset(new QTcpSocket(this));
    connect(socketRecvVideo.get(), &QTcpSocket::readyRead, this, &VideoReceiver::slotReadyRead);
    socketRecvVideo->connectToHost(ipAddressVideo, portRecvVideo);

    timerCheckWork.reset(new QTimer(this));
    connect(timerCheckWork.get(), &QTimer::timeout, this, &VideoReceiver::checkWork);
    timerCheckWork->start(1000);
}

#if 0
void VideoReceiver::slotReadyRead()
{
    isWorking = true;

    int j {0};
    buff = socketRecvVideo->readAll();
    //qDebug() << buff.size();
    if(buff.size() > 6)
    {
        if(goodFrame)
        {
            for(int i = 0; i < buff.size() && recvFrame.frame.size() < recvFrame.sizeFrame; i++)
            {
                recvFrame.frame.push_back(buff[i]);
                j = i;
            }

            if(recvFrame.frame.size() == recvFrame.sizeFrame)
            {
                goodFrame = false;
                resultImage = QImage(reinterpret_cast<uint8_t*>(recvFrame.frame.data()),
                         recvFrame.x, recvFrame.y,// QImage::Format_Grayscale16);
#if 1
                         QImage::toImageFormat(QPixelFormat(QPixelFormat::Grayscale,
                                            /*GRAY*/               16,
                                            /*SECOND*/             0,
                                            /*THIRD*/              0,
                                            /*FOURTH*/             0,
                                            /*FIFTH*/              0,
                                            /*ALPHA*/              0,
                                            /*ALPHA USAGE*/       QPixelFormat::IgnoresAlpha,
                                            /*ALPHA POSITION*/    QPixelFormat::AtBeginning,
                                            /*PREMULTIPLIED*/     QPixelFormat::NotPremultiplied,
                                            /*INTERPRETATION*/    QPixelFormat::UnsignedShort,
                                            /*BYTE ORDER*/        QPixelFormat::CurrentSystemEndian)
                                       ));
#endif

                emit sendImage(resultImage.convertToFormat(QImage::Format_Grayscale8));
                j++;
            }

            if((goodFrame == false) && (j < buff.size()))
            {
                indexStart = findStart();
                if(indexStart != -1
                        && buff[0 + indexStart] == static_cast<char>(0xFF)
                        && buff[1 + indexStart] == static_cast<char>(0xFF)
                        && buff[2 + indexStart] == static_cast<char>(0xAA)
                        && buff[3 + indexStart] == static_cast<char>(0x55))
                {
                    recvFrame.numFrame = ByteToUshort(buff[9 + indexStart], buff[8 + indexStart]);
                    recvFrame.headerSize = ByteToUshort(buff[7 + indexStart], buff[6 + indexStart]);
                    recvFrame.x = ByteToUshort(buff[11 + indexStart], buff[10 + indexStart]);
                    recvFrame.y = ByteToUshort(buff[13 + indexStart], buff[12 + indexStart]);
                    recvFrame.bitFrame = ByteToUshort(buff[15 + indexStart], buff[14 + indexStart]);

                    if(recvFrame.bitFrame == 8)
                        recvFrame.sizeFrame = recvFrame.x * recvFrame.y;
                    else if (recvFrame.bitFrame > 8 && recvFrame.bitFrame < 17)
                        recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 2;
                    else if (recvFrame.bitFrame > 16 && recvFrame.bitFrame < 33)
                        recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 4;

                    goodFrame = true;

                    if(indexStart + recvFrame.headerSize < buff.size())
                    {
                        recvFrame.frame.clear();
                        recvFrame.frame.reserve(3000000);
                        for(int i = indexStart + recvFrame.headerSize + 1;
                            i < buff.size() && recvFrame.frame.size() < recvFrame.sizeFrame; i++)
                        {
                            recvFrame.frame.push_back(buff[i]);
                            //                            if(i+1 < buff.size())
                            //                            {
                            //                                res.frame.push_back(buff[i+1]);
                            //                                res.frame.push_back(buff[i]);
                            //                            }
                        }

                    }
                }
            }
        }
        else
        {
            indexStart = findStart();
            if(indexStart != -1 && buff[0 + indexStart] == static_cast<char>(0xFF) && buff[1 + indexStart] == static_cast<char>(0xFF) && buff[2 + indexStart] == static_cast<char>(0xAA) && buff[3 + indexStart] == static_cast<char>(0x55))
            {
                recvFrame.numFrame = ByteToUshort(buff[9 + indexStart], buff[8 + indexStart]);
                recvFrame.headerSize = ByteToUshort(buff[7 + indexStart], buff[6 + indexStart]);
                recvFrame.x = ByteToUshort(buff[11 + indexStart], buff[10 + indexStart]);
                recvFrame.y = ByteToUshort(buff[13 + indexStart], buff[12 + indexStart]);
                recvFrame.bitFrame = ByteToUshort(buff[15 + indexStart], buff[14 + indexStart]);

                if(recvFrame.bitFrame == 8)
                    recvFrame.sizeFrame = recvFrame.x * recvFrame.y;
                else if (recvFrame.bitFrame > 8 && recvFrame.bitFrame < 17)
                    recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 2;
                else if (recvFrame.bitFrame > 16 && recvFrame.bitFrame < 33)
                    recvFrame.sizeFrame = (recvFrame.x * recvFrame.y) * 4;

                goodFrame = true;

                if(indexStart + recvFrame.headerSize < buff.size())
                {
                    recvFrame.frame.clear();
                    recvFrame.frame.reserve(3000000);
                    for(int i = indexStart + recvFrame.headerSize + 1; i < buff.size()
                        && recvFrame.frame.size() < recvFrame.sizeFrame; i++)
                    {
                        recvFrame.frame.push_back(buff[i]);
                        //                        if(i+1 < buff.size())
                        //                        {
                        //                            res.frame.push_back(buff[i+1]);
                        //                            res.frame.push_back(buff[i]);
                        //                        }
                    }
                }
            }
        }
    }
}
#endif
