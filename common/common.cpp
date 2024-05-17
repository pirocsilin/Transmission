#include "common.h"


QDataStream &operator <<(QDataStream &stream, const NavData &data)
{
    stream.writeRawData(data.hdr, sizeof(data.hdr));
    stream << data.altitude
           << data.roll
           << data.pitch
           << data.latitude
           << data.longitude;

    return stream;
}

QDataStream &operator >>(QDataStream &stream, NavData &data)
{
    stream.readRawData(data.hdr, sizeof(data.hdr));
    stream >> data.altitude
           >> data.roll
           >> data.pitch
           >> data.latitude
           >> data.longitude;

    return stream;
}

NavData::NavData(double al, double ro, double pi, double la, double lo)
{
    altitude = al;
    roll = ro;
    pitch = pi;
    latitude = la;
    longitude = lo;
}
