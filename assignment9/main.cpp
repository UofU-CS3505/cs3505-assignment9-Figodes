#include "mainwindow.h"

#include <QApplication>
#include "uilogicgate.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //TODO: when game is finished, delete below line. closing the welcome screen will make the game appear
    w.show();
    return a.exec();
}
