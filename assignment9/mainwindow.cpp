#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UILogicGate* ex = new UILogicGate(ui->canvas, "DEF");
    UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");
    UILogicGate* NotGate = new UILogicGate(ui->canvas, "NOT", 1, 1);

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateGateLocation(UILogicGate gate) {


}
