#include "simulatormodel.h"
#include <iostream>

simulatorModel::simulatorModel()
{
    //testing example, remove later
    gateNode testNode(2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
        outputs[0] = inputs[0] && inputs[1];
    });

    testNode.inputStates[0] = true;
    testNode.evaluate();
    std::cout << "one input on: " << testNode.outputStates[0] << std::endl;
    testNode.inputStates[1] = true;
    testNode.evaluate();
    std::cout << "two inputs on: " << testNode.outputStates[0] << std::endl;
}

simulatorModel::gateNode::gateNode(qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc) : evaluator(evaluatorFunc)
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
