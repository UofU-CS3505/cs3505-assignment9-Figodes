#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>
#include <QMouseEvent>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Main issue: how should i make it so all uilogicgates are connected
    UILogicGate* logicGate = new UILogicGate(ui->canvas, "TEST");

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(logicGate, &UILogicGate::updatePickedUpGateLocation, this, &MainWindow::updatePickedUpGate);


    // UILogicGate* ex = new UILogicGate(ui->canvas, "DEF");
    // UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");
    // UILogicGate* NotGate = new UILogicGate(ui->canvas, "NOT", 1, 1);

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    bool startingInputs[3] = {false, false, false};
    showInputs(startingInputs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePickedUpGate(UILogicGate *gate, QPoint initialPosition) {
    pickedUpGate = gate;
    dragStartPosition = initialPosition;
    std::cout << "in slot" << std::endl;

}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (pickedUpGate) {
        QPoint newPos = event->pos() - dragStartPosition;
        // Move the widget to the new position within its parent's coordinate system
        pickedUpGate->move(newPos);
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event) {

}

void MainWindow::setLevelDescription(QString text){
    ui->levelDescription->setText(text);
}

void MainWindow::onStartClicked(){
    emit startSimulation();
}

void MainWindow::showInputs(bool inputs[]){
    if(inputs[0])
        ui->input1->setStyleSheet("background-color : lawngreen");
    else
        ui->input1->setStyleSheet("background-color : green");

    if(inputs[1])
        ui->input2->setStyleSheet("background-color : lawngreen");
    else
        ui->input2->setStyleSheet("background-color : green");

    if(inputs[2])
        ui->input3->setStyleSheet("background-color : lawngreen");
    else
        ui->input3->setStyleSheet("background-color : green");
}
