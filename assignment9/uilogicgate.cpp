#include "uilogicgate.h"
#include <iostream>
#include <QPushButton>
#include <QMouseEvent>
#include <QVector>
#include <QtTypes>

UILogicGate::UILogicGate(QWidget* parent)
    : QLabel(parent)
{
    pickedUp = false;
    setFixedSize(100, 75);
    setStyleSheet("background-color : green");
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);

    QString operationName = "DEF";
    setText(operationName);
    setAlignment(Qt::AlignCenter);


    qint32 inputCount = 2;
    qint32 outputCount = 1;

    qint32 edgeBuffer = 5;
    qint32 buttonSize = 20;
    qint32 gateWidth = width();
    qint32 gateHeight = height();
    qint32 inputSpace = (gateHeight - 2 * edgeBuffer) / (inputCount + 1);
    qint32 outputSpace = (gateHeight - 2 * edgeBuffer) / (outputCount + 1);

    for (int c = 0; c < inputCount; c++)
    {
        inputs.append( new QPushButton("+", this));
        inputs[c]->setFixedSize(buttonSize, buttonSize);
        inputs[c]->move(x() + edgeBuffer, y() + edgeBuffer + ((c + 1) * inputSpace) - (buttonSize / 2));
    }

    for (int c = 0; c < outputCount; c++)
    {
        outputs.append( new QPushButton("+", this));
        outputs[c]->setFixedSize(buttonSize, buttonSize);
        outputs[c]->move(x() + (gateWidth - edgeBuffer - buttonSize), y() + edgeBuffer + ((c + 1) * outputSpace) - (buttonSize / 2));
    }
}

void UILogicGate::mousePressEvent(QMouseEvent* event)
{
    pickedUp = !pickedUp;
    if (pickedUp) {
        // Record the offset inside the widget at which it was clicked
        dragStartPosition = event->pos();
    }
    std::cout << "clicked!" << std::endl;
}

void UILogicGate::mouseMoveEvent(QMouseEvent* event)
{
    if (pickedUp) {
        // Calculate the new position for the widget by converting the current mouse position
        // within the widget to the parent's coordinate system, and then applying the initial offset
        QPoint newPos = event->pos() - dragStartPosition;
        // Move the widget to the new position within its parent's coordinate system
        this->move(mapToParent(newPos));
    }
    std::cout << "moved!" << std::endl;
}
