#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "simulatorModel.h"
#include "uilogicgate.h"

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
    Ui::MainWindow* ui;
    simulatorModel* model;

public slots:
    void updateGateLocation(UILogicGate *gate, QMouseEvent *event);
    /// \brief Sets the level description to display the given text.
    /// \param text
    void setLevelDescription(QString text);

    /// \brief Called when startButton is clicked, just calls simulationStarted.
    void onStartClicked();

    /// \brief Turns the 3 input buttons on and off to show the given inputs.
    /// \param inputs: an array of 3 bools.
    void showInputs(bool inputs[]);

signals:

    // Signal to tell the model that a connection has been made between logic gates
    void connectionDrawn();

    /// \brief Signal for when a level should start simulating.
    void startSimulation();
};
#endif // MAINWINDOW_H
