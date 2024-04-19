#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QDialog>
#include <Box2D/Box2D.h>
#include <QTimer>

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
    void updateWorld();
    void continueButtonClicked();

signals:
    void heightChanged(int height);
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event); // Override close event


private:
    Ui::welcomeScreen *ui;
    b2World world;
    b2Body* body;
    QTimer timer;
    bool continueToGame;
};

#endif // WELCOMESCREEN_H
