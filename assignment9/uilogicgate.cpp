#include "uilogicgate.h"
#include <iostream>
#include <QPushButton>
#include <QMouseEvent>

UILogicGate::UILogicGate(QWidget* parent)
    : QLabel(parent)
{
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::ClickFocus);

    pickedUp = false;
    this->setFixedSize(300, 200);
    this->setStyleSheet("background-color : green");

    input1 = new QPushButton("+", this);
    input1->setFixedSize(30, 30);
    input1->move(this->x() + 50, this->y() + 50);
    input2 = new QPushButton("+", this);
    input2->setFixedSize(30, 30);
    input2->move(this->x() + 50, this->y() + 100);
    output = new QPushButton("+", this);
    output->setFixedSize(30, 30);
    output->move(this->x() + 200, this->y() + 75);
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
