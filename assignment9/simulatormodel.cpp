#include "simulatormodel.h"
#include <iostream>

simulatorModel::simulatorModel()
{
    //testing example, remove later
    gateNode testNode(0, 2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
        outputs[0] = inputs[0] && inputs[1];
    });

    testNode.inputStates[0] = true;
    testNode.evaluate();
    std::cout << "one input on: " << testNode.outputStates[0] << std::endl;
    testNode.inputStates[1] = true;
    testNode.evaluate();
    std::cout << "two inputs on: " << testNode.outputStates[0] << std::endl;
}

simulatorModel::gateNode::gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc)
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
    reciever->outputToNodes[inputIndex] = giver;
}
void simulatorModel::disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* reciever = allGates.value(receivingId);
    giver->outputToNodes[outputIndex] = nullptr;
    reciever->outputToNodes[inputIndex] = nullptr;
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
