#include "widget.h"

#include <QApplication>
#include <QString>
#include <QDebug>
#include <QResource>

int main(int argc, char *argv[])
{
    QResource::registerResource(qApp->applicationDirPath()+"resource/image-qt_5_15-64.rcc");
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
