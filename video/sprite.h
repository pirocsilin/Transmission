#ifndef SPRITE_H
#define SPRITE_H
#include <QOpenGLFunctions>
#include <QVector3D>

#include "GLTexture.h"

class Sprite: protected QOpenGLFunctions {
public:
    Sprite();
    ~Sprite();

    void init(float x, float y, float width, float height);
    void initTexture(std::string textureName);
    void setPosition(float x, float y);
    QVector3D getCenterPoint();
    void draw();
    void texturedDraw();
    bool visible{true};
    float spriteWidth;
    float spriteHeight;
    float spriteX;
    float spriteY;
private:
    bool textureInit;
    GLuint vboID;
    GLuint vaoID;
    GLTexture texture;
};

#endif // SPRITE_H
