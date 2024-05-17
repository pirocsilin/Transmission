#ifdef none
#include "TransportHeader.h"

void cLNS::HandleRDMP (uint8_t *data, uint size)
{
    if (size < sizeof (Rdmp::TransportHeader))
        return;

    Rdmp::TransportHeader *th = (Rdmp::TransportHeader *)data;

    if (sizeof (Rdmp::TransportHeader) + th->protobufLength > size)
        return;

    if (th->version > Rdmp::TransportHeader::V_1_0)
        return;

    Rdmp__InfoPacket *info = rdmp__info_packet__unpack (NULL, th->protobufLength,
        data + sizeof (Rdmp::TransportHeader));
    if (info)
    {
        if (info->has_data)
            switch (info->type)
            {
                case RDMP__INFO_PACKET__PACKET_TYPE__Telemetry:
                {
                    Rdmp__Telemetry *tele = rdmp__telemetry__unpack (NULL, info->data.len, info->data.data);
                    if (tele)
                    {
                        cLNSObject *obj = Get (tele->id->id, true);
                        if (obj)
                        {
                            DPRINTF (D_LNS, "RDMP: борт %u\n", tele->id->id);
                            obj->rdmp_stamp = obj->last_active_time = GetMs ();

                            if (tele->has_aerialalt)
                            {
                                int altitude = (int)round (tele->aerialalt);
                                DPRINTF (D_LNS, "RDMP: бар. высота %u\n", altitude);
                                obj->SetAltitude (altitude);
                            }

                            if (tele->position)
                            {
                                Rdmp__GeoCoordinates *geo = tele->position->geo;
                                if (geo &&
                                    ((!geo->has_epsg) || (geo->epsg == EPSG_WGS84)) &&
                                    ((geo->latitude != 0) || (geo->longitude != 0) || (geo->altitude != 0)))
                                {
                                    DPRINTF (D_LNS, "RDMP: борт %u, GPS шир %010.6f дол %010.6f выс %g\n",
                                        obj->xy, geo->latitude, geo->longitude, geo->altitude);
                                    obj->SetGPSCoord (geo->latitude, geo->longitude, geo->altitude);
                                }
                            }
                        }

                        rdmp__telemetry__free_unpacked (tele, NULL);
                    }
                }
            }

        rdmp__info_packet__free_unpacked (info, NULL);
    }
}
#endif
