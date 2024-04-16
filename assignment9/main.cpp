#include "mainwindow.h"

#include <QApplication>
#include "uilogicgate.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile styleSheetFile(":/Adaptic/Adaptic.qss");
    if (!styleSheetFile.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open stylesheet file";
        return 1;  // Exit or handle the error as appropriate
    }
    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(styleSheetFile.readAll()) };

    //setup stylesheet
    a.setStyleSheet(styleSheet);

    //TODO: when game is finished, delete below line. closing the welcome screen will make the game appear
    w.show();
    return a.exec();
}
