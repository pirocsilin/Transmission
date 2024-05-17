#include "mainwindow.h"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QWidget>
#include <QApplication>
#include "common/structscod.h"

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.show();

    return a.exec();
}

