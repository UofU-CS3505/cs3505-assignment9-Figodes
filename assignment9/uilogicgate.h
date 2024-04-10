#ifndef UILOGICGATE_H
#define UILOGICGATE_H

#include <QLabel>
#include <QPushButton>
#include <QVector>

class UILogicGate : public QLabel
{
    Q_OBJECT

public:
    UILogicGate(QWidget* parent, QString operationName = "DEF", qint32 inputCount = 2, qint32 outputCount = 1);
private:
    QVector<QPushButton*> inputs;
    QVector<QPushButton*> outputs;
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
