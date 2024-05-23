#include <QDateTime>
#include <QSettings>
#include <QString>
#include "navidata.h"

#define BLA_ID   21         // идентификатор БЛА    // 439
#define BLA_TYPE 0x00100001 // тип Orlan10_U
#define BLA_DESC "RA-0439G" // описание
#define WGS_84    4326      // система координат WGS_84
#define WGS_World 4979      // система координат WGS_World
#define TYPE_PACK 7         // тип пакета
#define ARR_SIZE 128        // размера массива для сериализации данных

NaviData::NaviData()
{
    connect(this, &NaviData::started, this, &NaviData::slotInit);
    this->moveToThread(this);
    this->start();
}

void NaviData::slotInit()
{
    multicastSocket = QSharedPointer<QUdpSocket>(new QUdpSocket);
    socket = QSharedPointer<QUdpSocket>(new QUdpSocket);
    sendNavTimer = QSharedPointer<QTimer>(new QTimer);
    sendKeepAliveTimer = QSharedPointer<QTimer>(new QTimer);

    connect(socket.data(), &QUdpSocket::readyRead, this, &NaviData::slotReadyRead);
    connect(sendNavTimer.data(), &QTimer::timeout, this, &NaviData::slotSendNavData);
    connect(sendKeepAliveTimer.data(), &QTimer::timeout, this, &NaviData::slotSendKeepAlive);
    connect(this, &QThread::finished, sendNavTimer.data(), &QTimer::stop);
    connect(this, &QThread::finished, sendKeepAliveTimer.data(), &QTimer::stop);

    readInitialData();

    sendNavTimer->setInterval(1000);
    sendKeepAliveTimer->start(500);
}

void NaviData::readInitialData()
{
    QSettings settings("initial.ini", QSettings::IniFormat);

    srcIpAddress.setAddress(settings.value("srcIpAddress").toString());
    srcPort = settings.value("srcPort").toInt();

    groupIpAddress.setAddress(settings.value("groupIpAddress").toString());
    multicastDstPort = settings.value("multicastDstPort").toInt();
    multicastSrcPort = settings.value("multicastSrcPort").toInt();

    boardIpAddress.setAddress(settings.value("boardIpAddress").toString());
    boardPort = settings.value("boardPort").toInt();
}

void NaviData::creatProtobufNaviData(CodNavData &data)
{
    quint64 unixTimeMs = QDateTime::currentMSecsSinceEpoch();

    Rdmp::UavIdentity id;
    id.Clear();
    id.set_id(BLA_ID);
    id.set_type(BLA_TYPE);
    id.set_description(BLA_DESC);

    Rdmp::GeoCoordinates coord;
    coord.Clear();
    coord.set_latitude(55);
    coord.set_longitude(data.longitude);
    coord.set_altitude(data.altitude);
    coord.set_epsg(WGS_84);

    Rdmp::Point position;
    position.Clear();
    position.set_allocated_geo(&coord);

    Rdmp::Telemetry tele;
    tele.Clear();
    tele.set_timestamp(unixTimeMs);
    tele.set_allocated_id(&id);
    tele.set_allocated_position(&position);
    tele.set_gpscourse(data.gpsCourse);
    tele.set_gpsspeed(data.gpsSpeed);
    tele.set_aerialspeed(data.aerialSpeed);
    tele.set_aerialalt(data.aerialAlt);
    tele.set_verticalspeed(data.verticalSpeed);

    std::string serializeAsString;
    serializeAsString.clear();
    bool res = tele.SerializeToString(&serializeAsString);

    packId++;
    packId = packId == 0 ? 1 : packId;

    Rdmp::InfoPacket info;
    info.Clear();
    info.set_type(TYPE_PACK);
    info.set_id(BLA_ID);                // packId
    info.set_data(serializeAsString);

    position.release_geo();
    tele.release_id();
    tele.release_position();

    creatTransportHeader(info);
}

void NaviData::creatTransportHeader(Rdmp::InfoPacket &info)
{
    Rdmp::TransportHeader th{};

    th.protobufLength = info.ByteSize();
    th.version = Rdmp::TransportHeader::V_1_0;

    QByteArray buff;
    buff.clear();
    QDataStream stream(&buff, QIODevice::WriteOnly);
    stream.writeRawData(reinterpret_cast<const char*>(&th), sizeof(Rdmp::TransportHeader));
    stream.writeRawData(reinterpret_cast<const char*>(&info), th.protobufLength);

    lastNaviData = buff;

    info.release_data();

    // CHECK
    deserializeNavData(lastNaviData);
}

