#ifdef none
#include "rdmpclient.h"
#include "syssock.h"
#include "logger.h"
#include "netfun.h"

#include "rdmp/TransportHeader.h"
#include "InfoPacket.pb-c.h"
#include "TelemetryData.pb-c.h"

#include <math.h>
#include <string.h>

#define EPSG_WGS84              4979
/// Период отправки координат НПУ/ОТ обратно в 2Д НПУ
#define SEND_BACK_PERIOD        60000

RDMPClient::RDMPClient (cEventMuxer *iEvMux)
{
    (evmux = iEvMux)->IncRef ();
}

RDMPClient::~RDMPClient ()
{
    UNREF (evmux);
}

void RDMPClient::run (uint32_t &timeout)
{
    if (!tcp)
    {
        gLog.Info ("Устанавливаем соединение с 2Д НПУ %s:%u\n",
                   ip4a2str (RDMP_HOST), RDMP_PORT);

        tcp = cTcpClient::Connect (RDMP_HOST, RDMP_PORT, 100);
        if (!tcp)
            return;

        buffTail = buffHead = 0;

        tcp->SetBlocking (false);
        tcp->RegisterEvent (evmux, EMF_READ);
    }

    sendBackNpu ();
    if (timeout > SEND_BACK_PERIOD)
        timeout = SEND_BACK_PERIOD;

    int n = tcp->Read (buff + buffHead, sizeof (buff) - buffHead);

    if (n == 0)
    {
        tcp->UnregisterEvent (evmux);
        UNREF (tcp);
        return;
    }

    if (n < 0)
        return;

    buffHead += n;

    while (buffTail < buffHead)
    {
        const uint8_t *data = buff + buffTail;
        uint size = buffHead - buffTail;
        if (size < sizeof (Rdmp::TransportHeader))
            break;

        Rdmp::TransportHeader *th = (Rdmp::TransportHeader *)data;
        uint32_t protobufLength = UINT32_LE (th->protobufLength);
        //gLog.Dump (th, sizeof (Rdmp::TransportHeader) + protobufLength, "Received RDMP data\n");

        if (sizeof (Rdmp::TransportHeader) + protobufLength > size)
            break;

        if (UINT16_LE (th->version) > Rdmp::TransportHeader::V_1_0)
            break;

        buffTail += sizeof (Rdmp::TransportHeader) + protobufLength;

        Rdmp__InfoPacket *info = rdmp__info_packet__unpack (NULL, protobufLength,
            data + sizeof (Rdmp::TransportHeader));
        if (info)
        {
            bool ack = true;
            gLog.Trace ("RDMP< Получен пакет тип %u, %lu байт\n", info->type, info->data.len);

            if (info->has_data)
                switch (info->type)
                {
                    case RDMP__INFO_PACKET__PACKET_TYPE__Notification:
                        ack = false;
                        break;

                    case RDMP__INFO_PACKET__PACKET_TYPE__Telemetry:
                    {
                        Rdmp__Telemetry *tele = rdmp__telemetry__unpack (NULL, info->data.len, info->data.data);
                        if (tele)
                        {
                            handleTelemetry (tele);
                            rdmp__telemetry__free_unpacked (tele, NULL);
                        }
                    }
                }

            // При работе с RDMP по протоколу TCP необходимо подтверждать приём каждого пакета
            if (ack)
                sendAck (info->id);

            rdmp__info_packet__free_unpacked (info, NULL);
        }
    }

    if (buffTail > 0)
    {
        memmove (buff, buff + buffTail, buffHead - buffTail);
        buffHead -= buffTail;
        buffTail = 0;
    }
}

void RDMPClient::sendInfoPacket (Rdmp__InfoPacket *info)
{
    size_t sz = rdmp__info_packet__get_packed_size (info);
    if (sz > rdmp_buff.Capacity ())
        rdmp_buff.SetCapacity ((sz + 127) & ~127);

    sz = rdmp__info_packet__pack (info, (uint8_t *)rdmp_buff.GetData ());
    if (sz <= 0)
        return;

    Rdmp::TransportHeader th;
    memset (&th, 0, sizeof (th));
    th.protobufLength = UINT32_LE (sz);
    th.version = Rdmp::TransportHeader::V_1_0;
    if (tcp->Write (&th, sizeof (th)) == sizeof (th) &&
        tcp->Write (rdmp_buff.GetData (), sz) == int (sz))
        gLog.Trace ("RDMP> Отправлен пакет тип %u, размер %u\n",
                    info->type, th.protobufLength);
}

