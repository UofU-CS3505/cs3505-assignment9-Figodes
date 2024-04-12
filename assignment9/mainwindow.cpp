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

    model = new SimulatorModel();
    idCounter = 0;


    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(this, &MainWindow::startSimulation, model, &SimulatorModel::startSimulation);

    model = new SimulatorModel();

    connect(ui->addANDGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::AND); });
    connect(ui->addORGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::OR); });
    connect(ui->addNOTGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::NOT); });

    connect(this, &MainWindow::newGateCreated, model, &SimulatorModel::addNewGate);

    connect(model, &SimulatorModel::gatesCleared, this, &MainWindow::clearGates);
    connect(model, &SimulatorModel::newLevel, this, &MainWindow::setLevelDescription);

    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    QVector<bool> startingInputs = {false, false, false};
    showInputs(startingInputs);

    showWelcomeScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showWelcomeScreen() {

    // uncomment below line to make sure welcome screen stays on top
    //welcomescreen.setWindowFlags(Qt::WindowStaysOnTopHint);
    welcomescreen.show();
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

void MainWindow::showInputs(QVector<bool> inputs){
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

    // Mouse move event for moving picked up gates
    if (pickedUpGate)
    {
        QPointF newPos = event->scenePosition() - QPoint(pickedUpGate->width() / 2, pickedUpGate->height() / 2);
        pickedUpGate->move(ui->canvas->mapFromParent(newPos).toPoint());
    }
    //if (buttonBeingConnected) then draw a line to the cursor

    // Mouse move event for drawing lines between inputs/outputs
    if(connectionBeingDrawn)
    {
        std::cout << "line being drawn at " << "mouse pos: (XD)" << std::endl;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {

}


void MainWindow::addGate(GateTypes gateType) {

    // if the user adds a new gate, disable the current one
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

    emit newGateCreated(newGate->id, gateType);

    pickedUpGate = newGate;
    // make the pickedUpGate initially be offscreen to avoid weird snapping effects
    pickedUpGate->move(1000,1000);

    newGate->setStyleSheet("background-color : lime");
    newGate->pickedUp = true;

    gates.append(newGate);
    newGate->show();
    std::cout << gates.size() << std::endl;
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

void MainWindow::connectionBeingMade(qint32 gate, QPushButton* button){

    if(gates[gate]->inputs.contains(button))
        std::cout << "you clicked an input button" << std::endl;
    // If connectionBeingDrawn is false, that means the button being sent is the source, set buttonBeingConnected
    if(!connectionBeingDrawn)
    {
        connectionBeingDrawn = !connectionBeingDrawn;
        buttonBeingConnected = button;
    }
    // If connectionBeingDrawn is true, that means the button send is the destination, connect to buttonBeingConnected and add connection to uiButtonConnections
    if(connectionBeingDrawn){

    }

}
void MainWindow::trackButtonsOn(UILogicGate* quarry)
{
    inputButtons.unite(*new QSet(quarry->inputs.begin(), quarry->inputs.end()));
    outputButtons.unite(*new QSet(quarry->outputs.begin(), quarry->outputs.end()));
}
