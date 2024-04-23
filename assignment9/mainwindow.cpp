#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include "SimulatorModel.h"
#include <QPainter>
#include <QTimer>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , world(new b2World(b2Vec2(0.0f, 20.0f)))
    , timer(new QTimer())
{
    qApp->installEventFilter(this);
    setMouseTracking(true);
    pickedUpGate = nullptr;
    ui->setupUi(this);

    this->hide();
    ui->nextLevelButton->hide();
    ui->retryButton->hide();

    connectionBeingDrawn = false;
    idCounter = 0;
    model = new SimulatorModel();
    connect(this, &MainWindow::connectionDrawn, model, &SimulatorModel::connect);
    connect(this, &MainWindow::connectionBroken, model, &SimulatorModel::disconnect);
    connect(this, &MainWindow::newGateCreated, model, &SimulatorModel::addNewGate);
    connect(model, &SimulatorModel::displayNewLevel, this, &MainWindow::setupLevel);
    connect(model, &SimulatorModel::inputsSet, this, &MainWindow::showInputs);
    connect(model, &SimulatorModel::outputsSet, this, &MainWindow::showOutputs);
    connect(this, &MainWindow::startSimulation, model, &SimulatorModel::startSimulation);
    connect(ui->nextLevelButton, &QPushButton::clicked, model, &SimulatorModel::setupNextLevel);
    connect(model, &SimulatorModel::levelFinished, this, &MainWindow::simulationEnd);
    connect(model, &SimulatorModel::disableEditing, this, &MainWindow::disableAllButtons);
    connect(model, &SimulatorModel::enableEditing, this, &MainWindow::enableAllButtons);
    connect(model, &SimulatorModel::colorConnection, this, &MainWindow::colorWire);
    connect(model, &SimulatorModel::colorAllConnections, this, &MainWindow::colorAllWires);
    connect(model, &SimulatorModel::incorrectCircuit, this, &MainWindow::displayLevelFailed);
    connect(this, &MainWindow::removeGateFromModel, model, &SimulatorModel::removeGate);
    connect(model, &SimulatorModel::endGame, this, &MainWindow::gameCompleted);
    connect(model, &SimulatorModel::invalidCircuit, this, &MainWindow::displayInvalidCircuit);
    model->initializeView();
    connect(ui->nextLevelButton, &QPushButton::clicked, this, [this]{timer->stop();});

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(ui->retryButton, &QPushButton::clicked, this, &MainWindow::retryClicked);

    connect(&welcomescreen, &welcomeScreen::windowClosed, this, &MainWindow::showWindow);

    connect(ui->addANDGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::AND); });
    connect(ui->addORGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::OR); });
    connect(ui->addNOTGate, &QPushButton::pressed, this, [this](){ addGate(GateTypes::NOT); });

    connect(ui->resetButton, &QPushButton::clicked, model, &SimulatorModel::resetLevel);

    connect(timer, &QTimer::timeout, this, [this]() { updateFinishGates(); });

    ui->canvas->setStyleSheet("QLabel { border: 1px solid green; }");
    ui->resetButton->setStyleSheet("background-color : darkred");
    this->hide();
    ui->failLabel->hide();

    showWelcomeScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showWelcomeScreen() {

    // UNCOMMENT BELOW LINE TO
    welcomescreen.setWindowFlags(Qt::WindowStaysOnTopHint);
    welcomescreen.show();
}

