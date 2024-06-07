#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QDataStream>
#include <QDateTime>
#include "logger.h"

#define DATE_TIME "yyyy-MM-dd[hh-mm-ss]"

Logger::Logger()
{
    this->moveToThread(this);
    this->start();

    writeIsAllow    = false;
}

Logger::~Logger()
{
    closeFile();
}

void Logger::initFile()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    fileName = fileStart + "_" + currentDateTime.toString(DATE_TIME) + expansion;

    file.setFileName(fileName);

    if(!file.open(QIODevice::ReadWrite)) // QIODevice::Append
    {
        createLog("Error: Ошибка открытия файла "+fileStart);
        emit failInitFile();
        closeFile();
    }
    else
    {
        createLog("Файл открыт: "+fileStart+"...dat");
    }
}

void Logger::closeFile()
{
    if(file.isOpen())
    {
        file.close();
        createLog("Файл закрыт: "+fileStart+"...dat");
    }
}

void Logger::createLog(QString text)
{
    QDateTime dt = QDateTime::currentDateTime();
    text = dt.toString("[hh:mm:ss] ") + text + '\n';

    emit signalSendLog(text);
}

void Logger::slotWriteToFile(QByteArray data)
{
    if(writeIsAllow && file.isOpen())
    {
        QDataStream out(&file);
        out.writeRawData(data.data(), data.size());
    }
}

void Logger::slotSwitchStateRecord(bool state)
{
    writeIsAllow = state;

    if(writeIsAllow == true)
        initFile();
    else
        closeFile();
}

