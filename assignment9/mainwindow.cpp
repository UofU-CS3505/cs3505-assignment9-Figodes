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

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    
    UILogicGate* ex = new UILogicGate(ui->canvas, "DEF");
    UILogicGate* ex2 = new UILogicGate(ui->canvas, "DEF2");
    UILogicGate* NotGate = new UILogicGate(ui->canvas, "NOT", 1, 1);

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    bool startingInputs[3] = {false, false, false};
    showInputs(startingInputs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::updateGateLocation(UILogicGate *gate, QMouseEvent *event) {


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
