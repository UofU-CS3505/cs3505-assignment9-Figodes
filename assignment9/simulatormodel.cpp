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

///
/// \brief simulatorModel::gateNode::gateNode Creates a new node in the model for a gate, with the specified number of inputs and outputs
/// \param evaluatorFunc A function pointer (I would recommend using a lambda) for a function that sets the elements of outputs based on the elements of inputs. This should represent the operation of this gate.
///
simulatorModel::gateNode::gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc)
    : evaluator(evaluatorFunc)
    , id(id)
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

///
/// \brief simulatorModel::gateNode::evaluate Helper that uses a node's evaluation function to evaluate its outputs based on its inputs.
/// This will chnage the contents of outputStates.
///
void simulatorModel::gateNode::evaluate()
{
    evaluator(inputStates, outputStates);
}
