#include "uilogicgate.h"
#include <iostream>
#include <QPushButton>

UILogicGate::UILogicGate(QWidget* parent)
    : QLabel(parent)
{
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
