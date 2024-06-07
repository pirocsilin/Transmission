#include "spb.h"


QDataStream& operator<< (QDataStream& stream, const ReceivedFrame& in)
{
    stream << in.markOne
           << in.markTwo
           << in.markThree
           << in.headerSize
           << in.numFrame
           << in.x
           << in.y
           << in.bitFrame;

    return stream;

}

QDataStream& operator>> (QDataStream& stream, ReceivedFrame& out)
{
    stream  >> out.markOne
            >> out.markTwo
            >> out.markThree
            >> out.headerSize
            >> out.numFrame
            >> out.x
            >> out.y
            >> out.bitFrame;

    return stream;
}
