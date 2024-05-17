#ifdef none
#pragma once

#include "tcp.h"
#include "evmux.h"
#include "hydra-udp.h"
#include "ost.h"
#include "lnsserver.h"

// сервер RDMP всегда есть на 2600 и 2700 порту
#define RDMP_PORT       2700
// На localhost
#define RDMP_HOST       MK_IP(127,0,0,1)

typedef struct _Rdmp__Telemetry Rdmp__Telemetry;
typedef struct _Rdmp__InfoPacket Rdmp__InfoPacket;

/**
 * Клиент 2Д НПУ, вычитывающий текущие положения БЛА используя
 * протокол RDMP.
 */
class RDMPClient
{
    cTcpClient *tcp;
    cEventMuxer *evmux;
    uint8_t buff [4096];
    uint buffHead = 0, buffTail = 0;
    OneShotTimer sendBackTimer;
    uint32_t rdmp_id = 0;
    csString rdmp_buff;

    void sendAck (uint32_t packet_id);
    void sendInfoPacket (Rdmp__InfoPacket *info);
    void handleTelemetry (Rdmp__Telemetry *tele);
    // Периодически отправляет координаты НПУ и/или ОТ обратно в 2Д НПУ
    void sendBackNpu ();
    // Отправить телеметрию с координатами указанного объекта в 2Д НПУ
    void sendTelemetry (LnsObject *node);

public:
    RDMPClient (cEventMuxer *iEvMux);
    ~RDMPClient ();

    void run (uint32_t &timeout);
};
#endif
