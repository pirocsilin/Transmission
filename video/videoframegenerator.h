#ifndef VIDEOFRAMEGENERATOR_H
#define VIDEOFRAMEGENERATOR_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QImage>
#include <QPainter>

class VideoFrameGenerator : public QObject
{
    struct TargetShape
    {
        int id;
        int coordX1;
        int coordY1;
        int coordX2;
        int coordY2;
        int coordX3;
        int coordY3;
        int coordX4;
        int coordY4;
    };

    Q_OBJECT
public:
    explicit VideoFrameGenerator(QObject *parent = nullptr);

    QImage createImageForRectTarget(int targetCenterX, int targetCenterY,
                                    int targetWidth,   int targetHeight, uint8_t pixelBrightness);

    QImage createImageForTetragonTarget(int targetX1, int targetY1,
                                        int targetX2, int targetY2,
                                        int targetX3, int targetY3,
                                        int targetX4, int targetY4);

    void readTargetsFromFile(QString filename);
private:
    QPainter *painter;
    QImage   *generatedImage;
    QPolygon target;
    int frameWidth  {640};  //!< ширина результирующего кадра
    int frameHeight {480};  //!< высота результирующего кадра

    QVector<TargetShape> targets;

    void parseTargetLine(QString line);
public slots:

private slots:

signals:

};

#endif // VIDEOFRAMEGENERATOR_H
