#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include "uilogicgate.h"

class simulatorModel
{
public:
    simulatorModel();
    qint32 currentLevel;
    QVector<QString> levelDescriptions;

private:
    QVector <UILogicGate> gates;
};

#endif // SIMULATORMODEL_H
