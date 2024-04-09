#ifndef UILOGICGATE_H
#define UILOGICGATE_H

#include <QLabel>
#include <QPushButton>

class UILogicGate : public QLabel
{
public:
    UILogicGate(QWidget *parent);
private:
    QPushButton* input1;
    QPushButton* input2;
    QPushButton* output;

};

#endif // UILOGICGATE_H
