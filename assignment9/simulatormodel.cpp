#include "simulatormodel.h"
#include <iostream>
#include <QQueue>

simulatorModel::simulatorModel()
{
    //testing example, remove later
    gateNode testNode(1, 2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
        outputs[0] = inputs[0] && inputs[1];
    }, this);

    levelInputs.append(new gateNode(0, 1, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
        outputs[0] = inputs[0];
    }, this));

    levelOutputs.append(new gateNode(2, 1, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
        outputs[0] = inputs[0];
    }, this));

    connect(0, 0, 1, 0); //level in to testNode 0
    connect(1, 0, 2, 0); //testNode to levelout
    connect(2, 0, 1, 1); //levelout to testNode 1 (circular)
    std::cout << "simulation check?: " << canBeSimulated() << std::endl;
}

simulatorModel::gateNode::gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc, simulatorModel* parentModel)
    : id(id)
    , evaluator(evaluatorFunc)
{
    for (int c = 0; c < inputCount; c++)
    {
        inputFromNodes.append(nullptr);
        inputStates.append(false);
    }
    for (int c = 0; c < outputCount; c++)
    {
        outputToNodes.append(nullptr);
        outputStates.append(false);
    }

    parentModel->allGates[id] = this;
}

void simulatorModel::gateNode::evaluate()
{
    evaluator(inputStates, outputStates);
}

void simulatorModel::connect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* reciever = allGates.value(receivingId);
    giver->outputToNodes[outputIndex] = reciever;
    reciever->inputFromNodes[inputIndex] = giver;
}
void simulatorModel::disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* reciever = allGates.value(receivingId);
    giver->outputToNodes[outputIndex] = nullptr;
    reciever->inputFromNodes[inputIndex] = nullptr;
}

void simulatorModel::resetGateStates()
{
    for (gateNode* g : allGates.values())
    {
        for (int i = 0; i < g->inputStates.count(); i++)
            g->inputStates[i] = false;
        for (int i = 0; i < g->outputStates.count(); i++)
            g->outputStates[i] = false;
    }
}

bool simulatorModel::canBeSimulated()
{
    QMap<gateNode*, qint32> inDegrees;
    QQueue<gateNode*> frontier;
    for (gateNode* in : levelInputs)
        frontier.enqueue(in);

    while(!frontier.empty())
    {
        gateNode* currentGate = frontier.dequeue();

        for (gateNode* outputGate : currentGate->outputToNodes)
        {
            if (!outputGate) //skip this output if it's not connected
                continue;

            if (inDegrees.contains(outputGate))
                inDegrees[outputGate]++;
            else
                inDegrees[outputGate] = 0;

            if (inDegrees[outputGate] > outputGate->inputFromNodes.count()) //means this node has been visited more times than it has incoming connections, thus there must be some circularity
                return false;

            frontier.enqueue(outputGate);
        }
    }
    return true;
}
