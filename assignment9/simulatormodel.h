#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QVector>

class simulatorModel
{
private:
    class gateNode
    {
    public:
        gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc);
        QVector<gateNode*> inputFromNodes;
        QVector<gateNode*> outputToNodes;
        QVector<bool> inputStates;
        QVector<bool> outputStates;
        void evaluate();
        qint32 id;

    private:
        std::function<void(QVector<bool> currentInputs, QVector<bool>& futureOutputs)> evaluator;
    };

public:
    simulatorModel();
    qint32 currentLevel;
    QVector<QString> levelDescriptions;
    QVector<gateNode*> allGates;
    QVector<gateNode*> levelInputs;
    QVector<gateNode*> levelOutputs;
};

#endif // SIMULATORMODEL_H
