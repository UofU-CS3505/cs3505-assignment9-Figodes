#include "simulatormodel.h"
#include <iostream>
#include <QQueue>
#include <QTimer>
#include <QSet>
#include <QtMath>

SimulatorModel::SimulatorModel()
{
    std::cout<<"in constructor"<<std::endl;
    currentLevel = 0;

    //load levels from file
    levels.append(Level("testLevel", QVector<QVector<bool>>{{0},{1}}, 1, 1)); //FOR TESTING, NOT PERMANENT


    // //testing example, remove later
    // gateNode* testNode = new gateNode(1, 2, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
    //     outputs[0] = inputs[0] || inputs[1];
    // }, this);
    // levelInputs.append(new gateNode(0, 0, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
    //     outputs[0] = true;
    // }, this));
    // levelOutputs.append(new gateNode(2, 1, 0, [=](QVector<bool> inputs, QVector<bool>& outputs) {
    //     //nothing
    // }, this));
    // connect(0, 0, 1, 0); //level in to testNode 0
    // connect(1, 0, 2, 0); //testNode to levelout
    // //connect(2, 0, 1, 1); //levelout to testNode 1 (circular)
    // std::cout << "simulation check?: " << canBeSimulated() << std::endl;
    // startSimulation();
    // //end of testing example
}

SimulatorModel::gateNode::gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc, SimulatorModel* parentModel)
    : id(id)
    , evaluator(evaluatorFunc)
{
    for (int c = 0; c < inputCount; c++)
    {
        inputFromNodes.append(QSet<QPair<gateNode*, qint32>>());
        inputStates.append(false);
    }
    for (int c = 0; c < outputCount; c++)
    {
        outputToNodes.append(QSet<gateNode*>());
        outputStates.append(false);
    }

    parentModel->allGates[id] = this;
    hasOutputted = false;
}

void SimulatorModel::gateNode::evaluate()
{
    evaluator(inputStates, outputStates);
}

void SimulatorModel::connect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    std::cout << "connection made!" << std::endl;
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);
    giver->outputToNodes[outputIndex].insert(receiver);
    receiver->inputFromNodes[inputIndex].insert(QPair<gateNode*, qint32>(giver, outputIndex));
}
void SimulatorModel::disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);
    giver->outputToNodes[outputIndex].remove(receiver);
    receiver->inputFromNodes[inputIndex].remove(QPair<gateNode*, qint32>(giver, outputIndex));
}