void MainWindow::levelEndAnimation() {
    world = new b2World(b2Vec2(0.0f, 20.0f)); //reset world

    //  QTimer::singleShot(100, this, &MainWindow::updateVictoryGates);
    b2BodyDef* wallBodyDef = new b2BodyDef();
    wallBodyDef->type = b2_staticBody;
    wallBodyDef->position.Set(ui->canvas->width() / 2, ui->canvas->height()); //defines position of floor
    b2Body* wallsBody = world->CreateBody(wallBodyDef); //adds to world
    b2PolygonShape* wallBox = new b2PolygonShape();
    wallBox->SetAsBox(ui->canvas->width() / 2, 5.0f); //defines dimensions of ground, for some reason the argument here is half its dimension in each direction
    wallsBody->CreateFixture(wallBox, 0.0f); //applies dimensions to floor

    wallBodyDef->position.Set(ui->canvas->width() / 2, 0); //reuse and modify definer for position, now for ceiling
    wallsBody = world->CreateBody(wallBodyDef); //adds to world
    wallBox->SetAsBox(ui->canvas->width() / 2, 5.0f); //redefines dimensions for ceiling
    wallsBody->CreateFixture(wallBox, 0.0f); //applies dimensions to ceiling

    wallBodyDef->position.Set(0, ui->canvas->height() / 2); //same as before but for horizontal walls
    wallsBody = world->CreateBody(wallBodyDef);
    wallBox->SetAsBox(5.0f, ui->canvas->height() / 2);
    wallsBody->CreateFixture(wallBox, 0.0f);

    wallBodyDef->position.Set(ui->canvas->width(), ui->canvas->height() / 2); //same as before but for horizontal walls
    wallsBody = world->CreateBody(wallBodyDef);
    wallBox->SetAsBox(5.0f, ui->canvas->height() / 2);
    wallsBody->CreateFixture(wallBox, 0.0f);

    // Define the dynamic body. We set its position and call the body factory.
    //can set fixedRotation = true at bodyDef if we need them not to rotate
    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->type = b2_dynamicBody;
    bodyDef->fixedRotation = true;
    bodyDef->linearDamping = 0;
    for (UILogicGate* g : gates.values())
    {
        if (levelInOutGates.contains(g)) //ignore level-in/outs
            continue;
        bodyDef->position.Set(g->pos().x() + g->width()/2, g->pos().y() + g->height()/2);
        bodies.insert(g->id, world->CreateBody(bodyDef));
    }

    // Define another box shape for our dynamic body.
    UILogicGate* sampleGate = gates.values()[0];
    b2PolygonShape* dynamicBox = new b2PolygonShape();
    dynamicBox->SetAsBox(sampleGate->width() * 0.7, sampleGate->height()  * 0.7);

    // Define the dynamic body fixture.
    b2FixtureDef* fixtureDef = new b2FixtureDef();
    fixtureDef->shape = dynamicBox;
    fixtureDef->density = 100.0f;
    fixtureDef->friction = 0.0f;
    fixtureDef->restitution = 2.00f;

    // Add the shape to the body.
    QRandomGenerator rng;
    for (auto body : bodies)
    {
        body->CreateFixture(fixtureDef);
        body->SetLinearVelocity(b2Vec2(rng.bounded(-100, 100), rng.bounded(-100, 100)));
    }

    //start simulating
    timer->start(1000 / (60 * 3)); // *3 to increase speed
}

void MainWindow::updateFinishGates() {
    float32 timeStep = 1.0f / (60.0f);
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    world->Step(timeStep, velocityIterations, positionIterations);

    for (UILogicGate* gate: gates) {
        if (!levelInOutGates.contains(gate)) {
            b2Vec2 position = bodies[gate->id]->GetPosition();

            QPoint gatePos(position.x - gate->width()/2, position.y - gate->height()/2);
            gate->move(gatePos);
        }
    }

    update();
}

void MainWindow::stopTimer() {
    playFinishAnimation = false;

}
void MainWindow::showWindow() {
    this->show();
}


void MainWindow::updatePickedUpGate(UILogicGate *gate, QPoint initialPosition) {
    // if the user clicks on their selected gate, drop it.
    if (pickedUpGate == gate)
        pickedUpGate = nullptr;
    else
        pickedUpGate = gate;
}

void MainWindow::setupLevel(Level level){

    timer->stop();
    ui->nextLevelButton->hide();
    clearGates();

    ui->tableWidget->clear();

    // Set number of rows and columns in the table
    ui->tableWidget->setRowCount(qPow(2, level.inputCount));
    ui->tableWidget->setColumnCount(level.inputCount + level.outputCount);

    // Set headers
    QStringList headers;
    for (int i = 0; i < level.inputCount; ++i)
        headers << QString("IN %1").arg(i);
    for (int i = 0; i < level.outputCount; ++i)
        headers << QString("OUT %1").arg(i);
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < level.inputCount + level.outputCount; i++)
    {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // Populate the table with inputs and outputs
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        QVector<bool> inputs = level.getLevelInput(i);
        QVector<bool> outputs = level.getExpectedOutput(i);
        for (int j = 0; j < level.inputCount; ++j)
        {
            auto newItem = new QTableWidgetItem(QString::number(inputs[j]));
            newItem->setFlags(Qt::ItemIsEnabled); //prevents selection/editing of items
            ui->tableWidget->setItem(i, j, newItem);
        }
        for (int j = 0; j < level.outputCount; ++j)
        {
            auto newItem = new QTableWidgetItem(QString::number(outputs[j]));
            newItem->setFlags(Qt::ItemIsEnabled);
            ui->tableWidget->setItem(i, level.inputCount + j, newItem);
        }
    }

    ui->levelDescription->setText(level.getDescription());

    for (int i = 0; i < level.inputCount; i++) //create new inputs
    {
        addGate(GateTypes::LEVEL_IN);
    }
    for (int i = 0; i < level.outputCount; i++)//create new outputs
    {
        addGate(GateTypes::LEVEL_OUT);
    }

    update();
}

