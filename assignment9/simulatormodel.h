#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QVector>
#include <QMap>
#include <QObject>
#include "level.h"

class SimulatorModel : public QObject
{
    Q_OBJECT

private:
    ///
    /// \brief The gateNode class All lists of inputs and outputs are indexed based on height. 0 corresponds to the highest button of the inputs/outputs.
    ///
    class gateNode
    {
    public:
        ///
        /// \brief SimulatorModel::gateNode::gateNode Creates a new node in the model for a gate, with the specified number of inputs and outputs
        /// \param evaluatorFunc A function pointer (I would recommend using a lambda) for a function that sets the elements of outputs based on the elements of inputs. This should represent the operation of this gate.
        ///
        gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc, SimulatorModel* parentModel);
        //The nodes that this node takes inputs from
        QVector<gateNode*> inputFromNodes;
        //The nodes that this node gives outputs to
        QVector<gateNode*> outputToNodes;
        //Whether the input/output at a given position are receiving/giving a signal
        QVector<bool> inputStates;
        QVector<bool> outputStates;
        ///
        /// \brief SimulatorModel::gateNode::evaluate Helper that uses a node's evaluation function to evaluate its outputs based on its inputs.
        /// This will chnage the contents of outputStates.
        ///
        void evaluate();
        // Not sure if gates actually need to know their ids
        qint32 id;

    private:
        std::function<void(QVector<bool> currentInputs, QVector<bool>& futureOutputs)> evaluator;
    };

    QVector<bool> intToInputSequence(qint32 integer);

public:
    SimulatorModel();
    qint32 currentLevel;
    QVector<Level> levels;
    /// All the gates in the model, keyed by their id
    QMap<qint32, gateNode*> allGates;
    /// Right now I'm thinking of just making the level inputs special nodes that only have either outputs or inputs
    QVector<gateNode*> levelInputs;
    QVector<gateNode*> levelOutputs;
    ///
    /// \brief disconnect Conects two gates to eachother. Input/output positions are based on spatial height, 0 indicating the highest button on that side of a gate.
    /// An input index of 1 on an AND gate would indicate the lower input.
    /// \param givingId The id of the gate which should be outputting to the other gate.
    /// \param outputIndex The position of the output button which was connected.
    /// \param receivingId The id of the gate which should be getting input from the other gate.
    /// \param inputIndex The position of the input button which was connected.
    ///
    void connect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex);
    ///
    /// \brief disconnect Disconects two gates from eachother. Input/output positions are based on spatial height, 0 indicating the highest button on that side of a gate.
    /// An input index of 1 on an AND gate would indicate the lower input.
    /// \param givingId The id of the gate which was outputting to the other gate.
    /// \param outputIndex The position of the output button which was disconnected.
    /// \param receivingId The id of the gate which was getting input from the other gate.
    /// \param inputIndex The position of the input button which was disconnected.
    ///
    void disconnect(qint32 givingId, qint32 outputIndex, qint32 receivingId, qint32 inputIndex);
    ///
    /// \brief resetGateStates Resets the states of all inputs/outputs, essentially resets all on/off states.
    ///
    void resetGateStates();
    ///
    /// \brief canBeSimulated Checks whether the current model can be simulated. This could be because the current model contains a cycle.
    ///
    bool canBeSimulated();
    //since we want to make the simulation take time, I thinik we'll need to break it up like this. A bunch of these robably need to be slots
    void startSimulation();
    void simulateInput(QVector<bool> inputs, QVector<bool> expectedOutputs);
    void simulateOneIteration();
    void endSimulation();
};

#endif // SIMULATORMODEL_H
