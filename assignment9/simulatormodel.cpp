#include "simulatormodel.h"
#include <iostream>
#include <QQueue>
#include <QTimer>
#include <QSet>

SimulatorModel::SimulatorModel()
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

    currentInput = 0;
}

SimulatorModel::gateNode::gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc, SimulatorModel* parentModel)
    : id(id)
    , evaluator(evaluatorFunc)
{
    for (int c = 0; c < inputCount; c++)
    {
        inputFromNodes.append(QSet<gateNode*>());
        inputStates.append(false);
    }
    for (int c = 0; c < outputCount; c++)
    {
        outputToNodes.append(QSet<gateNode*>());
        outputStates.append(false);
    }

    parentModel->allGates[id] = this;
}

void SimulatorModel::gateNode::evaluate()
{
    evaluator(inputStates, outputStates);
}

void SimulatorModel::connect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);
    giver->outputToNodes[outputIndex].insert(receiver);
    receiver->inputFromNodes[inputIndex].insert(giver);
}
void SimulatorModel::disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);
    giver->outputToNodes[outputIndex].remove(receiver);
    receiver->inputFromNodes[inputIndex].remove(giver);
}

void SimulatorModel::resetGateStates()
{
    for (gateNode* g : allGates.values())
    {
        for (int i = 0; i < g->inputStates.count(); i++)
            g->inputStates[i] = false;
        for (int i = 0; i < g->outputStates.count(); i++)
            g->outputStates[i] = false;
    }
}

bool SimulatorModel::canBeSimulated()
{
    QMap<gateNode*, qint32> timesVisited; //how many times this algorithm visits the key node by traversing forward through the model
    QQueue<gateNode*> frontier;
    for (gateNode* in : levelInputs)
        frontier.enqueue(in);

    while(!frontier.empty())
    {
        gateNode* currentGate = frontier.dequeue();

        for (QSet outputTerminal : currentGate->outputToNodes)
        {
            for (gateNode* outputGate: outputTerminal) //all the gates that this particular output terminal outputs to
            {
                if (!outputGate) //skip this output if it's not connected
                    continue;

                if (timesVisited.contains(outputGate))
                    timesVisited[outputGate]++;
                else
                    timesVisited[outputGate] = 0;

                qint32 outputsInDegree = 0;
                for (QSet outputsInput: outputGate->inputFromNodes) //find the in-degree of this gate we're outputting to, counting the number of connections on each terminal
                    outputsInDegree += outputsInput.count();

                if (timesVisited[outputGate] > outputsInDegree) //means this node has been visited more times than it has incoming connections, thus there must be some circularity
                    return false;

                frontier.enqueue(outputGate);
            }
        }
    }
    return true;
}

QVector<bool> SimulatorModel::intToInputSequence(qint32 integer){
    QVector<bool> sequence;
    for(int i = 0; i < 3; i++)
        sequence.append((integer >> i) & 1); //get ith bit of integer

    return sequence;
}

void SimulatorModel::startSimulation(){
    simulateInput();
}

void SimulatorModel::simulateInput(){
    //if(currentInput == 8)
    endSimulation();
    return;

    QVector<bool> inputs = intToInputSequence(currentInput);
    QVector<bool> expectedOutputs = levels[currentLevel].getExpectedOutput(currentInput);

    simulateOneIteration();
}

void SimulatorModel::simulateOneIteration(){
    //simulate iteration, update view
    //if(not done)
    QTimer::singleShot(1000, this, &SimulatorModel::simulateOneIteration);
    //else
    currentInput++;
    QTimer::singleShot(1000, this, &SimulatorModel::simulateInput);
}

void SimulatorModel::endSimulation(){
    currentInput = 0;
    currentLevel++;
    //move to next level: clear circuits and ui
}

void SimulatorModel::addNewGate(qint32 gateID, GateTypes gateType) {
    // Temporarily declare a pointer to gateNode
    gateNode* newNode = nullptr;

    switch(gateType) {
    case GateTypes::AND:
        newNode = new gateNode(gateID, 2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
                outputs[0] = inputs[0] && inputs[1];
            }, this);
        break;
    case GateTypes::OR:
        newNode = new gateNode(gateID, 2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
                outputs[0] = inputs[0] || inputs[1];
            }, this);
        break;
    case GateTypes::NOT:
        newNode = new gateNode(gateID, 1, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
                outputs[0] = !inputs[0];
            }, this);
        break;
    }

    if (newNode) {
        allGates.insert(gateID, newNode);
        std::cout << "new node, " << gateID << ", added to model" << std::endl;
    }
}

