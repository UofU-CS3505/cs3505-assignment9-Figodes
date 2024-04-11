#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"
#include <QDrag>
#include <QMouseEvent>
#include <iostream>
#include "simulatorModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new simulatorModel();
    idCounter = 0;

    // Main issue: how should i make it so all uilogicgates are connected
    UILogicGate* logicGate = new UILogicGate(ui->canvas, idCounter++, "TEST");

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);


    // UILogicGate* ex = new UILogicGate(ui->canvas, "DEF");
    // UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");
    // UILogicGate* NotGate = new UILogicGate(ui->canvas, "NOT", 1, 1);
    model = new simulatorModel();


    connect(ui->addANDGate, &QPushButton::pressed, this, [this](){ addGate(0); });
    connect(ui->addORGate, &QPushButton::pressed, this, [this](){ addGate(1); });
    connect(ui->addNOTGate, &QPushButton::pressed, this, [this](){ addGate(2); });


    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    bool startingInputs[3] = {false, false, false};
    showInputs(startingInputs);

    gatePlaceholder = new QLabel(this); // Parent to MainWindow
    gatePlaceholder->setFixedSize(100, 75);
    gatePlaceholder->setStyleSheet("border: 2px dashed #000; background-color: rgba(255, 255, 255, 0);");
    gatePlaceholder->hide();

    this->setMouseTracking(true);
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

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    std::cout << "moved!" << std::endl;
    if (pickedUpGate)
    {
        std::cout << "moved picked gate" << std::endl;
        QPoint newPos = event->pos() - QPoint(pickedUpGate->width() / 2, pickedUpGate->height() / 2);
        pickedUpGate->move(ui->canvas->mapFromParent(newPos));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {

}


void MainWindow::addGate(qint32 gateType) {
    UILogicGate* newGate = nullptr;
    // Switch statement to instantiate the correct gate type
    switch(gateType) {
    case 0:
        newGate = new UILogicGate(ui->canvas, idCounter++, "AND", 2, 1);
        break;
    case 1:
        newGate = new UILogicGate(ui->canvas, idCounter++, "OR", 2, 1);
        break;
    case 2:
        newGate = new UILogicGate(ui->canvas, idCounter++, "NOT", 1, 1);
        break;
    }

    pickedUpGate = newGate;
    newGate->show();
}

void MainWindow::clearGates(){
    for(QObject* o: ui->canvas->children())
        delete o;
}
