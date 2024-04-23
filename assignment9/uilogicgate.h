#ifndef UILOGICGATE_H
#define UILOGICGATE_H

#include <QLabel>
#include <QPushButton>
#include <QVector>

///
///  Written for CS 3505 Final Project, by Duncan Gilbreath, Ty Corbin, Will Campbell, Austin Laguette, and Indigo Barber
/// \brief This class represents a logic gate in the UI that can be
/// placed and connected to other gates.
///
class UILogicGate : public QLabel
{
    Q_OBJECT

public:
    // Constructor for custom logic label
    UILogicGate(QWidget* parent, qint32 id, QString operationName = "DEF", qint32 inputCount = 2, qint32 outputCount = 1);
    ~UILogicGate();

    // ID for tracking
    qint32 id;

    // If this gate has been picked up
    bool pickedUp;

    // Input and output buttons
    QVector<QPushButton*> inputs;
    QVector<QPushButton*> outputs;

    // Can this gate be moved bool
    bool canBeMoved = true;
private:

    // Location states for moving gates and physics
    QPoint dragStartPosition;
    bool drawingLine;
    QPoint lineStart;
    QPoint lineEnd;
    QPushButton* activeButton;

signals:

    /// \brief updatePickedUpGateLocation signal to slot the new position of the gate
    void updatePickedUpGateLocation(UILogicGate *gate, QPoint initialPosition);

    /// \brief startDrawingLine at this location
    void startDrawingLine(QPoint location);

    /// \brief inputOrOutputPressed signal whether input or output button has been pressed, send the button object
    void inputOrOutputPressed(qint32 id, QPushButton* button);

    /// \brief deleteGate delete this gate from whatever list is connected
    void deleteGate(UILogicGate *gate);

    ///
    /// \brief enableButtons
    /// enable buttons for connection
    void enableButtons();

    ///
    /// \brief disableButtons
    /// disable the buttons
    void disableButtons();


public slots:

    ///
    /// \brief mousePressEvent
    /// \param event
    /// handler for mouse press events
    void mousePressEvent(QMouseEvent* event);

    ///
    /// \brief startLineDrawing
    /// start drawing a line off this gate
    void startLineDrawing();

    ///
    /// \brief updateLocation
    /// \param newLocation
    ///update the location with the point
    void updateLocation(QPoint newLocation);


};

#endif // UILOGICGATE_H