void RDMPClient::sendAck (uint32_t packet_id)
{
    Rdmp__InfoPacket info;
    rdmp__info_packet__init (&info);
    info.type = RDMP__INFO_PACKET__PACKET_TYPE__Notification;
    info.id = rdmp_id = packet_id;

    sendInfoPacket (&info);
}

void RDMPClient::sendBackNpu ()
{
    uint32_t now = GetMs ();
    if (sendBackTimer.enabled () && !sendBackTimer.expired (now))
        return;

    sendBackTimer.arm (now, SEND_BACK_PERIOD);

    sendTelemetry (gLns.objects.GetByKey (csSomeU (NODEID_NPU)));
    sendTelemetry (gLns.objects.GetByKey (csSomeU (NODEID_LNSREF)));
}

void RDMPClient::sendTelemetry (LnsObject *node)
{
    if (!node || !node->nav.hasCoords ())
        return;

    Rdmp__UavIdentity id;
    rdmp__uav_identity__init (&id);
    id.id = node->node;
    id.has_type = true;
    id.type = (node->node == NODEID_NPU) ? RDMP__UAV_TYPE__GroundStation :
              (node->node == NODEID_LNSREF) ? RDMP__UAV_TYPE__LnsRefPoint :
                                              RDMP__UAV_TYPE__Orlan10;

    Rdmp__GeoCoordinates coord;
    rdmp__geo_coordinates__init (&coord);
    const NavInfo::Coord *c = node->nav.getCoords ();
    coord.latitude = c->latitude;
    coord.longitude = c->longitude;
    coord.altitude = c->altitude;

    Rdmp__Point position;
    rdmp__point__init (&position);
    position.geo = &coord;

    Rdmp__Telemetry tele;
    rdmp__telemetry__init (&tele);
    tele.timestamp = GetMs64 ();
    tele.id = &id;
    tele.position = &position;
    if (node->getAltitude (tele.aerialalt))
        tele.has_aerialalt = true;

    uint8_t tele_buff [128];
    if (rdmp__telemetry__get_packed_size (&tele) > sizeof (tele_buff))
        return;
    size_t tele_sz = rdmp__telemetry__pack (&tele, tele_buff);

    Rdmp__InfoPacket info;
    rdmp__info_packet__init (&info);
    info.type = RDMP__INFO_PACKET__PACKET_TYPE__Telemetry;
    info.id = ++rdmp_id;
    info.has_data = true;
    info.data.data = tele_buff;
    info.data.len = tele_sz;

    sendInfoPacket (&info);
}

void RDMPClient::handleTelemetry (Rdmp__Telemetry *tele)
{
    nodeid_t node_id = tele->id->id;

    if (tele->id->has_type)
        switch (tele->id->type)
        {
            case RDMP__UAV_TYPE__GroundStation:
                node_id = NODEID_NPU;
                sendBackTimer.disable ();
                break;

            case RDMP__UAV_TYPE__LnsRefPoint:
                node_id = NODEID_LNSREF;
                sendBackTimer.disable ();
                break;
        }

    LnsObject *obj = gLns.getObject (node_id);
    if (!obj)
        obj = gLns.createObject (node_id);

    if (!obj)
        return;

    uint32_t now = GetMs ();

    if (tele->has_aerialalt)
    {
        int altitude = (int)round (tele->aerialalt);
        gLog.Trace ("RDMP< борт %u, бар. высота %u\n", obj->node, altitude);
        obj->setAltitude (now, altitude);
    }

    if (tele->position)
    {
        Rdmp__GeoCoordinates *geo = tele->position->geo;
        if (geo &&
            ((!geo->has_epsg) || (geo->epsg == EPSG_WGS84)) &&
            ((geo->latitude != 0) || (geo->longitude != 0) || (geo->altitude != 0)))
        {
            gLog.Trace ("RDMP< борт %u, GPS шир %010.6f дол %010.6f выс %g\n",
                        obj->node, geo->latitude, geo->longitude, geo->altitude);
            NavInfo::Coord coord (now, NavInfo::Sputnik, geo->latitude, geo->longitude, geo->altitude);
            obj->updateCoords (now, coord);
        }
    }
}
#endif
