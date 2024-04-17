#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SimulatorModel.h"
#include "uilogicgate.h"
#include <QSet>
#include "gatetypes.h"
#include <QPair>
#include "welcomescreen.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UILogicGate *pickedUpGate;
    QSet<QPushButton*> inputButtons;
    QSet<QPushButton*> outputButtons;

    QPoint dragStartPosition;

    SimulatorModel* model;
    QLabel* gatePlaceholder = nullptr;
    qint32 currentGateType;
    qint32 idCounter;
    bool connectionBeingDrawn;
    QPushButton* buttonBeingConnected;
    qint32 connectingGate;

    struct Wire
    {
        QPushButton* first, * second;
        QColor color;
    };

    QVector<Wire> uiButtonConnections;

    QVector<UILogicGate*> gates;
    bool eventFilter(QObject *obj, QEvent *event);
    void trackButtonsOn(UILogicGate* quarry);

    welcomeScreen welcomescreen;
    b2World world;
    b2Body* body;
    QTimer timer;


    void showWelcomeScreen();
    bool isConnectionAlreadyExists(QPushButton* button1, QPushButton* button2);
    bool isConnectionValid(QPushButton* button1, QPushButton* button2, qint32 gate1, qint32 gate2);

public slots:

    void updatePickedUpGate(UILogicGate *gate, QPoint initialPosition);

    /// \brief Sets up the given level
    void setupLevel(Level level);

    /// \brief Called when startButton is clicked, just calls simulationStarted.
    void onStartClicked();

    /// \brief Turns the 3 input buttons on and off to show the given inputs.
    /// \param inputs: an array of 3 bools.
    void showInputs(QVector<bool> inputs);
    void showOutputs(QVector<bool> outputs);

    void mouseMoveEvent(QMouseEvent* event);
    void addGate(GateTypes gateType);
    void mouseReleaseEvent(QMouseEvent* event);

    /// \brief Clears all currently placed logic gates in UI.
    void clearGates();

    void connectionBeingMade(qint32 gate, QPushButton* button);

    void paintEvent(QPaintEvent* event);

    void showWindow();

    void victoryAnimation();

    void updateVictoryGates();
    void disableAllButtons();
    void enableAllButtons();
    void simulationEnd(bool success);

    void colorAllWires(QColor color);
    void colorWire(qint32 giverId, qint32 outputIndex, qint32 receiverId, qint32 inputIndex, QColor newColor);

signals:

    // Signal to tell the model that a connection has been made between logic gates
    void connectionDrawn(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex);

    /// \brief Signal for when a level should start simulating.
    void startSimulation();

    /// \brief Signal for when a new gate button is clicked, sends the new gate created to model
    void newGateCreated(qint32 gateID, GateTypes gt);
};
#endif // MAINWINDOW_H
