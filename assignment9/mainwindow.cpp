#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <iostream>
#include "SimulatorModel.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qApp->installEventFilter(this);
    setMouseTracking(true);
    pickedUpGate = nullptr;
    ui->setupUi(this);

    this->hide();

    connectionBeingDrawn = false;
    idCounter = 0;
    model = new SimulatorModel();
    connect(this, &MainWindow::connectionDrawn, model, &SimulatorModel::connect);
    connect(this, &MainWindow::newGateCreated, model, &SimulatorModel::addNewGate);
    //connect(model, &SimulatorModel::gatesCleared, this, &MainWindow::clearGates); //should just clear on setupNewLevel
    connect(model, &SimulatorModel::displayNewLevel, this, &MainWindow::setupLevel);
    connect(model, &SimulatorModel::inputsSet, this, &MainWindow::showInputs);
    connect(model, &SimulatorModel::outputsSet, this, &MainWindow::showOutputs);
    connect(this, &MainWindow::startSimulation, model, &SimulatorModel::startSimulation);
    connect(model, &SimulatorModel::levelComplete, model, &SimulatorModel::loadNextLevel); //temporary, start next level when current ends
    model->initializeView();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    connect(&welcomescreen, &welcomeScreen::windowClosed, this, &MainWindow::showWindow);

    connect(ui->addANDGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::AND); });
    connect(ui->addORGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::OR); });
    connect(ui->addNOTGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::NOT); });


    ui->canvas->setStyleSheet("QLabel { border: 1px solid black; }");
    this->hide();

    showWelcomeScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showWelcomeScreen() {

    // UNCOMMENT BELOW LINE TO
    //welcomescreen.setWindowFlags(Qt::WindowStaysOnTopHint);
    welcomescreen.show();
}

void MainWindow::showWindow() {
    this->show();
}


void MainWindow::updatePickedUpGate(UILogicGate *gate, QPoint initialPosition) {
    if (pickedUpGate == gate)
        pickedUpGate = nullptr;
    else
        pickedUpGate = gate;
}

void MainWindow::setupLevel(Level level){
    clearGates();

    ui->tableWidget->setRowCount(level.inputCount);
    ui->tableWidget->setColumnCount(level.outputCount + level.outputCount);

    // Set headers
    QStringList headers;
    for (int i = 0; i < level.outputCount; ++i) {
        headers << QString("Input %1").arg(i);
    }
    for (int i = 0; i < level.outputCount; ++i) {
        headers << QString("Output %1").arg(i);
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);


    for (int i = 0; i < level.inputCount; ++i) {
        QVector<bool> inputSet = level.getLevelInput(i);
        for (int j = 0; j < level.inputCount; ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(inputSet[j] ? "1" : "0");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, j, item);
        }
        QVector<bool> outputSet = level.getExpectedOutput(i);
        for (int j = 0; j < level.outputCount; ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(outputSet[j] ? "1" : "0");
            ui->tableWidget->setItem(i, level.outputCount + j, item);
        }
    }

    // Resize columns to fit content
    ui->tableWidget->resizeColumnsToContents();

    ui->levelDescription->setText(level.getDescription());

    QVector<QLayoutItem*> previousInputs;
    for (int i = 0; i < ui->inputs->count(); i++) //clear input buttons
        previousInputs.append(ui->inputs->itemAt(i));
    for (QLayoutItem* item : previousInputs)
    {
        ui->inputs->removeItem(item);
        delete item->widget();
        delete item;
    }
    for (int i = 0; i < level.inputCount; i++) //create new inputs
    {
        addGate(GateTypes::LEVEL_IN);
    }

    QVector<QLayoutItem*> previousOutputs;
    for (int i = 0; i < ui->outputs->count(); i++) //clear output buttons
        previousOutputs.append(ui->outputs->itemAt(i));
    for (QLayoutItem* item : previousOutputs)
    {
        ui->outputs->removeItem(item);
        delete item->widget();
        delete item;
    }
    for (int i = 0; i < level.outputCount; i++)//create new outputs
    {
        addGate(GateTypes::LEVEL_OUT);
    }

    //display required inputs->outputs for level
}

void MainWindow::onStartClicked(){
    std::cout<<"start clicked"<<std::endl;
    emit startSimulation();
}

void MainWindow::showInputs(QVector<bool> inputs){
    //iterate through ui->inputs->children (input buttons)
    for (int i = 0; i < inputs.size(); i++)
    {
        QWidget* input = ui->inputs->itemAt(i)->widget();
        if(inputs[i] && input)
            input->setStyleSheet("background-color : lawngreen");
        else
            input->setStyleSheet("background-color : green");
    }
}

void MainWindow::showOutputs(QVector<bool> outputs){
    //iterate through ui->inputs->children (input buttons)
    for (int i = 0; i < outputs.size(); i++)
    {
        QWidget* output = ui->outputs->itemAt(i)->widget();
        if(outputs[i] && output)
            output->setStyleSheet("background-color : lawngreen");
        else
            output->setStyleSheet("background-color : green");
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    // Mouse move event for moving picked up gates
    if (pickedUpGate)
    {
        QPointF newPos = event->scenePosition() - QPoint(pickedUpGate->width() / 2, pickedUpGate->height() / 2);
        pickedUpGate->move(ui->canvas->mapFromParent(newPos).toPoint());
    }

    update();
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
    case GateTypes::LEVEL_IN:
        newGate = new UILogicGate(this, idCounter++, "IN", 0, 1);
        break;
    case GateTypes::LEVEL_OUT:
        newGate = new UILogicGate(this, idCounter++, "OUT", 1, 0);
        break;
    }
    trackButtonsOn(newGate);
    gates.append(newGate);
    newGate->show();
    emit newGateCreated(newGate->id, gateType);

    if (gateType == GateTypes::LEVEL_IN)
    {
        ui->inputs->addWidget(newGate);
        return;
    }
    if (gateType == GateTypes::LEVEL_OUT)
    {
        ui->outputs->addWidget(newGate);
        return;
    }

    pickedUpGate = newGate;
    // make the pickedUpGate initially be offscreen to avoid weird snapping effects
    pickedUpGate->move(1000,1000);

    newGate->setStyleSheet("background-color : lime");
    newGate->pickedUp = true;

}

