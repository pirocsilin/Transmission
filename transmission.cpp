
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QDateTime>
#include "transmission.h"

Transmission::Transmission()
{
    connect(this, &Transmission::started, this, &Transmission::slotInit);
    this->moveToThread(this);
    this->start();
}

void Transmission::slotInit()
{
    socket = QSharedPointer<QUdpSocket>(new QUdpSocket);
    sendTimer = QSharedPointer<QTimer>(new QTimer);

    connect(sendTimer.data(), &QTimer::timeout, this, &Transmission::slotSendData);
    connect(this, &QThread::finished, sendTimer.data(), &QTimer::stop);

    sendTimer->start(500);
}
void Transmission::slotSendData()
{
    (++mutex) %= 10;

    CodNavData data;
    data.header.time = QDateTime::currentMSecsSinceEpoch();
    data.id = ++id;
    data.latitude = 45.234234+mutex;
    data.longitude = 55.43234+mutex;
    data.altitude = 940.23+mutex;
    data.roll = 12.23432+mutex;
    data.pitch = 9.0978+mutex;
    data.yaw = 5.0978+mutex;
    data.gpsCourse = 34.6534+mutex;
    data.gpsSpeed = 34.0558+mutex;
    data.aerialSpeed = 12.3378+mutex;
    data.aerialAlt = 1010.5531+mutex;
    data.verticalSpeed = 20.5323+mutex;

    QByteArray buff = getSendedByteArray(data);
    //qint64 size = buff.size();
    qint64 rezult = socket->writeDatagram(buff, QHostAddress(dstIpAddress), dstPort);
}














