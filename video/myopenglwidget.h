#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTime>

#include "imageViewer.h"
#include "structseth.h"

class MyOpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    MyOpenGLWidget() ;

    MyOpenGLWidget(QWidget *parent);
    void setImage(QImage image);
    void setTargets(std::vector<TargetInfo>);

public slots:
    void onFpsTimer();

protected:
    void initializeGL() override;

    void paintGL() override;

private:
    bool contextCreated = false;
    QTimer *fpstimer;
    uint32_t frame;
    ImageViewer thermalFrameViewer;
};

#endif // MYOPENGLWIDGET_H
