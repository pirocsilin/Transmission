#include "videoframegenerator.h"
#include <QPolygon>
#include <QPoint>
#include <QFile>
#include <QTextStream>

VideoFrameGenerator::VideoFrameGenerator(QObject *parent)
{
    generatedImage = new QImage(frameWidth, frameHeight, QImage::Format_Grayscale8);
    painter = new QPainter(generatedImage);

    QPen pen;
    pen.setColor(Qt::white);

    QBrush brush;
    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);

    painter->setBrush(brush);
    painter->setPen(pen);
}

/**
 * @brief Построение картинки для прямоугольника
 * @param targetCenterX     координата Х центра прямоугольника
 * @param targetCenterY     координата Y центра прямоугольника
 * @param targetWidth       ширина цели
 * @param targetHeight      высота цели
 * @param pixelBrightness   яркость пикселей цели [0-255]
 * @return
 */
QImage VideoFrameGenerator::createImageForRectTarget(int targetCenterX, int targetCenterY,
                                                      int targetWidth,   int targetHeight,
                                                      uint8_t pixelBrightness)
{
    if(targetCenterX >= frameWidth)
        targetCenterX = frameWidth - 1;
    else if(targetCenterX < 0)
        targetCenterX = 0;

    if(targetCenterY >= frameHeight)
        targetCenterY = frameHeight - 1;
    else if(targetCenterY < 0)
        targetCenterY = 0;

    QByteArray buf(frameWidth*frameHeight, 20);
    int halfTargetWidth  = targetWidth/2;
    int halfTargetHeight = targetHeight/2;

    int minY = std::max(targetCenterY - halfTargetHeight, 0);
    int maxY = std::min(targetCenterY + halfTargetHeight, frameHeight);

    int minX = std::max(targetCenterX - halfTargetWidth, 0);
    int maxX = std::min(targetCenterX + halfTargetWidth, frameWidth);

    for(int row = minY; row < maxY; ++row)
    {
        for(int col = minX; col < maxX; ++col)
        {
            int index = row*frameWidth + col;
            index = std::min(frameWidth*frameHeight - 1, index);
            index = std::max(0, index);
            buf[index] = pixelBrightness;
        }
    }

    QImage ret (reinterpret_cast<uint8_t*>(buf.data()),
                frameWidth, frameHeight, QImage::Format_Grayscale8);

#if 0
    ret.save("testimage.bmp", "BMP");
#endif

    return ret;
}

/**
 * @brief Построение картинки для произвольного четырехугольника
 * @param targetX1
 * @param targetY1
 * @param targetX2
 * @param targetY2
 * @param targetX3
 * @param targetY3
 * @param targetX4
 * @param targetY4
 * @return
 */
QImage VideoFrameGenerator::createImageForTetragonTarget(int targetX1, int targetY1,
                                                         int targetX2, int targetY2,
                                                         int targetX3, int targetY3,
                                                         int targetX4, int targetY4)
{
    generatedImage->fill(0);
    target.clear();

    target << QPoint(targetX1, targetY1) << QPoint(targetX2, targetY2)
           << QPoint(targetX3, targetY3) << QPoint(targetX4, targetY4);
    painter->drawPolygon(target);

#if 0
    generatedImage->save("testimage.bmp", "BMP");
#endif

    return *generatedImage;
}

void VideoFrameGenerator::readTargetsFromFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!file.atEnd())
        parseTargetLine(in.readLine());
}

void VideoFrameGenerator::parseTargetLine(QString line)
{
    QStringList targetsInfo = line.split(" ");
    if(targetsInfo.size() == 9)
    {
        TargetShape newTarget;
        newTarget.id      = targetsInfo.at(0).toInt();
        newTarget.coordX1 = targetsInfo.at(1).toInt();
        newTarget.coordY1 = targetsInfo.at(2).toInt();
        newTarget.coordX2 = targetsInfo.at(3).toInt();
        newTarget.coordY2 = targetsInfo.at(4).toInt();
        newTarget.coordX3 = targetsInfo.at(5).toInt();
        newTarget.coordY3 = targetsInfo.at(6).toInt();
        newTarget.coordX4 = targetsInfo.at(7).toInt();
        newTarget.coordY4 = targetsInfo.at(8).toInt();
        targets.push_back(newTarget);
    }
}
