#ifndef UILOGICGATE_H
#define UILOGICGATE_H

#include <QLabel>
#include <QPushButton>
#include <QVector>

class UILogicGate : public QLabel
{
    Q_OBJECT

public:
    UILogicGate(QWidget* parent, qint32 id, QString operationName = "DEF", qint32 inputCount = 2, qint32 outputCount = 1);
    ~UILogicGate();
    qint32 id;
    bool pickedUp;
    QVector<QPushButton*> inputs;
    QVector<QPushButton*> outputs;
    bool canBeMoved = true;
private:
    QPoint dragStartPosition;
    bool drawingLine;
    QPoint lineStart;
    QPoint lineEnd;
    QPushButton* activeButton;

signals:
    void updatePickedUpGateLocation(UILogicGate *gate, QPoint initialPosition);
    void startDrawingLine(QPoint location);
    void inputOrOutputPressed(qint32 id, QPushButton* button);
    void deleteGate(UILogicGate *gate);
    void enableButtons();
    void disableButtons();


public slots:
    void mousePressEvent(QMouseEvent* event);
    void startLineDrawing();

    void updateLocation(QPoint newLocation);


};

#endif // UILOGICGATE_H