void MainWindow::clearGates(){
    std::cout<<"clearing gates"<<std::endl;
    for(QObject* o: ui->canvas->children())
        delete o;
    inputButtons.clear();
    outputButtons.clear();
    uiButtonConnections.clear();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
    }
    return false;
}

void MainWindow::connectionBeingMade(qint32 gate, QPushButton* button)
{
    if (!connectionBeingDrawn)
    {
        // First button clicked, start drawing connection
        buttonBeingConnected = button;
        connectionBeingDrawn = true;
        connectingGate = gate;

        if (outputButtons.contains(buttonBeingConnected))
        {
            for (QPushButton* b : outputButtons)
                b->setEnabled(false);
            for (QPushButton* b : gates[gate]->inputs)
                b->setEnabled(false);
        }
        else
        {
            for (QPushButton* b : inputButtons)
                b->setEnabled(false);
            for (QPushButton* b : gates[gate]->outputs)
                b->setEnabled(false);
        }
    }
    else
    {
        // Second button clicked, end connection        
        if (isConnectionValid(buttonBeingConnected, button, connectingGate, gate)) // Check if the connection is valid
        {
            if (!isConnectionAlreadyExists(buttonBeingConnected, button)) // Check if the connection already exists
            {
                qint32 giver;
                qint32 receiver;

                Wire newWire = {.first = buttonBeingConnected, .second = button, .color = Qt::black};
                uiButtonConnections.append(newWire);

                qint32 outputIndex = -1; // Initialize to -1 to indicate no match found
                qint32 inputIndex = -1; // Initialize to -1 to indicate no match found
                if (gates[connectingGate]->outputs.contains(buttonBeingConnected))
                {
                    outputIndex = gates[connectingGate]->outputs.indexOf(buttonBeingConnected);
                    inputIndex = gates[gate]->inputs.indexOf(button);
                    giver = connectingGate;
                    receiver = gate;
                }
                else
                {
                    outputIndex = gates[gate]->outputs.indexOf(button);
                    inputIndex = gates[connectingGate]->inputs.indexOf(buttonBeingConnected);
                    giver = gate;
                    receiver = connectingGate;
                }

                emit connectionDrawn(giver, outputIndex, receiver, inputIndex);
            }
        }

        for (QPushButton* button : outputButtons)
            button->setEnabled(true);
        for (QPushButton* button : inputButtons)
            button->setEnabled(true);

        // Reset state for next connection
        buttonBeingConnected = nullptr;
        connectionBeingDrawn = false;
        update();

    }
}

bool MainWindow::isConnectionValid(QPushButton* button1, QPushButton* button2, qint32 gate1, qint32 gate2)
{
    // Check if both buttons are not inputs or both are not outputs
    if ((gates[gate1]->inputs.contains(button1) && gates[gate2]->inputs.contains(button2)) ||
        (gates[gate1]->outputs.contains(button1) && gates[gate2]->outputs.contains(button2)))
    {
        return false; // Invalid connection
    }
    return true; // Valid connection
}

bool MainWindow::isConnectionAlreadyExists(QPushButton* button1, QPushButton* button2)
{
    for (const auto& pair : uiButtonConnections)
    {
        if ((pair.first == button1 && pair.second == button2) || (pair.first == button2 && pair.second == button1))
        {
            return true;
        }
    }
    return false;
}


void MainWindow::trackButtonsOn(UILogicGate* quarry)
{
    inputButtons.unite(*new QSet(quarry->inputs.begin(), quarry->inputs.end()));
    outputButtons.unite(*new QSet(quarry->outputs.begin(), quarry->outputs.end()));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Draw the background gradient first
    QLinearGradient gradient(QPointF(0, 0), QPointF(width(), height()));
    gradient.setColorAt(0, QColor(102, 115, 140));
    gradient.setColorAt(1, QColor(56, 63, 77));
    painter.fillRect(rect(), gradient);

    // Iterate over all the connections
    for (const auto& connection : uiButtonConnections)
    {
        // Set pen color and width for the lines
        painter.setPen(QPen(connection.color, 4));
        // Get the positions of the connected buttons relative to the canvas
        QPoint startPos = connection.first->mapToGlobal(QPoint(0,0));
        QPoint endPos = connection.second->mapToGlobal(QPoint(0,0));
        startPos = this->mapFromGlobal(startPos);
        endPos = this->mapFromGlobal(endPos);
        //adjust positions to draw toward the middle of the buttons
        startPos = startPos + QPoint(connection.first->width() / 2, connection.first->height() / 2);
        endPos = endPos + QPoint(connection.second->width() / 2, connection.second->height() / 2);

        // Draw a line between the buttons
        painter.drawLine(startPos, endPos);
    }

    if (connectionBeingDrawn)
    {
        painter.setPen(QPen(Qt::black, 2));

        QPoint startPos = buttonBeingConnected->mapToGlobal(QPoint(0,0));
        QPoint endPos = QCursor::pos();
        startPos = this->mapFromGlobal(startPos);
        endPos = this->mapFromGlobal(endPos);
        //adjust top draw from center of button
        startPos = startPos + QPoint(buttonBeingConnected->width() / 2, buttonBeingConnected->height() / 2);

        painter.drawLine(startPos, endPos);
    }

}








