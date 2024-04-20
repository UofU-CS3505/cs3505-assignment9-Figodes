#include "simulatormodel.h"
#include <QQueue>
#include <QTimer>
#include <QSet>
#include <QtMath>
#include <QColor>

SimulatorModel::SimulatorModel()
{
    currentLevel = 0;

    //load levels from file
    levels = Level::getLevelList();
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
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);
    giver->outputToNodes[outputIndex].insert(receiver);
    receiver->inputFromNodes[inputIndex].insert(QPair<gateNode*, qint32>(giver, outputIndex));
}
void SimulatorModel::disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex)
{
    gateNode* giver = allGates.value(givingId);
    gateNode* receiver = allGates.value(receivingId);

    qint32 timesGiverInputsIntoReceiver = 0;
    for (auto inputSet : receiver->inputFromNodes)
        for (auto inputter : inputSet)
            if (inputter.first->id == givingId)
                timesGiverInputsIntoReceiver++;
    if (timesGiverInputsIntoReceiver == 1) //only remove the tie if this is the only connection
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
            g->outputStates[i] = false;
        }
        g->hasOutputted = false;
    }
}

bool SimulatorModel::canBeSimulated()
{
    for (gateNode* levelIn : levelInputs)
    {
        auto visited = new QSet<qint32>();
        if (levelIn->recursiveDFSLoopDetected(visited))
            return false;
    }
    return true;
}

bool SimulatorModel::gateNode::recursiveDFSLoopDetected(QSet<qint32>* visitedIds)
{
    visitedIds->insert(id); //mark this node as visited
    for (auto outputToGroup : outputToNodes)
        for (gateNode* outputtedTo : outputToGroup)
        {
            if (visitedIds->contains(outputtedTo->id)) //already visited
                return true;
            bool loopDownstream = outputtedTo->recursiveDFSLoopDetected(visitedIds);
            if (loopDownstream)
                return true;
        }
    visitedIds->remove(id); //unmark this node
    return false;
}

void SimulatorModel::setNthInputSequence(qint32 n){
    QVector<bool> inputSet = levels[currentLevel].getLevelInput(n);
    for(int i = 0; i < levelInputs.size(); i++)
        levelInputs[i]->outputStates[0] = inputSet[i]; //prepare actual inputs in model

    emit inputsSet(inputSet);
}

void SimulatorModel::startSimulation(){   
    //lock ui for input testing
    emit disableEditing();
    currentInput = 0;
    if (canBeSimulated())
        simulateInput();
    else
    {
        emit invalidCircuit();
        emit enableEditing();
    }
}

void SimulatorModel::simulateInput(){
    activeGates.clear();
    for (gateNode* levelInput : levelInputs)
        activeGates.insert(levelInput);
    for (gateNode* gate : allGates)
        for (auto inputSet : gate->inputFromNodes)
            if (inputSet.size() == 0) //this gate has any inputs that don't receive from anywhere
                activeGates.insert(gate);

    emit colorAllConnections(Qt::black);
    resetGateStates();
    setNthInputSequence(currentInput);
    simulateOneIteration();
}

void SimulatorModel::simulateOneIteration(){
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
        //update wire colors
        for (qint32 outputIndex = 0; outputIndex < activeGate->outputStates.size(); outputIndex++)
        {
            QColor wireColor = Qt::darkRed;
            if (activeGate->outputStates[outputIndex])
                wireColor = Qt::green;
            for (auto receivingGate : activeGate->outputToNodes[outputIndex])
                for (qint32 inputIndex = 0; inputIndex < receivingGate->inputStates.size(); inputIndex++)
                {
                    auto receivingInputSet = receivingGate->inputFromNodes[inputIndex];
                    for (auto incomingConnection : receivingInputSet)
                        if (incomingConnection.first == activeGate) //selects all connections this gate is outputting on
                            emit colorConnection(activeGate->id, outputIndex, receivingGate->id, inputIndex, wireColor);
                }
        }

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
            if(levelOutputs[i]->inputStates[0] != levels[currentLevel].getExpectedOutput(currentInput)[i]){
                endSimulation(false);
                return; //end sim early if level failed at any point
            }
        }

        if (currentInput == qPow(2, levelInputs.size()) - 1) //last input finished simulating, no wrong outputs
            endSimulation(true);
        else //more input sets to simulate
        {
            currentInput++;
            QTimer::singleShot(1000, this, &SimulatorModel::simulateInput);
        }
    }
}

void SimulatorModel::endSimulation(bool levelSucceeded){
    if(levelSucceeded){
        levelInputs.clear();
        levelOutputs.clear();

        if (currentLevel == levels.size() - 1)//just finished final level
        {
            emit endGame();
            return;
        }
    }
    else{
        QVector<bool> actualOutputs;
        for(gateNode* output: levelOutputs)
            actualOutputs.append(output->inputStates[0]);

        emit incorrectCircuit(levels[currentLevel].getLevelInput(currentInput),
            levels[currentLevel].getExpectedOutput(currentInput),
            actualOutputs);
    }
    emit levelFinished(levelSucceeded);
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

    if (newNode)
        allGates.insert(gateID, newNode);
}

void SimulatorModel::setupNextLevel()
{
    if(currentLevel < levels.size() - 1)
        currentLevel++;

    allGates.clear();
    levelInputs.clear();
    levelOutputs.clear();
    emit displayNewLevel(levels[currentLevel]);
    emit enableEditing();
}

void SimulatorModel::resetLevel(){
    allGates.clear();
    levelInputs.clear();
    levelOutputs.clear();
    emit displayNewLevel(levels[currentLevel]);
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

void SimulatorModel::removeGate(qint32 gateId)
{
    if (!allGates.contains(gateId))
        return;
    //disconnect before deleting
    gateNode* toDelete = allGates[gateId];

    for (qint32 inputIndex = 0; inputIndex < toDelete->inputFromNodes.size(); inputIndex++) //remove input connections
    {
        auto inputSet = toDelete->inputFromNodes[inputIndex];
        for (auto inputter : inputSet)
        {
            disconnect(inputter.first->id, inputter.second, gateId, inputIndex);
        }
    }

    for (auto outputSet : toDelete->outputToNodes)
        for (gateNode* receiverFrom : outputSet)
            for (qint32 receiverInputIndex = 0; receiverInputIndex < receiverFrom->inputFromNodes.size(); receiverInputIndex++) //remove input connections
            {
                auto inputSet = receiverFrom->inputFromNodes[receiverInputIndex];
                for (auto inputter : inputSet)
                {
                    if (inputter.first == toDelete) //this connection is from the deleted node
                        disconnect(inputter.first->id, inputter.second, receiverFrom->id, receiverInputIndex);
                }
            }

    allGates.remove(gateId);
}
