#include "mainwindow.h"
#include <QApplication>

int image_enable = 0;
int param_enable = 0;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
