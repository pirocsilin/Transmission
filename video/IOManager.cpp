#include "IOManager.h"

#include <fstream>
#include <QBuffer>
#include <QImage>
#include <QDebug>

bool IOManager::readToBuffer(std::string filePath, std::vector<unsigned char> &buffer)
{
    std::ifstream file(filePath, std::ios::binary);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    // seek to tht end
    file.seekg(0, std::ios::end);

    // get the file size
    long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Reduce the file size by any header bytes that might be present
    fileSize -= file.tellg();

    buffer.resize(static_cast<unsigned long>(fileSize));

    file.read((char*)&(buffer[0]), fileSize);
    file.close();
    return true;
}

bool IOManager::readToBuffer(std::vector<uint8_t> colorsVector, std::vector<unsigned char> &buffer, int sizeX, int sizeY)
{
    QImage image = QImage(sizeX, sizeY, QImage::Format::Format_Grayscale8);
    unsigned long pixel = 0;
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            image.setPixel(i, j, qRgb(colorsVector[pixel], colorsVector[pixel], colorsVector[pixel]));
            pixel++;
        }
    }
    QByteArray byteArray;
    QBuffer qbuffer(&byteArray);
    image.save(&qbuffer, "PNG");

    const unsigned char* begin = reinterpret_cast<unsigned char*>(byteArray.data());
    const unsigned char* end = begin + byteArray.length();
    buffer = std::vector<unsigned char>( begin, end );


}
