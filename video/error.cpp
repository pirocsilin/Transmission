#include "error.h"
#include <QDebug>

extern void fatalError(const char* errorString)
{
    qDebug() << errorString;
    exit(100);
}
