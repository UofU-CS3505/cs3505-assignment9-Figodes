#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "uilogicgate.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void updateGateLocation(UILogicGate *gate, QMouseEvent *event);

signals:

    // Signal to tell the model that a connection has been made between logic gates
    void connectionDrawn();
};
#endif // MAINWINDOW_H
