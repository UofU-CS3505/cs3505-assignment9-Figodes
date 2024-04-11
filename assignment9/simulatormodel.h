#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QVector>
#include <QMap>

class simulatorModel
{
private:
    ///
    /// \brief The gateNode class All lists of inputs and outputs are indexed based on height. 0 corresponds to the highest button of the inputs/outputs.
    ///
    class gateNode
    {
    public:
        ///
        /// \brief simulatorModel::gateNode::gateNode Creates a new node in the model for a gate, with the specified number of inputs and outputs
        /// \param evaluatorFunc A function pointer (I would recommend using a lambda) for a function that sets the elements of outputs based on the elements of inputs. This should represent the operation of this gate.
        ///
        gateNode(qint32 id, qint32 inputCount, qint32 outputCount, std::function<void(QVector<bool> , QVector<bool>&)> evaluatorFunc);
        //The nodes that this node takes inputs from
        QVector<gateNode*> inputFromNodes;
        //The nodes that this node gives outputs to
        QVector<gateNode*> outputToNodes;
        //Whether the input/output at a given position are receiving/giving a signal
        QVector<bool> inputStates;
        QVector<bool> outputStates;
        ///
        /// \brief simulatorModel::gateNode::evaluate Helper that uses a node's evaluation function to evaluate its outputs based on its inputs.
        /// This will chnage the contents of outputStates.
        ///
        void evaluate();
        // Not sure if gates actually need to know their ids
        qint32 id;

    private:
        std::function<void(QVector<bool> currentInputs, QVector<bool>& futureOutputs)> evaluator;
    };

public:
    simulatorModel();
    qint32 currentLevel;
    QVector<QString> levelDescriptions;
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
};

#endif // SIMULATORMODEL_H
