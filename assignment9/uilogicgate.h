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
    qint32 id;
private:
    QVector<QPushButton*> inputs;
    QVector<QPushButton*> outputs;
    QPushButton* input1;
    QPushButton* input2;
    QPushButton* output;
    QPoint dragStartPosition;
    bool pickedUp;
    bool drawingLine;
    QPoint lineStart;
    QPoint lineEnd;
    QPushButton* activeButton;

signals:
    void updateLocation(UILogicGate *gate, QMouseEvent* event);


public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void startLineDrawing();
    void endLineDrawing();

protected:
    void paintEvent(QPaintEvent* event);
};

#endif // UILOGICGATE_H
