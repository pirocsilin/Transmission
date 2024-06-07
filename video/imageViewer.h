#ifndef IMAGEVIEWER_H_POINT
#define IMAGEVIEWER_H_POINT

#include <cstdint>
#include <vector>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QTimer>
#include <QTime>
#include <QPointer>
#include <QThread>
#include <QVector>

#include "GLSLProgram.h"
#include "sprite.h"
#include "GLTexture.h"
#include "structseth.h"

class ImageViewer: public QThread, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ImageViewer();
    ~ImageViewer();
    void setData(std::vector<uint8_t> &data, int sizeX, int sizeY);
    void setData(const QImage& img);
    void setTargets(std::vector<TargetInfo> newTargets);
    void draw();
    void init(float x, float y, float width, float height, bool translate);
    void setViewPortDimensions(float x, float y, float width, float height);
private:
    void initShaders();
    float viewPortWidth{};
    float viewPortHeight{};
    float viewPortX{};
    float viewPortY{};
    float SCREEN_WIDTH = 640.0f;
    float SCREEN_HEIGHT = 512.0f;
    bool isInit = false;
    bool textureInit = false;
    bool needTranslate{false};
    std::vector<uint8_t> localData;
    QImage localImg;
    QOpenGLContext context;
    GLTexture texture;
    GLSLProgram colorProgram;
    Sprite mainSprite;
    Sprite cross;
    std::vector<TargetInfo> targets;
};

#endif // IMAGEVIEWER_H_POINT