void MainWindow::onStartClicked(){
    emit startSimulation();
}

void MainWindow::showInputs(const QVector<bool>& inputs){
    //iterate through ui->inputs->children (input buttons)
    for (int i = 0; i < inputs.size(); i++)
    {
        QWidget* input = ui->inputs->itemAt(i)->widget();
        if(inputs.at(i) && input)
            input->setStyleSheet("background-color : lawngreen");
        else if(input)
            input->setStyleSheet("background-color : darkred");
    }
}

void MainWindow::showOutputs(const QVector<bool>& outputs){
    //iterate through ui->inputs->children (input buttons)
    for (int i = 0; i < outputs.size(); i++)
    {
        QWidget* output = ui->outputs->itemAt(i)->widget();
        if(outputs.at(i) && output)
            output->setStyleSheet("background-color : lawngreen");
        else if(output)
            output->setStyleSheet("background-color : darkred");
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    // move the gate if it's been picked up.
    if (pickedUpGate)
    {
        disableAllButtons();
        QPointF newPos = event->scenePosition() - QPoint(pickedUpGate->width() / 2, pickedUpGate->height() / 2);
        pickedUpGate->move(ui->canvas->mapFromParent(newPos).toPoint());
    }

    update();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    // Check if connection is being drawn
    if (connectionBeingDrawn) {
        // Enable all input and output buttons
        for (QPushButton* button : outputButtons) {
            button->setEnabled(true);
        }
        for (QPushButton* button : inputButtons) {
            button->setEnabled(true);
        }
        enableAllButtons();
        // Stop drawing the connection
        connectionBeingDrawn = false;
        update(); // Update the UI
    }
    // Call the base class implementation
    QMainWindow::mousePressEvent(event);
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
        newGate = new UILogicGate(this, idCounter++, QString("IN %1").arg(ui->inputs->count()), 0, 1);
        newGate->canBeMoved = false;
        break;
    case GateTypes::LEVEL_OUT:
        newGate = new UILogicGate(this, idCounter++, QString("OUT %1").arg(ui->outputs->count()), 1, 0);
        newGate->canBeMoved = false;
        break;
    }
    trackButtonsOn(newGate);
    gates.insert(newGate->id, newGate);

    newGate->show();
    emit newGateCreated(newGate->id, gateType);

    if (gateType == GateTypes::LEVEL_IN)
    {
        levelInOutGates.insert(newGate);
        ui->inputs->addWidget(newGate);
        return;
    }
    if (gateType == GateTypes::LEVEL_OUT)
    {
        levelInOutGates.insert(newGate);
        ui->outputs->addWidget(newGate);
        return;
    }

    pickedUpGate = newGate;
    // make the pickedUpGate initially be offscreen to avoid weird snapping effects
    pickedUpGate->move(1000,1000);

    newGate->setStyleSheet("background-color : lime");
    newGate->pickedUp = true;

}

void MainWindow::deleteGate(UILogicGate *gate) {
    // if the gate is picked up or is an input or output gate, skip the method
    if(gate->pickedUp || gate->canBeMoved == false) {
        return;
    }

    // if the gate isn't an input or output gate, remove from model
    if(!levelInOutGates.contains(gate)) {
        emit removeGateFromModel(gate->id);
    }

    // collect all the wires that must be removed from the gate
    QVector<int> wiresToRemove;
    for (int i = 0; i < uiButtonConnections.size(); i++) {

        for (QPushButton* button: gate->inputs) {
            if (uiButtonConnections[i].first == button || uiButtonConnections[i].second == button) {
                wiresToRemove.append(i);
                }
            }

        for (QPushButton* button: gate->outputs) {
            if (uiButtonConnections[i].first == button || uiButtonConnections[i].second == button) {
                wiresToRemove.append(i);
            }
        }
    }

    // int to see how many wires have been removed so far
    int numWiresRemoved = 0;

    // iterate through the wires and delte them
    for (int index : wiresToRemove) {
        uiButtonConnections.removeAt(index - numWiresRemoved);
       // numWiresRemoved++;
        numWiresRemoved++;
    }

    if(!levelInOutGates.contains(gate)) {
        // delete the gate and their input and output buttons
        gates.remove(gate->id);
        for (QPushButton* button : gate->inputs) {
            inputButtons.remove(button);
        }
        for (QPushButton* button : gate->outputs) {
            outputButtons.remove(button);
        }
        delete gate;
    }

    // redraw the screen so the deleted gates are gone
    update();
}

