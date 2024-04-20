#ifndef SIMULATORMODEL_H
#define SIMULATORMODEL_H

#include <QVector>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QColor>
#include "level.h"
#include "gatetypes.h"

/// \brief This class the circuit that the user has created, and handles simulating it,
/// testing its behavior for different inputs, and moving between levels.
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
        //The sets of nodes that this node takes inputs from -- each input having its own set -- and output terminal each input it connected to.
        QVector<QSet<QPair<gateNode*, qint32>>> inputFromNodes;
        //The set of nodes that this node gives outputs to
        QVector<QSet<gateNode*>> outputToNodes;
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
        ///
        /// \brief hasOutputted Whether or not this node has produced an output in the simulation yet
        ///
        bool hasOutputted;
        ///
        /// \brief recursiveDFSLoopDetected A recursive algorithm that detects any loops on the path forward from this node
        /// \param visitedIds A list of IDs of nodes that this recursive run has visited, starts empty, passed down to each successive recursive call.
        ///
        bool recursiveDFSLoopDetected(QSet<qint32>* visitedIds);

    private:
        std::function<void(QVector<bool> currentInputs, QVector<bool>& futureOutputs)> evaluator;
    };

    ///
    /// \brief Gets the Nth input sequence of the current level, tells the model to display it, and prepares it for simulation in the model
    /// Assumes level input nodes have one output state
    ///
    void setNthInputSequence(qint32 n);
    ///
    /// \brief currentInput The index of the input sequence currently being simulated
    ///
    qint32 currentInput;
    ///
    /// \brief activeGates Gates which have received some, but not all of their inputs, and have not outputted yet
    ///
    QSet<gateNode*> activeGates;
    ///
    /// \brief toBoolVector Returns the output states of the given gates in an order matching their order in the argument list.
    ///
    QVector<bool> toBoolVector(QVector<gateNode*> gates);

public:
    SimulatorModel();
    ///
    /// \brief currentLevel The index of the currently displayed/modelled level
    ///
    qint32 currentLevel;
    ///
    /// \brief levels A list of all levels
    ///
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
    ///
    /// \brief simulateInput Begins the simulaiton for one of the level's given inputs
    ///
    void simulateInput();
    ///
    /// \brief simulateOneIteration Simulates one iteration of the simulation, updating view graphics as it goes and checking whether the simulaiton is complete
    ///
    void simulateOneIteration();
    ///
    /// \brief endSimulation signals view that the simulation is over
    ///
    void endSimulation(bool levelSucceeded);
    ///
    /// \brief initializeView Sends necessary data likethe first level
    ///
    void initializeView();

public slots:
    ///
    /// \brief addNewGate Adds a gate of the specified type
    /// \param gateID The ID of the new gate
    ///
    void addNewGate(qint32 gateID, GateTypes gt);
    ///
    /// \brief removeGate Removes a gate and all its connections
    ///
    void removeGate(qint32 gateID);
    ///
    /// \brief setupNextLevel Initializes and loads a new level
    ///
    void setupNextLevel();
    ///
    /// \brief startSimulation Initializes values so the simluation can start
    ///
    void startSimulation();
    ///
    /// \brief resetLevel Completely clears all existent gates and reload the current level
    ///
    void resetLevel();

signals:
    void disableEditing();
    void enableEditing();
    void invalidCircuit(); //indicates the circuit cannot be simulated
    void levelFinished(bool succeeded); // enable button to move to next level in view, trigger celebration
    void displayNewLevel(Level level);
    ///
    /// \brief inputsSet Set the appearance of the level inputs
    ///
    void inputsSet(const QVector<bool>& inputs);
    ///
    /// \brief inputsSet Set the appearance of the level outputs
    ///
    void outputsSet(const QVector<bool>& outputs);
    void colorConnection(qint32 giverId, qint32 outputIndex, qint32 receiverId, qint32 inputIndex, QColor newColor);
    void colorAllConnections(QColor color);
    void incorrectCircuit(QVector<bool> failedInput, QVector<bool> expectedOutput, QVector<bool> actualOutput);
    void endGame();
};

#endif // SIMULATORMODEL_H
