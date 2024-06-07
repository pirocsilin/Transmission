#include "imageLoader.h"
#include "IOManager.h"
#include "error.h"
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QPainter>


GLTexture ImageLoader::loadPNG(QString fileName)
{
    initializeOpenGLFunctions();

    // Преобразуем QImage в формат, совместимый с OpenGL
    QImage image;
    image.load(fileName);
    QImage formattedImage = image.convertToFormat(QImage::Format_RGBA8888);

    GLTexture texture = {};

    glGenTextures(1, &(texture.id));
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Устанавливаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загружаем данные изображения в текстуру
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedImage.width(), formattedImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedImage.bits());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    texture.width = formattedImage.width();
    texture.height = formattedImage.height();
    return texture;
}

GLTexture ImageLoader::createTargetTextureText(QString text)
{
    initializeOpenGLFunctions();
    int sizeX = 100;
    int sizeY = 100;
    GLTexture texture = {};
    QImage image(sizeX, sizeY, QImage::Format_RGBA8888);

    image.fill(Qt::transparent); // Прозрачный фон
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QFont font = painter.font();
    font.setPointSize(30); // Увеличиваем размер шрифта
    painter.setFont(font);
    painter.setPen(QPen(Qt::green));
    painter.drawText(QRect(0,0,sizeX,sizeY), Qt::AlignCenter, text);

    glBindTexture(GL_TEXTURE_2D, 0);
    glGenTextures(1, &(texture.id));
    glBindTexture(GL_TEXTURE_2D, texture.id);

       // Устанавливаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    texture.width = image.width();
    texture.height = image.height();
    return texture;
}

GLTexture ImageLoader::createTargetTexture()
{
    initializeOpenGLFunctions();
    int sizeX = 100;
    int sizeY = 100;
    GLTexture texture = {};
    QImage image(sizeX, sizeY, QImage::Format_RGBA8888);

    image.fill(Qt::transparent); // Прозрачный фон
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Установка размера и стиля шрифта

    QPen pen(Qt::red);
    pen.setWidth(5); // Установка толщины линии
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush); // Убираем заливку, оставляем только контур
    painter.drawRect(5, 5, 90, 90); // Рисуем квадрат

    glBindTexture(GL_TEXTURE_2D, 0);
    glGenTextures(1, &(texture.id));
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Устанавливаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    texture.width = image.width();
    texture.height = image.height();
    return texture;
}
