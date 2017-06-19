#include "mainwindow.h"
#include <QApplication>
#include"lightcalculator.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow m;
    m.setFixedHeight(600);
    m.setFixedWidth(1100);
    m.move(100,80);
    m.show();
    return a.exec();
}


