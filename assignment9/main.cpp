#include "mainwindow.h"

#include <QApplication>
#include <QApplication>
#include <QFile>

///
/// \brief qMain Main method, starts the app.
/// \param argc
/// \param argv
/// \return
///
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

    //set up stylesheet
    a.setStyleSheet(styleSheet);

    return a.exec();
}
