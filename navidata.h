#ifndef CONNECTOR_H
#define CONNECTOR_H
#include <QThread>
#include <QTimer>
#include <QByteArray>
#include <QUdpSocket>
#include <QDataStream>
#include <QSharedDataPointer>
#include <QHostAddress>

#include "TelemetryData.pb.h"
#include "GeoData.pb.h"
#include "InfoPacket.pb.h"
#include "UavIdentity.pb.h"
#include "example/TransportHeader.h"
#include "common/structscod.h"
#include "common/common.h"
using namespace cod;

class NaviData : public QThread
{
    Q_OBJECT

public:
    NaviData();
    ~NaviData(){}

    void setDstPort(quint32 port);
    void setIpAddress(QString ipAddress);

    QString getGroupAddress(){ return groupIpAddress.toString(); }
    quint32 getMulticastDstPort() { return multicastDstPort; }

private:
    //
    QHostAddress srcIpAddress {"37.0.20.15"};
    quint32 srcPort {5800};
    //
    QHostAddress groupIpAddress {"225.0.0.1"};
    quint32 multicastDstPort    {2300};
    quint32 multicastSrcPort    {6001};
    //
    QHostAddress boardIpAddress {"37.0.21.15"};
    qint32 boardPort {50900};
    //
    QByteArray lastNaviData;
    quint64 packId{0};

    QSharedPointer<QUdpSocket> multicastSocket;
    QSharedPointer<QUdpSocket> socket;
    QSharedPointer<QTimer> sendNavTimer;
    QSharedPointer<QTimer> sendKeepAliveTimer;

    quint64 countKeepAliveMsg {};

    void readInitialData();
    void createLog(QString text);
    void creatProtobufNaviData(CodNavData &data);
    void creatTransportHeader(Rdmp::InfoPacket &info);

    // for test
    void deserializeNavData(QByteArray &sData);
    void ctrlSumm(const char* data, int size, const char* name);

public slots:
    void slotInit();
    void slotReadyRead();
    void slotSendNavData();
    void slotSendKeepAlive();
    void bindReceiveSocket();
    void bindMulticastSocket();
    void slotCnabgeStateSending(bool sign);

signals:
    void signalSendLog(QString);
    void signalWriteData(CodNavData);
    void signalWriteData(CodNavDataToFile);
    void signalWriteCodData(CodDataToFile);
};

#endif // CONNECTOR_H
