#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SimulatorModel.h"
#include "uilogicgate.h"
#include <QSet>
#include "gatetypes.h"
#include <QPair>
#include "welcomescreen.h"
#include <mutex>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

///
///  Written for CS 3505 Final Project, by Duncan Gilbreath, Ty Corbin, Will Campbell, Austin Laguette, and Indigo Barber
/// \brief The main window of the game, allowing the user to make circuits,
/// interact with the game, and see the results of their circuits.
///
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    UILogicGate *pickedUpGate;

    // Model instantiation
    SimulatorModel* model;

    // Id counter for gates that get placed
    qint32 idCounter;

    // Map of gates with their ID and UILogicGate object
    QMap<qint32, UILogicGate*> gates;

    // Welcome screen instantiation
    welcomeScreen welcomescreen;

    // Box2D world object
    b2World* world;

    // Map of the bodies for box2d
    QMap<qint32, b2Body*> bodies;

    // Instantiate timer for timing
    QTimer* timer;

    // Bool states for different functionality
    bool playFinishAnimation = false;
    bool levelSuccess = false;
    bool connectionBeingDrawn;

    // Save state for button being connected for connecting buttons
    QPushButton* buttonBeingConnected;

    // Gate being connected
    qint32 connectingGate;

    // Lists of buttons that represent connections between gates
    QSet<QPushButton*> inputButtons;
    QSet<QPushButton*> outputButtons;

    // List of gates
    QSet<UILogicGate*> levelInOutGates;

    struct Wire
    {
        QPushButton* first, * second; //first and second imply output to input order, first should alwas=ys be an outputting button and vise versa
        QColor color;
        bool operator==(const Wire& other);
    };
    QVector<Wire> uiButtonConnections;

    bool eventFilter(QObject *obj, QEvent *event);
    void trackButtonsOn(UILogicGate* quarry);

    std::mutex mtx;

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
    void showInputs(const QVector<bool>& inputs);
    /// \brief showOutputs Same as showInputs, but for level outputs
    void showOutputs(const QVector<bool>& outputs);

    //Overrides that respond to the mouse hovering and clicking anywhere in the main window.
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);

    /// \brief addGate Adds a gate of the specified type to the view, additonally signalling the model to do the same.
    void addGate(GateTypes gateType);
    /// \brief deleteGate Removes the specified gate from the view and signals the model to do the same.
    void deleteGate(UILogicGate *gate);

    /// \brief Clears all currently placed logic gates in UI.
    void clearGates();

    void connectionBeingMade(qint32 gate, QPushButton* button);

    void paintEvent(QPaintEvent* event);

    void showWindow();

    /// \brief levelEndAnimation Starts the level-end animation
    void levelEndAnimation();
    /// \brief updateFinishGates Updates positions each frame for level-end animation
    void updateFinishGates();
    /// \brief stopTimer Stops the timer responsible for updating the level-end animation
    void stopTimer();

    void disableAllButtons();
    void enableAllButtons();
    void simulationEnd(bool success);

    /// \brief colorAllWires Colors all wires with the given color
    void colorAllWires(QColor color);
    /// \brief colorWire Changes the color of a specific wire going from the outputIndexth output of the node with giverId
    /// to the inputIndexth input on the node with receiverId.
    void colorWire(qint32 giverId, qint32 outputIndex, qint32 receiverId, qint32 inputIndex, QColor newColor);

    /// \brief displayLevelFailed Displays a message informing the user why their circuit failed.
    /// \param failedInput The input sequence that cause a wrong output
    /// \param expectedOutput The expected output for the failed input
    /// \param actualOutput The result given by the circuit for the failed input
    void displayLevelFailed(QVector<bool> failedInput, QVector<bool> expectedOutput, QVector<bool> actualOutput);
    /// \brief displayInvalidCircuit Informas the user that their circuit cannot be simulated
    void displayInvalidCircuit();

    QString boolVectorToString(const QVector<bool>& vec);

    void retryClicked();
    void gameCompleted();

signals:

    // Signal to tell the model that a connection has been made between logic gates
    void connectionDrawn(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex);
    void connectionBroken(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex);

    /// \brief Signal for when a level should start simulating.
    void startSimulation();

    /// \brief Signal for when a new gate button is clicked, sends the new gate created to model
    void newGateCreated(qint32 gateID, GateTypes gt);

    void removeGateFromModel(qint32 gateID);
};
#endif // MAINWINDOW_H
