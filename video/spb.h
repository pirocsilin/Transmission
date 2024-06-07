#ifndef SPB_H
#define SPB_H
#include <stdint.h>
#include <stddef.h>
#include <QDataStream>
#include <QVector>

struct ReceivedFrame
{
    uint16_t markOne;
    uint16_t markTwo;
    uint16_t markThree;
    uint16_t headerSize;
    uint16_t numFrame;
    uint16_t x;
    uint16_t y;
    uint16_t bitFrame;

    int sizeFrame;
    QByteArray frame;
};


QDataStream& operator<< (QDataStream& stream, const ReceivedFrame& in);
QDataStream& operator>> (QDataStream& stream, ReceivedFrame& out);

#endif // SPB_H
