#include "structseth.h"


QDataStream& operator<< (QDataStream& stream, const UsnData& in)
{
    stream << in.signature
       << in.creationPacketTime
       << in.length
       << in.durationDataProcessing
       << in.rezerv
       << in.courseAnglePeleng
       << in.targetCapture
       << in.operabilityUsn
       << in.needChangeTarget
       << in.rezerv1
       << in.rezerv2
       << in.pitchAnglePeleng
       << in.crc16;

    return stream;
}

QDataStream& operator>> (QDataStream& stream, UsnData& out)
{
    stream >> out.signature
        >> out.creationPacketTime
        >> out.length
        >> out.durationDataProcessing
        >> out.rezerv
        >> out.courseAnglePeleng
        >> out.targetCapture
        >> out.operabilityUsn
        >> out.needChangeTarget
        >> out.rezerv1
        >> out.rezerv2
        >> out.pitchAnglePeleng
        >> out.crc16;

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const TargetInfo &in)
{
    stream << in.coordXTargetCenter
           << in.coordYTargetCenter
           << in.isTracked
           << in.pelengCourse
           << in.pelengPitch;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, TargetInfo &out)
{
    stream  >> out.coordXTargetCenter
            >> out.coordYTargetCenter
            >> out.isTracked
            >> out.pelengCourse
            >> out.pelengPitch;

    return stream;
}

bool operator== (const TargetInfo& a, const TargetInfo& b)
{
    return ((a.coordXTargetCenter == b.coordXTargetCenter)
            && (a.coordYTargetCenter == b.coordYTargetCenter)
            && (a.isTracked == b.isTracked)
            && (a.pelengCourse == b.pelengCourse)
            && (a.pelengPitch == b.pelengPitch));
}

QDataStream &operator<<(QDataStream &stream, const UsnTarget &in)
{
    stream << in.signature
           << in.targets
           << in.isOkDataSaver
           << in.isOkUartTrasceiver
           << in.isOkThermalSensor
           << in.isOkImageProcessor;

    return stream;
}
QDataStream &operator>>(QDataStream &stream, UsnTarget &out)
{
    stream >> out.signature
            >> out.targets
            >> out.isOkDataSaver
            >> out.isOkUartTrasceiver
            >> out.isOkImageProcessor;

    return stream;
}

bool operator!= (const UsnTarget& a, const UsnTarget& b)
{
    return (a.signature != b.signature || a.targets != b.targets);
}

QDataStream &operator<<(QDataStream &stream, const SauData &in)
{
    stream << in.signature
       << in.creationPacketTime
       << in.length
       << in.coordXTargetPoint
       << in.coordYTargetPoint
       << in.coordZTargetPoint
       << in.radiusSearchArea
       << in.coordXDevice
       << in.coordYDevice
       << in.coordZDevice
       << in.yawning
       << in.pitch
       << in.roll
       << in.speedX
       << in.speedY
       << in.speedZ
       << in.changeTarget
       << in.rezerv
       << in.rezerv1
       << in.rezerv2
       << in.rezerv3
       << in.rezerv4
       << in.crc16;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, SauData &out)
{
    stream >> out.signature
            >> out.creationPacketTime
            >> out.length
            >> out.coordXTargetPoint
            >> out.coordYTargetPoint
            >> out.coordZTargetPoint
            >> out.radiusSearchArea
            >> out.coordXDevice
            >> out.coordYDevice
            >> out.coordZDevice
            >> out.yawning
            >> out.pitch
            >> out.roll
            >> out.speedX
            >> out.speedY
            >> out.speedZ
            >> out.changeTarget
            >> out.rezerv
            >> out.rezerv1
            >> out.rezerv2
            >> out.rezerv3
            >> out.rezerv4
            >> out.crc16;

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const ImageFrameGlobus &in)
{
    stream << in.signature
           << in.creationPacketTime
           << in.countLine
           << in.numberFrame
           << in.numberFirstLine
           << in.width
           << in.height
           << in.bits
           << in.imageArray;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, ImageFrameGlobus &out)
{
    stream >> out.signature
            >> out.creationPacketTime
            >> out.countLine
            >> out.numberFrame
            >> out.numberFirstLine
            >> out.width
            >> out.height
            >> out.bits
            >> out.imageArray;

    return stream;
}