void SimulatorModel::resetGateStates()
{
    for (gateNode* g : allGates.values())
    {
        for (int i = 0; i < g->inputStates.size(); i++)
            g->inputStates[i] = false;
        for (int i = 0; i < g->outputStates.size(); i++){
            std::cout<<"outputStates size = "<<g->outputStates.size()<<", i = "<<i<<std::endl;
            if(g->outputStates.size() == 192)
                std::cout<<"reached mystery 192 case"<<std::endl;

            g->outputStates[i] = false;
        }
        g->hasOutputted = false;
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

void SimulatorModel::setNthInputSequence(qint32 n){
    QVector<bool> inputSet = levels[currentLevel].getLevelInput(n);
    for(int i = 0; i < levelInputs.size(); i++)
        levelInputs[i]->outputStates[0] = inputSet[i]; //prepare actual inputs in model

    emit inputsSet(inputSet);
}

void SimulatorModel::startSimulation(){   
    //lock ui for input testing
    //std::cout<<"in startSimulation"<<std::endl;
    emit disableEditing();
    currentInput = 0;
    if (canBeSimulated())
        simulateInput();
    else
    {
        std::cout << "CANNOT SIMULATE" << std::endl;
        emit invalidLevel();
        emit enableEditing(); //may move this into the view as a response to invalidLevel
    }
}

void SimulatorModel::simulateInput(){
    std::cout<<"in simulateInput, simulating input "<<currentInput<<std::endl;
    if(currentInput == qPow(2, levelInputs.size())){
        endSimulation();
        return;
    }

    activeGates.clear();
    for (gateNode* levelInput : levelInputs)
        activeGates.insert(levelInput);

    resetGateStates();
    setNthInputSequence(currentInput);
    simulateOneIteration();
}

void SimulatorModel::simulateOneIteration(){
    std::cout<<"in simulateOneIteration()"<<std::endl;
    //simulate iteration, update view
    QSet<gateNode*> spentGates;
    QSet<gateNode*> futureGates;
    for (gateNode* activeGate : activeGates)
    {
        bool ready = true;
        for (qint32 inputIndex = 0; inputIndex < activeGate->inputFromNodes.size(); inputIndex++)//check that all inputting nodes have evaluated, and take their inputs if they have
        {
            auto inputTerminalSet = activeGate->inputFromNodes[inputIndex];
            for (QPair<gateNode*, qint32> connection : inputTerminalSet)
            {
                gateNode* inputtingGate = connection.first;
                if (!inputtingGate->hasOutputted)
                    ready = false;
                else
                {
                    qint32 indexOfConnectedOutput = connection.second;
                    if (inputtingGate->outputStates[indexOfConnectedOutput]) //the result of this is that if any of the inputs to this terminal are true, then the terminal state is true
                        activeGate->inputStates[inputIndex] = true;
                }
            }
        }
        if (!ready) //if not all inputs have evaluated, skip this gate for this iteration
            continue;

        //by this point the active gate has all it inputs
        activeGate->evaluate();
        //this is where to signal the view to light up the wires coming out of this gate according to its output states
        activeGate->hasOutputted = true;

        spentGates.insert(activeGate);// queue this gate to be removed from the list of active gates
        for (qint32 outputIndex = 0; outputIndex < activeGate->outputToNodes.size(); outputIndex++) //add this gate's outputs to the list of future active gates
        {
            auto outputTerminalSet = activeGate->outputToNodes[outputIndex];
            futureGates.unite(outputTerminalSet);
        }
    }
    activeGates.unite(futureGates);
    activeGates.subtract(spentGates); //removes the gates that were evaluated this iteration

    if(!activeGates.empty()) //not done with this input
        QTimer::singleShot(1000, this, &SimulatorModel::simulateOneIteration);
    else{
        emit outputsSet(toBoolVector(levelOutputs));

        for(int i = 0; i < levelOutputs.size(); i++){
            if(levelOutputs[i]->inputStates[0] == levels[currentLevel].getExpectedOutput(currentInput)[i])
                std::cout << "input " << i << " is correct" << std::endl; //what happens based on if inputs are right?
            else
                emit levelFailed();
        }
        if (currentInput == qPow(2, levelInputs.size()) - 1) //last input finished simulating, no wrong outputs
            endSimulation();
        else //more input sets to simulate
        {
            currentInput++;
            QTimer::singleShot(1000, this, &SimulatorModel::simulateInput);
        }
    }
}

void SimulatorModel::endSimulation(){
    emit levelComplete();
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
    case GateTypes::LEVEL_IN:
        newNode = new gateNode(gateID, 0, 1, [=](QVector<bool> inputs, QVector<bool>& outputs) {
                ;
            }, this);
        levelInputs.append(newNode);
        break;
    case GateTypes::LEVEL_OUT:
        newNode = new gateNode(gateID, 1, 0, [=](QVector<bool> inputs, QVector<bool>& outputs) {
                ;
            }, this);
        levelOutputs.append(newNode);
        break;
    }

    if (newNode) {
        allGates.insert(gateID, newNode);
        std::cout << "new node, " << gateID << ", added to model" << std::endl;
    }
}

void SimulatorModel::loadNextLevel()
{
    currentLevel++;
    allGates.clear();
    emit displayNewLevel(levels[currentLevel]);
    emit enableEditing();
}

QVector<bool> SimulatorModel::toBoolVector(QVector<gateNode*> gates){
    QVector<bool> result;
    for(gateNode* gate: gates)
        result.append(gate->inputStates[0]);
    return result;
}

void SimulatorModel::initializeView()
{
    emit displayNewLevel(levels[0]); //sends first level
}
