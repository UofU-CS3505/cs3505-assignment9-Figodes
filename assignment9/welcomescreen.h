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

signals:
    void heightChanged(int height);

private:
    Ui::welcomeScreen *ui;
    b2World world;
    b2Body* body;
    QTimer timer;
};

#endif // WELCOMESCREEN_H