void MainWindow::clearGates(){
    for(QObject* o: ui->canvas->children())
        delete o;
    gates.clear();
    inputButtons.clear();
    outputButtons.clear();
    uiButtonConnections.clear();
    levelInOutGates.clear();
    QLayoutItem* prevItem;
    while (true) //clear input buttons
    {
        prevItem = ui->inputs->takeAt(0); //takes first each time until there are none left
        if (!prevItem)
            break;

        delete prevItem->widget();
        delete prevItem;
    }
    while (true) //clear output buttons
    {
        prevItem = ui->outputs->takeAt(0);
        if (!prevItem)
            break;

        delete prevItem->widget();
        delete prevItem;
    }
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
        disableAllButtons();

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
            qint32 giver;
            qint32 receiver;

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

            if (!isConnectionAlreadyExists(buttonBeingConnected, button))// Check if the connection already exists
            {
                emit connectionDrawn(giver, outputIndex, receiver, inputIndex);
                Wire newWire;
                if (outputButtons.contains(buttonBeingConnected))
                    newWire = {.first = buttonBeingConnected, .second = button, .color = Qt::black};
                else
                    newWire = {.first = button, .second = buttonBeingConnected, .color = Qt::black};
                uiButtonConnections.append(newWire);
            }
            else //conneciton already exists and should be deleted
            {
                QPushButton* givingButton = gates[giver]->outputs[outputIndex];
                QPushButton* receivingButton = gates[receiver]->inputs[inputIndex];
                Wire brokenWire = {.first = givingButton, .second = receivingButton, .color = Qt::black};
                emit connectionBroken(giver, outputIndex, receiver, inputIndex);
                //remove wire from ui connections
                for (qint32 i = 0; i < uiButtonConnections.size(); i++)
                    if (uiButtonConnections[i] == brokenWire)
                    {
                        uiButtonConnections.removeAt(i);
                        enableAllButtons();
                        break;
                    }
            }
        }

        for (QPushButton* button : outputButtons)
            button->setEnabled(true);
        for (QPushButton* button : inputButtons)
            button->setEnabled(true);

        // Reset state for next connection
        buttonBeingConnected = nullptr;
        connectionBeingDrawn = false;
        enableAllButtons();
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
    for (const Wire w : uiButtonConnections)
    {
        if ((w.first == button1 && w.second == button2) || (w.first == button2 && w.second == button1))
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

    // Set up a slight gradient for the charcoal gray background
    QLinearGradient gradient(QPointF(0, 0), QPointF(width(), height()));
    gradient.setColorAt(0, QColor(51, 51, 51)); // Dark charcoal gray
    gradient.setColorAt(1, QColor(41, 41, 41)); // Even darker shade of gray
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
        painter.setPen(QPen(Qt::black, 2)); //yes this is intentionally thinner

        QPoint startPos = buttonBeingConnected->mapToGlobal(QPoint(0,0));
        QPoint endPos = QCursor::pos();
        startPos = this->mapFromGlobal(startPos);
        endPos = this->mapFromGlobal(endPos);
        //adjust top draw from center of button
        startPos = startPos + QPoint(buttonBeingConnected->width() / 2, buttonBeingConnected->height() / 2);

        painter.drawLine(startPos, endPos);
    }

}

