#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include "common/common.h"
#include "common/structscod.h"
using namespace cod;

class Transmission : public QThread
{
    Q_OBJECT

public:
    Transmission();

private:
    const QString dstIpAddress  {"127.0.0.1"};
    const quint32 dstPort       {5800};

    uint32_t id {0};

    QSharedPointer<QUdpSocket> socket;
    QSharedPointer<QTimer> sendTimer;

    quint8 mutex{10};

public slots:
    void slotInit();
    void slotSendData();
};

#endif // TRANSMISSION_H
