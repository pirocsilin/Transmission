#include "sprite.h"
#include "vertex.h"
#include "resourceManager.h"

#include <cstddef>

Sprite::Sprite(): vboID(0) {
}

Sprite::~Sprite() {
    if (vboID != 0)
        glDeleteBuffers(1, &vboID);
}

#include <QDebug>
void Sprite::init(float x, float y, float width, float height)
{
    if ( (spriteX == x) && (spriteY == y) && (spriteWidth == width) && (spriteHeight==height) )
        return;
    spriteWidth = width;
    spriteHeight = height;
    spriteX = x;
    spriteY = y;

    initializeOpenGLFunctions();
    if (vboID == 0)
    {
        glGenBuffers(1, &vboID);
    }

    Vertex vertexData[6];
    // first triangle
    vertexData[0].setPosition(x + width, y + height);
    vertexData[0].setUV(1.0f, 1.0f);

    vertexData[1].setPosition(x, y + height);
    vertexData[1].setUV(0.0f, 1.0f);

    vertexData[2].setPosition(x, y);
    vertexData[2].setUV(0.0f, 0.0f);

    // second triangle
    vertexData[3].setPosition(x, y);
    vertexData[3].setUV(0.0f, 0.0f);

    vertexData[4].setPosition(x + width, y);
    vertexData[4].setUV(1.0f, 0.0f);

    vertexData[5].setPosition(x + width, y + height);
    vertexData[5].setUV(1.0f, 1.0f);


    for (int i = 0; i < 6; i++) {
        vertexData[i].setColor(255, 255, 255, 255);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, color)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite::initTexture(std::string textureName)
{
    texture = ResourceManager::getTexture(textureName);
}

void Sprite::setPosition(float x, float y)
{
    init(x, y, spriteWidth, spriteHeight);
}

QVector3D Sprite::getCenterPoint()
{
    return QVector3D(spriteX, spriteY, 0.0) + QVector3D(spriteWidth/2.0, spriteHeight / 2.0f, 0.0f);
}

void Sprite::draw()
{
    if (visible)
    {

        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, color)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, uv)));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Sprite::texturedDraw()
{
    if (visible)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, color)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, uv)));

        glBindTexture(GL_TEXTURE_2D, texture.id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

