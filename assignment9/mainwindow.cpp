#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    UILogicGate* ex = new UILogicGate(ui->canvas);
    UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLevelDescription(QString text){
    ui->levelDescription->setText(text);
}

void MainWindow::onStartClicked(){
    emit startSimulation();
}
