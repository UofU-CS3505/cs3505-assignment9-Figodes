#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QVector>

class simulatorModel
{
private:
    class gateNode
    {
    public:
        gateNode(qint32 inputCount, qint32 outputCount, std::function<void()> evaluatorFunc);
        QVector<gateNode*> inputFromNodes;
        QVector<gateNode*> outputToNodes;
        QVector<bool> inputStates;
        QVector<bool> outputStates;
        std::function<void()> evaluate;
    };
public:
    simulatorModel();
};

#endif // SIMULATORMODEL_H
