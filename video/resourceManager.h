#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "texturecache.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    static GLTexture getTexture(std::string textureName);

private:
    static TextureCache textureCache;
};

#endif // RESOURCEMANAGER_H
