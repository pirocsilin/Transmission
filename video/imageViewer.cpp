#include "imageViewer.h"

#include "error.h"
#include "imageLoader.h"

#include <QtGlobal>
#include <QDateTime>
#include <QPainter>
#include <QMatrix4x4>
#include <QPainterPath>

ImageViewer::ImageViewer()
{
    //context.create();
    //initializeOpenGLFunctions();
}

ImageViewer::~ImageViewer()
{
    if (textureInit)
    {
        glDeleteTextures(1, &texture.id);
    }
}

void ImageViewer::setViewPortDimensions(float x, float y, float width, float height)
{
    viewPortX = x - width / 2.0f;
    viewPortY = y - height / 2.0f;
    viewPortWidth = width;
    viewPortHeight = height;

    mainSprite.init((viewPortX/SCREEN_WIDTH)*2.0f, (viewPortY/SCREEN_HEIGHT)*2.0f, (viewPortWidth/SCREEN_WIDTH)*2.0f, (viewPortHeight/SCREEN_HEIGHT)*2.0f);
}

void ImageViewer::init(float x, float y, float width, float height, bool translate)
{
    initializeOpenGLFunctions();
    initShaders();

    needTranslate = translate;

    setViewPortDimensions(x, y,  width,  height);

    isInit = true;

    cross.init(-1.0f, -1.0f, 2.0f, 2.0f);
    cross.initTexture("cross");
    cross.visible = true;

}

void ImageViewer::setTargets(std::vector<TargetInfo> newTargets)
{
    targets = newTargets;
}

void ImageViewer::setData(const QImage& img)
{
    if (!textureInit)
    {
        glGenTextures(1, &(texture.id));
        textureInit = true;
    }
    glBindTexture(GL_TEXTURE_2D, texture.id);

    if (!targets.empty())
    {
        QImage coloredImg = img.convertToFormat(QImage::Format_RGB888); // Используем RGB888
        QPainter painter(&coloredImg);
        QPainterPath path;
        static QPen greenPen(Qt::green, 2);
        static QPen redPen(Qt::red, 2);
        painter.setBrush(Qt::NoBrush);
        for (auto target:targets)
        {
            if (!target.isTracked)
            {
                painter.setPen(greenPen);
                painter.drawRect(target.coordXTargetCenter - 25, target.coordYTargetCenter - 25, 50, 50);
            }
            else
            {
                painter.setPen(redPen);
                path.moveTo(target.coordXTargetCenter, target.coordYTargetCenter - 25);
                path.lineTo(target.coordXTargetCenter - 25, target.coordYTargetCenter + 25);
                path.lineTo(target.coordXTargetCenter + 25, target.coordYTargetCenter + 25);
                path.lineTo(target.coordXTargetCenter, target.coordYTargetCenter - 25);
                painter.drawPath(path);

            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, coloredImg.width(), coloredImg.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, coloredImg.bits());
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, img.width(), img.height(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img.bits());
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    texture.width = img.width();
    texture.height = img.height();
    glBindTexture(GL_TEXTURE_2D, 0);
}



void ImageViewer::draw()
{
    if (isInit)
    {
        static float angle = 0;
        if (needTranslate)
            angle = 90;
        QMatrix4x4 rotationMatrix;
        float centerX = mainSprite.spriteX + mainSprite.spriteWidth / 2.0f;
        float centerY = mainSprite.spriteY + mainSprite.spriteHeight / 2.0f;
        rotationMatrix.translate(centerX, centerY, 0.0);
        rotationMatrix.rotate(angle, 0.0, 0.0, 1.0); // Rotate around Z-axis
        rotationMatrix.translate(-centerX, -centerY, 0.0);

        colorProgram.use();
        GLint rotLoc = colorProgram.getUniformLocation("rotationMatrix");
        glUniformMatrix4fv(rotLoc, 1, GL_FALSE, rotationMatrix.constData());

        glActiveTexture(GL_TEXTURE0);

        GLint textureLocation = colorProgram.getUniformLocation("mySampler");
        glUniform1i(textureLocation, 0);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        mainSprite.draw();
        glBindTexture(GL_TEXTURE_2D, 0);

        colorProgram.unuse();
    }
}

void ImageViewer::initShaders()
{
    colorProgram.compileShaders();
    colorProgram.addAttribute("vertexPosition");
    colorProgram.addAttribute("colorPosition");
    colorProgram.addAttribute("vertexUV");
    colorProgram.linkShaders();
}
