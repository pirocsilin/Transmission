#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <map>
#include <QImage>
#include "GLTexture.h"
#include "imageLoader.h"

class TextureCache: protected QOpenGLFunctions
{
public:
    TextureCache();
    ~TextureCache();

    GLTexture getTexture(std::string imageName);

private:
    ImageLoader imageLoader;
    std::map<std::string, GLTexture> textureMap;
};

#endif // TEXTURECACHE_H
