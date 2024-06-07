#ifndef IMAGELOADER_H
#define IMAGELOADER_H
#include <QOpenGLFunctions>
#include "GLTexture.h"

class ImageLoader: protected QOpenGLFunctions
{
public:
    ImageLoader() {}
    GLTexture loadPNG(QString fileName);
    GLTexture createTargetTexture();
    GLTexture createTargetTextureText(QString text);
};

#endif // IMAGELOADER_H
