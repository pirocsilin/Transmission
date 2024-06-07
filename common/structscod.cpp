#include "structscod.h"

namespace cod
{

uint8_t calcChecksum(const char *in, size_t sz)
{
    uint8_t sum {};
    for(size_t i = 0; i < sz; ++i)
        sum += in[i];
    return (sum - static_cast<uint8_t>(in[indexCheckSum]));
}

QDataStream &operator<<(QDataStream &stream, const MessageId &in)
{
    stream << static_cast<uint8_t>(in);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, MessageId &out)
{
    uint8_t type;
    stream >> type;
    out = static_cast<MessageId>(type);
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const HeaderCodData &in)
{
    stream << in.syncByte << in.id << in.checkSum << in.time;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, HeaderCodData &out)
{
    stream >> out.syncByte >> out.id >> out.checkSum >> out.time;
    return stream;
}

bool isValidHeaderStructCod(const QByteArray &data)
{
    if(data.size() <= 3)
        return false;

    if((static_cast<uint8_t>(data.at(0)) == 0xDD) && (static_cast<uint8_t>(data.at(1)) == 0xC0))
        return true;

    return false;
}

QDataStream &operator<<(QDataStream &stream, const CodNavData &in)
{
    stream << in.header
           << in.id
           << in.latitude
           << in.longitude
           << in.altitude
           << in.roll
           << in.pitch
           << in.yaw
           << in.gpsCourse
           << in.gpsSpeed
           << in.aerialSpeed
           << in.aerialAlt
           << in.verticalSpeed;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, CodNavData &out)
{
    stream  >> out.header
            >> out.id
            >> out.latitude
            >> out.longitude
            >> out.altitude
            >> out.roll
            >> out.pitch
            >> out.yaw
            >> out.gpsCourse
            >> out.gpsSpeed
            >> out.aerialSpeed
            >> out.aerialAlt
            >> out.verticalSpeed;

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const cod::CodData &in)
{
    stream << in.header << in.id;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, cod::CodData &out)
{
    stream  >> out.header >> out.id;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const HdrCodNavData &in)
{
    stream << in.header << in.receiveTime << in.sizeMessage;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, HdrCodNavData &out)
{
    stream >> out.header >> out.receiveTime >> out.sizeMessage;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const CodNavDataToFile &in)
{
    stream << in.hdrCodNavData << in.codNavData;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, CodNavDataToFile &out)
{
    stream >> out.hdrCodNavData >> out.codNavData;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const CodDataToFile &in)
{
    stream << in.hdrCodNavData << in.codData;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, CodDataToFile &out)
{
    stream >> out.hdrCodNavData >> out.codData;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const CodFrameData &in)
{
    stream << in.header
           << in.creationFrameTime
           << in.countLine
           << in.numberFrame
           << in.numberFirstLine
           << in.width
           << in.height
           << in.bits
           << in.frameArray;

    return  stream;
}

QDataStream &operator>>(QDataStream &stream, CodFrameData &out)
{
    stream >> out.header
           >> out.creationFrameTime
           >> out.countLine
           >> out.numberFrame
           >> out.numberFirstLine
           >> out.width
           >> out.height
           >> out.bits
           >> out.frameArray;

    return  stream;
}

// ================================= READ FILE =========================//

void testReadFile(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        CodNavDataToFile codNavDataToFile;
        QDataStream in(&file);

        while(!in.atEnd())
        {
            in >> codNavDataToFile;
        }
    }
    else
        qDebug() << "ERROR Open File:" << fileName;
}

template<typename T>
QDataStream &operator<<(QDataStream &stream, const DataToFile<T> &data)
{
    stream << data.hdrCodNavData
           << data.data;

    return stream;
}

template<typename T>
QDataStream &operator>>(QDataStream &stream, DataToFile<T> &data)
{
    stream >> data.hdrCodNavData
           >> data.data;

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const ImageDataToFile &data)
{
    stream << data.hdrCodNavData << data.image;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, ImageDataToFile data)
{
    stream >> data.hdrCodNavData >> data.image;
    return stream;
}

}
