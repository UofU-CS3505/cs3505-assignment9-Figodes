#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QDialog>
#include <Box2D/Box2D.h>
#include <QTimer>

///
/// NewSpriteWindow class for assignment 9, CS 3505, by Duncan G., Austin L, Ty C, Indigo B, and Will C.
/// This is a class for the window that pops up when the game is launched
///

namespace Ui {
class welcomeScreen;
}

class welcomeScreen : public QDialog
{
    Q_OBJECT

public:
    explicit welcomeScreen(QWidget *parent = nullptr);
    ~welcomeScreen();

private slots:

    ///
    /// \brief updateWorld - updates the world for the box2d physics
    ///
    void updateWorld();

    ///
    /// \brief continueButtonClicked - method to check how to proceed if the continue button is clicked
    ///
    void continueButtonClicked();

signals:

    ///
    /// \brief windowClosed - signal to start the game when the window is closed
    ///
    void windowClosed();

protected:

    ///
    /// \brief closeEvent - overriden method to make the game appear when the x button is clicked on the window
    /// \param event - the event
    ///
    void closeEvent(QCloseEvent *event);


private:
    Ui::welcomeScreen *ui;

    // stuff for box2d
    b2World world;
    b2Body* body;
    QTimer timer;

    // bool to see if the game should begin
    bool continueToGame;
};

#endif // WELCOMESCREEN_H
