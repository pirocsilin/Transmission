#ifndef STRUCTSETH_H
#define STRUCTSETH_H

#include <stdint.h>
#include <stddef.h>
#include <QDataStream>
#include <QVector>

struct UsnData
{
    uint16_t signature;				//!< признак пакета 0xDAED
    qint64   creationPacketTime; 	//!< время формирования пакета от запуска УСН, мкс
    uint8_t  length; 				//!< длина пакета, байт
    qint64   durationDataProcessing;//!< продолжительность обработки данных, мкс
    int   	 rezerv;	   		    //!< резерв
    int16_t courseAnglePeleng;		//!< сигнал пеленга по углу курса, град
    uint8_t  targetCapture;  		//!< признак захвата цели ЗЦ
    uint8_t  operabilityUsn; 		//!< сигнал исправлности УСН
    uint8_t  needChangeTarget; 		//!< признак необходимости перенацеливания ПЦ
    uint8_t  rezerv1;        		//!< резерв
    uint16_t rezerv2;				//!< резерв
    int16_t pitchAnglePeleng;		//!< сигнал пеленга по углу тангажа, град
    uint16_t crc16;					//!< контрольная сумма, crc16
};

QDataStream& operator<< (QDataStream& stream, const UsnData& in);
QDataStream& operator>> (QDataStream& stream, UsnData& out);

struct TargetInfo
{
    uint16_t coordXTargetCenter;
    uint16_t coordYTargetCenter;
    bool isTracked;
    float pelengCourse;
    float pelengPitch;
};

QDataStream& operator<< (QDataStream& stream, const TargetInfo& in);
QDataStream& operator>> (QDataStream& stream, TargetInfo& out);

bool operator== (const TargetInfo& a, const TargetInfo& b);

struct UsnTarget
{
    uint16_t signature;				//!< признак пакетa 0xBEED
    QVector<TargetInfo> targets;	//!< список обнаруженных целей
    bool isOkDataSaver;
    bool isOkUartTrasceiver;
    bool isOkThermalSensor;
    bool isOkImageProcessor;
};

QDataStream& operator<< (QDataStream& stream, const UsnTarget& in);
QDataStream& operator>> (QDataStream& stream, UsnTarget& out);
bool operator!= (const UsnTarget& a, const UsnTarget& b);

struct SauData
{
    uint16_t signature;				//!< признак пакета 0xCEBA
    qint64   creationPacketTime; 	//!< время формирования пакета от запуска САУ, мкс
    uint8_t  length; 				//!< длина пакета, байт
    uint32_t coordXTargetPoint;		//!< координата Х точки прицеливания в НЗСК, м
    uint32_t coordYTargetPoint;		//!< координата Y точки прицеливания в НЗСК, м
    uint32_t coordZTargetPoint;		//!< координата Z точки прицеливания в НЗСК, м
    uint32_t radiusSearchArea;		//!< значение радиуса зоны поиска цели, м
    uint32_t coordXDevice;			//!< координата Х изделия в НЗСК, м
    uint32_t coordYDevice;			//!< координата Y изделия в НЗСК, м
    uint32_t coordZDevice;			//!< координата Z изделия в НЗСК, м
    uint32_t yawning;				//!< рысканье, рад
    uint32_t pitch;					//!< тангаж, рад
    uint32_t roll;					//!< крен, рад
    uint32_t speedX;				//!< составляющая Х полного вектора скорости изделия по осям НСК, рад
    uint32_t speedY;				//!< составляющая Y полного вектора скорости изделия по осям НСК, рад
    uint32_t speedZ;				//!< составляющая Z полного вектора скорости изделия по осям НСК, рад
    uint8_t  changeTarget;			//!< признак перенацеливания
    uint8_t  rezerv;				//!< резерв
    uint8_t  rezerv1;				//!< резерв
    uint8_t  rezerv2;				//!< резерв
    uint16_t rezerv3;				//!< резерв
    uint16_t rezerv4;				//!< резерв
    uint16_t crc16;					//!< контрольная сумма, crc16
};

QDataStream& operator<< (QDataStream& stream, const SauData& in);
QDataStream& operator>> (QDataStream& stream, SauData& out);

struct ImageFrameGlobus
{
    uint16_t signature {0xDDAA};    //!< заголовок кадра
    uint32_t creationPacketTime;    //!< время создания пакета
    uint8_t countLine;              //!< количество строк в пакете
    uint32_t numberFrame;           //!< номер текущего кадра
    uint16_t numberFirstLine;       //!< номер первой строки
    uint16_t width;                 //!< ширина кадра в пикселях
    uint16_t height;                //!< высота кадра в пикселях
    uint8_t bits;                   //!< количество бит на пиксель
    QByteArray imageArray;          //!< пиксели изображения построчно
};

QDataStream& operator<< (QDataStream& stream, const ImageFrameGlobus& in);
QDataStream& operator>> (QDataStream& stream, ImageFrameGlobus& out);

#endif
