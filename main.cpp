#include "mainwindow.h"

#include <QApplication>
#include <QObject>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QFile  qssFile(":/qss/mystyle.qss");
    if(qssFile.open(QIODevice::ReadOnly))
    {
        a.setStyleSheet(qssFile.readAll());
    }
    return a.exec();
}
