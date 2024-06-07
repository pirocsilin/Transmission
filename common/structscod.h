#ifndef STRUCTS_COD_H
#define STRUCTS_COD_H

#include <stdint.h>
#include <stddef.h>
#include <QDataStream>
#include <QDateTime>
#include <QImage>
#include <QFile>
#include <QDebug>

namespace cod
{
const int indexMessageId    {1};   //!< индекс типа сообщения в структуре
const int indexCheckSum     {2};   //!< индекс контрольной суммы в структуре
const int minSizeCodMessage {7};   //!< минимальный размер сообщения

#pragma pack(push, 1)

/**
 * @brief Расчет контрольной суммы сообщения
    Суммирование всех байтов сообщения без переноса старшего байта
    Пример использования:
    uint8_t checkSum = calcChecksum(var.hdr, sizeof(var));
 * @param in указатель на начало структуры
 * @param sz размер структуры
 * @return значение контрольной суммы
 */
uint8_t calcChecksum(const char *in, size_t sz);

/**
 * @brief Идентификатор для каждого сообщения между БЦВМ и МФИ
 */
enum class MessageId : uint8_t
{
    NONE = 0,

    //! данные от бортового модуля
    BOARD_NAV_DATA,                 //!< навигационные данные для ОПУ
    BOARD_FRAME_DATA,               //!< пакет с кадром изображения
    BOARD_COD_KEEPALIVE,            //!< сообщение о наличии соединения с КОД-Б

    //! данные от наземного модуля
    GROUND_COD_DATA,    //!< данные
};

QDataStream& operator<< (QDataStream& stream, const MessageId& in);
QDataStream& operator>> (QDataStream& stream, MessageId& out);

/**
 * @brief Заголовок сообщений для данных в КОД
 */
struct HeaderCodData
{
    uint8_t     syncByte {0xCD};    //!< синхробайт (0xCD)
    MessageId   id;                 //!< идентификатор сообщения
    uint8_t	    checkSum {};        //!< контрольная сумма сериализованной структуры (сумма всех байтов)
    uint32_t    time {};            //!< время формирования сообщения, мс
};

QDataStream& operator<< (QDataStream& stream, const HeaderCodData& in);
QDataStream& operator>> (QDataStream& stream, HeaderCodData& out);

/**
 * @brief Навигационные данные из МФИ для КОД
 */
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

QDataStream& operator<< (QDataStream& stream, const CodNavData& in);
QDataStream& operator>> (QDataStream& stream, CodNavData& out);

/**
 * @brief Заголовок навигационных данных для записи в файл
 */
struct HdrCodNavData
{
    uint16_t header;        //!< Заголовок (0xС0DB - бортовая часть, 0xС0DA - наземная)
    uint32_t receiveTime;   //!< Время получения сообщения, мс
    uint16_t sizeMessage;   //!< Размер сообщения
};

QDataStream& operator<< (QDataStream& stream, const HdrCodNavData& in);
QDataStream &operator>> (QDataStream& stream, HdrCodNavData& out);

struct CodFrameData
{
    HeaderCodData header;           //!< заголовок сообщения
    uint32_t creationFrameTime;     //!< время формерования кадра
    uint8_t countLine;              //!< количество строк в пакете
    uint32_t numberFrame;           //!< номер текущего кадра
    uint16_t numberFirstLine;       //!< номер первой строки в пакете
    uint16_t width;                 //!< ширина кадра в пикселях
    uint16_t height;                //!< высота кадра в пикселях
    uint8_t bits;                   //!< количество бит на пиксель
    QByteArray frameArray;          //!< пиксели изображения построчно

    CodFrameData()  { header.id = MessageId::BOARD_FRAME_DATA; }
};

QDataStream &operator<< (QDataStream& stream, const CodFrameData& in);
QDataStream& operator>> (QDataStream& stream, CodFrameData& out);

struct CodData
{
    HeaderCodData header;   //!< заголовок сообщения с данными
    uint32_t id;            //!< id сообщения
    CodData()  { header.id = MessageId::GROUND_COD_DATA; }
};

QDataStream& operator<< (QDataStream& stream, const CodData& in);
QDataStream& operator>> (QDataStream& stream, CodData& out);

#pragma pack(pop)

template <class T>
QByteArray getSendedByteArray(const T& data)
{
    QByteArray ret;
    QDataStream out(&ret, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out.setByteOrder(QDataStream::BigEndian);

    out << data;
    ret[cod::indexCheckSum] = cod::calcChecksum(ret.data(), ret.size());
    return ret;
}

template <class T>
void getDataFromByteArray(T& data, const QByteArray& source)
{
    QDataStream in(source);
    in.setVersion(QDataStream::Qt_5_7);
    in.setByteOrder(QDataStream::BigEndian);
    in >> data;
}

template <class T>
QByteArray getDataForWriteToFile(T& data, int size)
{
    QByteArray ret;
    QDataStream out(&ret, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out.setByteOrder(QDataStream::BigEndian);

    HdrCodNavData hdr;
    hdr.header = 0xC0DA;
    hdr.receiveTime = QDateTime::currentMSecsSinceEpoch();
    hdr.sizeMessage = size;

    out << hdr << data;

    return ret;
}

/**
 * @brief isValidHeaderStructBcvm
 * @param data полученный массив данных от БЦВМ
 * @return true - заголовок структуры валидный
 */
bool isValidHeaderStructCod(const QByteArray &data);

}
#endif // STRUCTS_COD_H
