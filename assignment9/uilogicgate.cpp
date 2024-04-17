#include "uilogicgate.h"
#include "mainwindow.h"
#include <iostream>
#include <QPushButton>
#include <QMouseEvent>
#include <QVector>
#include <QtTypes>
#include <QPainter>

UILogicGate::UILogicGate(QWidget* parent, qint32 id, QString operationName, qint32 inputCount, qint32 outputCount)
    : QLabel(parent)
    , id(id)
{
    MainWindow* mainWindow = nullptr;
    QObject* currentParent = this;
    while (!mainWindow) //if cast succeeded and we found mainwindow, this should exit since the ptr should no longer be null
    {
        currentParent = currentParent->parent();
        mainWindow = qobject_cast<MainWindow*>(currentParent);
    }
    connect(this, &UILogicGate::updatePickedUpGateLocation, mainWindow, &MainWindow::updatePickedUpGate);
    connect(this, &UILogicGate::inputOrOutputPressed, mainWindow, &MainWindow::connectionBeingMade);
    connect(this, &UILogicGate::deleteGate, mainWindow, &MainWindow::deleteGate);


    pickedUp = false;
    setFixedSize(100, 75);
    setStyleSheet("background-color : green");
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);

    setText(operationName);
    setAlignment(Qt::AlignCenter);
    setMargin(5);

    if (inputCount == 0)
    {
        setFixedSize(width() - 30, height() - 30);
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    if (outputCount == 0)
    {
        setFixedSize(width() - 30, height() - 30);
        setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    qint32 edgeBuffer = 5;
    qint32 buttonSize = 20;
    qint32 gateWidth = width();
    qint32 gateHeight = height();
    qint32 inputSpace = (gateHeight) / (inputCount + 1);
    qint32 outputSpace = (gateHeight - 2 * edgeBuffer) / (outputCount + 1);

    for (int c = 0; c < inputCount; c++)
    {
        inputs.append( new QPushButton("+", this));
        inputs[c]->setFixedSize(buttonSize, buttonSize);
        inputs[c]->move(x() + edgeBuffer, y() + ((c + 1) * inputSpace) - (buttonSize / 2));
        // Connect button press signals for inputs
        connect(inputs[c], &QPushButton::pressed, this, &UILogicGate::startLineDrawing);
    }

    for (int c = 0; c < outputCount; c++)
    {
        outputs.append( new QPushButton("+", this));
        outputs[c]->setFixedSize(buttonSize, buttonSize);
        outputs[c]->move(x() + (gateWidth - edgeBuffer - buttonSize), y() + edgeBuffer + ((c + 1) * outputSpace) - (buttonSize / 2));
        // Connect button press signals for outputs
        connect(outputs[c], &QPushButton::pressed, this, &UILogicGate::startLineDrawing);
    }
}

void UILogicGate::mousePressEvent(QMouseEvent* event)
{

    if(canBeMoved){
        if (event->button() == Qt::RightButton) {
            emit deleteGate(this);
        }
        else{
            pickedUp = !pickedUp;
            if (pickedUp) {
                // Record the offset inside the widget at which it was clicked
                // dragStartPosition = event->pos();
                setStyleSheet("background-color : lime");
                QWidget::raise();
            }
            else {
                setStyleSheet("background-color : green");
            }
            emit updatePickedUpGateLocation(this, event->pos());
        }

    }
}



void UILogicGate::updateLocation(QPoint newLocation) {
    std::cout << newLocation.x() << newLocation.y() << std::endl;
    this->move(mapToParent(newLocation));
}

void UILogicGate::startLineDrawing()
{

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    // After, go to the gate, do indexof sender pushbutton on the inputs/outputs
    emit inputOrOutputPressed(this->id, button);
}

UILogicGate::~UILogicGate(){}