void NaviData::deserializeNavData(QByteArray &sData)
{
    Rdmp::InfoPacket info{};
    Rdmp::TransportHeader th{};
    //
    QDataStream stream(&sData, QIODevice::ReadOnly);
    stream.readRawData(reinterpret_cast<char*>(&th), sizeof(Rdmp::TransportHeader));
    stream.readRawData(reinterpret_cast<char*>(&info), th.protobufLength);
    //
    Rdmp::Telemetry tele{};
    bool res = tele.ParseFromString(info.data());

    qDebug().noquote() << qSetFieldWidth(15) << left << '\n'
             << "protobufLength" << th.protobufLength << '\n'
             << "version" << th.version << '\n'
             << "type" << info.type() << '\n'
             << "pack_id" << info.id() << '\n'
             << "timestamp" << tele.timestamp() << '\n'
             << "БЛА_id" << tele.id().id() << '\n'
             << "БЛА_type" << tele.id().type() << '\n'
             << "БЛА_desc" << tele.id().description().c_str() << '\n'
             << "Широта" << tele.position().geo().latitude() << '\n'
             << "Долгота" << tele.position().geo().longitude() << '\n'
             << "Высота" << tele.position().geo().altitude() << '\n'

             << "Путевой угол" << tele.gpscourse() << '\n'
             << "Путевая скор." << tele.gpsspeed() << '\n'
             << "Воздушная скор." << tele.aerialspeed() << '\n'
             << "Баром. высота" << tele.aerialalt() << '\n'
             << "Верт. скорость" << tele.verticalspeed() << '\n'

             << "Coord_system" << tele.position().geo().epsg() << '\n'
             << "==============================================";

    info.release_data();
}

void NaviData::slotReadyRead()
{
    while(socket->hasPendingDatagrams())
    {
        QByteArray buff;
        buff.resize(socket->pendingDatagramSize());
        socket->readDatagram(buff.data(), buff.size());

        if(cod::calcChecksum(buff.data(), buff.size()) ==
                static_cast<uint8_t>(buff[cod::indexCheckSum]))
        {
            CodNavData navData;
            getDataFromByteArray(navData, buff);

            emit createLog(QString("Получен пакет № %1").arg(navData.id));
            emit signalWriteData(navData);
            emit signalWriteData(CodNavDataToFile(navData, buff.size()));

            creatProtobufNaviData(navData);
        }
        else
            emit createLog(QString("Принятый пакет поврежден!"));
    }
}

void NaviData::slotSendNavData()
{
    if(lastNaviData.size() > 0)
        multicastSocket->writeDatagram(lastNaviData, groupIpAddress, multicastDstPort);
}

void NaviData::slotSendKeepAlive()
{
    CodData codData;
    codData.id = ++countKeepAliveMsg;
    codData.header.time = QDateTime::currentMSecsSinceEpoch();

    socket->writeDatagram(getSendedByteArray(codData), boardIpAddress, boardPort);

    emit signalWriteCodData(CodDataToFile(codData, sizeof(cod::CodData)));
}

void NaviData::slotCnabgeStateSending(bool sign)
{
    if(sign == true)
    {
        sendNavTimer->start();
        createLog("Старт передачи");
    }
    else
    {
        sendNavTimer->stop();
        createLog("Стоп передачи");
    }
}

void NaviData::createLog(QString text)
{
    QDateTime dt = QDateTime::currentDateTime();
    text = dt.toString("[hh:mm:ss] ") + text + '\n';

    emit signalSendLog(text);
}

void NaviData::setDstPort(quint32 port)
{
    multicastDstPort = port;
}

void NaviData::setIpAddress(QString ipAddress)
{
    groupIpAddress = ipAddress;
}

void NaviData::bindReceiveSocket()
{
    socket->close();

    if(socket->bind(srcIpAddress, srcPort))
        createLog(QString("Прослушивание порта получения: %1").arg(srcPort));
    else
        createLog(QString("%1:%2 - ").arg(srcPort).arg(srcIpAddress.toString())+socket->errorString());
}

void NaviData::bindMulticastSocket()
{
    multicastSocket->close();

    if(multicastSocket->bind(QHostAddress::AnyIPv4, multicastSrcPort, QUdpSocket::ShareAddress) &&
       multicastSocket->joinMulticastGroup(groupIpAddress))
    {
        createLog(QString("Привязка порта мультикаста: %1").arg(multicastSrcPort));
    }
    else
        createLog(QString("%1:%2 - ").arg(multicastSrcPort).arg(groupIpAddress.toString())+multicastSocket->errorString());
}

#include <iostream>
void NaviData::ctrlSumm(const char *data, int size, const char *name)
{
    int ctrlSumm{};
    const char* ptr = data;
    for(int i{}; i<size; i++)
    {
        std::cout << (int)*ptr;

        ctrlSumm += *ptr;
        ptr++;
    }
    std::cout << '\n';
    qDebug() << QString("%1: %2, %3").arg(name).arg(ctrlSumm).arg(size);
}


