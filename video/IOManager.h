#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <vector>
#include <string>

class IOManager
{
public:
    static bool readToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
    static bool readToBuffer(std::vector<uint8_t> colorsVector, std::vector<unsigned char>& buffer, int sizeX, int sizeY);
};

#endif // IOMANAGER_H
