#ifndef UILOGICGATE_H
#define UILOGICGATE_H

#include <QLabel>
#include <QPushButton>

class UILogicGate : public QLabel
{
    Q_OBJECT

public:
    UILogicGate(QWidget *parent);
private:
    QPushButton* input1;
    QPushButton* input2;
    QPushButton* output;
    QPoint dragStartPosition;
    bool pickedUp;

public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

};

#endif // UILOGICGATE_H
