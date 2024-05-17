#ifndef COMMON_H
#define COMMON_H
#include <QDataStream>

struct NavData
{
    char hdr[3] = {'N','A','V' };
    double altitude;
    double roll;
    double pitch;
    double latitude;
    double longitude;

    NavData(){}
    NavData(double al, double ro, double pi, double la, double lo);
};
QDataStream& operator << (QDataStream &stream, const NavData &data);
QDataStream& operator >> (QDataStream &stream, NavData &data);

#endif // COMMON_H


/*
struct HdrCodNavData
{
    uint16_t header;        //!< 0xС0DA - наземная
    uint32_t receiveTime;   //!< Время получения сообщения, мс
    uint16_t sizeMessage;   //!< Размер сообщения
};

struct CodNavData
{
    HeaderCodData header;   //!< заголовок сообщения с данными
    uint32_t id;            //!< id сообщения
    double latitude;        //!< широта, градусы
    double longitude;       //!< долгота, градусы
    double altitude;        //!< высота, м
    double roll;            //!< крен, градусы
    double pitch;           //!< тангаж, градусы
    double yaw;             //!< рысканье, градусы
    double gpsCourse;       //!< путевой угол, градусы
    double gpsSpeed;        //!< путевая скорость, м/с
    double aerialSpeed;     //!< воздушная скорость, м/с
    double aerialAlt;       //!< барометрическая абсолютная высота, м
    double verticalSpeed;   //!< вертикальная скорость, м/с

    CodNavData()  { header.id = MessageId::BOARD_NAV_DATA; }
};



struct HdrCodNavData
{
    uint16_t header;        //!< 0xС0DA - наземная
    uint32_t receiveTime;   //!< Время получения сообщения, мс
    uint16_t sizeMessage;   //!< Размер сообщения
};

struct CodData
{
    HeaderCodData header;   //!< заголовок сообщения с данными
    uint32_t id;            //!< id сообщения

    CodData()  { header.id = MessageId::GROUND_COD_DATA; }
};

*/
