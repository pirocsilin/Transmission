#include "resourceManager.h"

TextureCache ResourceManager::textureCache;

GLTexture ResourceManager::getTexture(std::string textureName) {
    return textureCache.getTexture(textureName);
}