void MainWindow::disableAllButtons() {
    ui->startButton->setDisabled(1);
    ui->resetButton->setDisabled(1);

    ui->addANDGate->setDisabled(1);
    ui->addORGate->setDisabled(1);
    ui->addNOTGate->setDisabled(1);

    if(pickedUpGate == nullptr) {
        for(UILogicGate* g : gates) {
            g->canBeMoved = false;
        }
    }


    if(!connectionBeingDrawn) {
        for (QPushButton* button : outputButtons)
            button->setEnabled(false);
        for (QPushButton* button : inputButtons)
            button->setEnabled(false);
    }

}
void MainWindow::enableAllButtons() {
    ui->startButton->setEnabled(1);
    ui->resetButton->setEnabled(1);

    ui->addANDGate->setEnabled(1);
    ui->addORGate->setEnabled(1);
    ui->addNOTGate->setEnabled(1);

    for(UILogicGate* g : gates) {
        if (!levelInOutGates.contains(g)) //if not an level-in/out
            g->canBeMoved = true;
    }

    for (QPushButton* button : outputButtons)
        button->setEnabled(true);
    for (QPushButton* button : inputButtons)
        button->setEnabled(true);
}

void MainWindow::colorWire(qint32 giverId, qint32 outputIndex, qint32 receiverId, qint32 inputIndex, QColor newColor)
{
    UILogicGate* givingGate = nullptr;
    UILogicGate* receivingGate = nullptr;;
    for (UILogicGate* g : gates)
    {
        if (g->id == giverId)
            givingGate = g;
        if (g->id == receiverId)
            receivingGate = g;
    }
    if (givingGate && receivingGate)
    {
        QPushButton* firstButton = givingGate->outputs[outputIndex];
        QPushButton* secondButton = receivingGate->inputs[inputIndex];
        for (qint32 i = 0; i < uiButtonConnections.size(); i++)
        {
            if ((uiButtonConnections[i].first == firstButton && uiButtonConnections[i].second == secondButton)
                || (uiButtonConnections[i].first == secondButton && uiButtonConnections[i].second == firstButton))
            {
                uiButtonConnections[i].color = newColor;
                break;
            }
        }

        update();
    }
}

void MainWindow::colorAllWires(QColor color)
{
    for (qint32 i = 0; i < uiButtonConnections.size(); i++)
        uiButtonConnections[i].color = color;
    update();
}

void MainWindow::simulationEnd(bool success)
{
    if (success)
    {
       ui->nextLevelButton->show();
       levelEndAnimation(); //start celebration here
    }
    else
    {
        for(UILogicGate* gate: levelInOutGates)
            gate->setStyleSheet("background-color : green");
       // enableAllButtons();
        colorAllWires(Qt::black);
    }
}
void MainWindow::displayLevelFailed(QVector<bool> failedInput, QVector<bool> expectedOutput, QVector<bool> actualOutput) {
    disableAllButtons();

    QString message = "Level Failed!\n\n";
    message += "Input That Failed: " + boolVectorToString(failedInput) + "\n";
    message += "Expected Output: " + boolVectorToString(expectedOutput) + "\n";
    message += "Actual Output: " + boolVectorToString(actualOutput);

    ui->failLabel->setText(message);
    ui->failLabel->setWordWrap(true);  // Ensure the text wraps if too long
    ui->failLabel->setStyleSheet("QLabel { background-color: black; color: lime; }");
    ui->failLabel->show();  // Show the label with the failure information
    ui->retryButton->show();

}

void MainWindow::displayInvalidCircuit()
{
    disableAllButtons();

    QString message = "This circuit cannot be simulated!\n\n"
                      "This circuit is circular, and could simulate indefinitely";

    ui->failLabel->setText(message);
    ui->failLabel->setWordWrap(true);  // Ensure the text wraps if too long
    ui->failLabel->setStyleSheet("QLabel { background-color: black; color: lime; }");
    ui->failLabel->show();  // Show the label with the failure information
    ui->retryButton->show();
}

void MainWindow::gameCompleted() {
    // reuse the fail label to tell the player they won the game
    ui->failLabel->setText("You Win! Congratulations on passing CS 3810!");
    ui->failLabel->setStyleSheet("QLabel { background-color: black; color: lime; }");
    ui->failLabel->show();
}

QString MainWindow::boolVectorToString(const QVector<bool>& vec) {
    QString result;
    for (bool b : vec) {
        result += (b ? "1" : "0") + QString(" ");
    }
    return result.trimmed();
}

void MainWindow::retryClicked() {
    //uiButtonConnections.clear();
    repaint();
    enableAllButtons();
    ui->retryButton->hide();
    ui->failLabel->hide();
}

bool MainWindow::Wire::operator==(const Wire& other)
{
    return this->first == other.first && this->second == other.second;
}

