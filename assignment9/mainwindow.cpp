#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>
#include "simulatorModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new simulatorModel();

    UILogicGate* ex = new UILogicGate(ui->canvas);
    UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}
