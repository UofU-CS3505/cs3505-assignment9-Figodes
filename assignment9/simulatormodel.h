#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include "uilogicgate.h"

class simulatorModel
{
public:
    simulatorModel();

private:
    QVector <UILogicGate> gates;
};

#endif // SIMULATORMODEL_H
