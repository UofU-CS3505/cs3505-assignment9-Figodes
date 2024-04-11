#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <iostream>
#include "simulatorModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qApp->installEventFilter(this);
    setMouseTracking(true);
    pickedUpGate = nullptr;
    ui->setupUi(this);

    model = new simulatorModel();
    idCounter = 0;

    // Main issue: how should i make it so all uilogicgates are connected
    UILogicGate* logicGate = new UILogicGate(ui->canvas, idCounter++, "TEST");

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    model = new simulatorModel();

    connect(ui->addANDGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::AND); });
    connect(ui->addORGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::OR); });
    connect(ui->addNOTGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::NOT); });


    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    bool startingInputs[3] = {false, false, false};
    showInputs(startingInputs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePickedUpGate(UILogicGate *gate, QPoint initialPosition) {
    if (pickedUpGate == gate)
        pickedUpGate = nullptr;
    else
        pickedUpGate = gate;
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
    if (pickedUpGate)
    {
        std::cout << "moved picked gate" << std::endl;
        QPointF newPos = event->scenePosition() - QPoint(pickedUpGate->width() / 2, pickedUpGate->height() / 2);
        pickedUpGate->move(ui->canvas->mapFromParent(newPos).toPoint());
        std::cout << newPos.x() << ", " << newPos.y() << std::endl;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {

}


void MainWindow::addGate(GateTypes gateType) {

    if (pickedUpGate){
        pickedUpGate->setStyleSheet("background-color : green");
        pickedUpGate->pickedUp = false;
        pickedUpGate = nullptr;
    }
    UILogicGate* newGate = nullptr;
    // Switch statement to instantiate the correct gate type
    switch(gateType) {
    case GateTypes::AND:
        newGate = new UILogicGate(ui->canvas, idCounter++, "AND", 2, 1);
        break;
    case GateTypes::OR:
        newGate = new UILogicGate(ui->canvas, idCounter++, "OR", 2, 1);
        break;
    case GateTypes::NOT:
        newGate = new UILogicGate(ui->canvas, idCounter++, "NOT", 1, 1);
        break;
    }
    trackButtonsOn(newGate);


    pickedUpGate = newGate;

    newGate->setStyleSheet("background-color : lime");
    newGate->pickedUp = true;

    gates.append(newGate);
    newGate->show();
}

void MainWindow::clearGates(){
    for(QObject* o: ui->canvas->children())
        delete o;
    inputButtons.clear();
    outputButtons.clear();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    //std::cout << obj->objectName().toStdString() << std::endl;
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
    }
    return false;
}

void MainWindow::connectionBeingMade(qint32 gate, QString type, qint32 index){
    std::cout << gate << ", " << type.toStdString() << ", " << index << std::endl;
}
void MainWindow::trackButtonsOn(UILogicGate* quarry)
{
    inputButtons.unite(*new QSet(quarry->inputs.begin(), quarry->inputs.end()));
    outputButtons.unite(*new QSet(quarry->outputs.begin(), quarry->outputs.end()));
}
