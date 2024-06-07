#include <QImage>
#include <QDebug>

#include <qopenglfunctions.h>
#include "myopenglwidget.h"

MyOpenGLWidget::MyOpenGLWidget()  : frame(0)
{
    fpstimer = new QTimer();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    connect(fpstimer, SIGNAL(timeout()),  this, SLOT(onFpsTimer()));
    fpstimer->setInterval(1000);
    fpstimer->start();

}

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent): QOpenGLWidget(parent), frame(0)
{
    fpstimer = new QTimer();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    connect(fpstimer, SIGNAL(timeout()),  this, SLOT(onFpsTimer()));
    fpstimer->setInterval(1000);
    fpstimer->start();
}

void MyOpenGLWidget::setImage(QImage image)
{

    if (!contextCreated)
        return;
    thermalFrameViewer.setViewPortDimensions(0.0f, 0.0f,
                                             static_cast<float>(image.width()),
                                             static_cast<float>(image.height()));
    thermalFrameViewer.setData(image);
}

void MyOpenGLWidget::setTargets(std::vector<TargetInfo> newTargets)
{
    thermalFrameViewer.setTargets(newTargets);
//    if(!newTargets.empty())
//    qDebug() << __PRETTY_FUNCTION__ << newTargets.size() << newTargets.front().coordXTargetCenter
//             << newTargets.front().coordYTargetCenter << newTargets.front().isTracked;
}

void MyOpenGLWidget::onFpsTimer()
{
    qDebug() << "FPS = " << frame;
    frame = 0;
}

void MyOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    thermalFrameViewer.init(0.0f, 0.0f, 640.0f, 512.0f, false);

    if (/* DISABLES CODE */ (0))
    {
        TargetInfo testTarget1;
        testTarget1.coordXTargetCenter = 320;
        testTarget1.coordYTargetCenter = 256;

        TargetInfo testTarget2;
        testTarget2.coordXTargetCenter = 120;
        testTarget2.coordYTargetCenter = 400;

        std::vector<TargetInfo> testTargets;
        testTargets.push_back(testTarget1);
        testTargets.push_back(testTarget2);
        setTargets(testTargets);

        QImage* image = new QImage;
        image->load("testimage.bmp");
        *image = image->convertToFormat(QImage::Format_Grayscale8);
        setImage(*image);
    }
    contextCreated = true;
}


void MyOpenGLWidget::paintGL()
{
    ++frame;
    thermalFrameViewer.draw();
}
