#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QFile>
#include "common.h"
#include "structscod.h"
using namespace cod;

#define DATE_TIME "yyyy-MM-dd[hh-mm-ss]"

typedef unsigned long DWORD;

class Logger : public QThread
{
    Q_OBJECT

public:

    Logger();
    ~Logger();

    void initFile();
    void closeFile();

private:

    QString fileStart {"Nav_Data"};
    QString expansion {".dat"};
    QString fileName;
    QFile file;

    bool writeIsAllow;

    void createLog(QString text);

public slots:
    void slotWriteNavData(CodNavDataToFile data);
    void slotWriteCodData(CodDataToFile data);
    void slotSwitchStateRecord(bool state);

signals:
    void signalSendLog(QString msg);
    void failInitFile();
};

#endif // LOGGER_H
