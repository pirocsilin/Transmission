#include "texturecache.h"
#include <QDebug>

TextureCache::TextureCache()
{

}

TextureCache::~TextureCache()
{
    initializeOpenGLFunctions();
    for (auto &texture: textureMap)
    {
        glDeleteTextures(1, &texture.second.id);
    }
}

GLTexture TextureCache::getTexture(std::string imageName)
{
    auto mit = textureMap.find(imageName);
    ImageLoader imageLoader;

    if (mit == textureMap.end())
    {
        if (imageName == "cross")
        {
            GLTexture newTexture = imageLoader.loadPNG("cross.png");
            textureMap.insert(make_pair(imageName, newTexture));
            return newTexture; // returns new texture
        }
        else if (imageName == "target")
        {
            GLTexture newTexture = imageLoader.createTargetTexture();
            textureMap.insert(make_pair(imageName, newTexture));
            return newTexture; // returns new texture
        }
        else
        {
            GLTexture newTexture = imageLoader.createTargetTextureText(QString::fromStdString(imageName));
            textureMap.insert(make_pair(imageName, newTexture));
            return newTexture; // returns new texture
        }
    }
    return mit->second; // returns cached texture
}
