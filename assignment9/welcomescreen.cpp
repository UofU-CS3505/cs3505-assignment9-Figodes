#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include <QPainter>

welcomeScreen::welcomeScreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::welcomeScreen),
    world(b2Vec2(0.0f, -10.0f)),
    body(nullptr)
{
    ui->setupUi(this);

    ui->gameDescription->hide();
    ui->controlDescription->hide();
    continueToGame = false;

    connect(ui->continueButton, &QPushButton::clicked, this, &welcomeScreen::continueButtonClicked);

    connect(&timer, &QTimer::timeout, this, &welcomeScreen::updateWorld);
    timer.start(1000 / 60);

    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world.CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2PolygonShape groundBox;

    // The extents are the half-widths of the box.
    groundBox.SetAsBox(50.0f, 10.0f);

    // Add the ground fixture to the ground body.
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    body = world.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

    fixtureDef.restitution = 0.9f;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);

}

welcomeScreen::~welcomeScreen()
{
    delete ui;
}

void welcomeScreen::updateWorld()
{
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    // Instruct the world to perform a single step of simulation.
    world.Step(timeStep, velocityIterations, positionIterations);

    b2Vec2 position = body->GetPosition();

    QPoint labelPos((this->width() - ui->welcomeText->width())/2, (position.y * 100));
    ui->welcomeText->move(labelPos);
}

void welcomeScreen::continueButtonClicked() {
    // if the description is hidden, then show it. otherwise, show the game and close the window.
    if (continueToGame) {
        emit windowClosed();
        accept();
    }
    else if (ui->gameDescription->isHidden()) {
        ui->welcomeText->hide();
        ui->gameDescription->show();
    }
    else if (ui->controlDescription->isHidden()) {
        ui->gameDescription->hide();
        ui->controlDescription->show();
        continueToGame = true;

    }
}

void welcomeScreen::closeEvent(QCloseEvent *event) {
    emit windowClosed();
    accept();
}

