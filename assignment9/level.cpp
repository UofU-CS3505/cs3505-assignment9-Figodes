#include "level.h"
#include <iostream>
#include <QtMath>

Level::Level(QString description, QVector<QVector<bool>> expectedOutputs, qint32 inputCount, qint32 outputCount)
    : inputCount(inputCount)
    , outputCount(outputCount)
    , description(description)
    , expectedOutputs(expectedOutputs)
{
    for (qint32 n = 0; n < qPow(2, inputCount); n++)
    {
        QVector<bool> inputGroup;
        for(int i = 0; i < inputCount; i++){
            bool bit = (n >> i) & 1; //get ith bit of integer
            inputGroup.append(bit);
        }
        levelInputs.append(inputGroup);
    }
}

Level::Level(QString description, std::function<void(QVector<bool> inputSet, QVector<bool>& outputSet)> levelFunction, qint32 inputCount, qint32 outputCount)
    : inputCount(inputCount)
    , outputCount(outputCount)
    , description(description)
{
    qint32 numberOfInputSets = qPow(2, inputCount);
    for (qint32 n = 0; n < numberOfInputSets; n++)
    {
        QVector<bool> inputGroup;
        for(int i = 0; i < inputCount; i++){
            bool bit = (n >> i) & 1; //get ith bit of integer
            inputGroup.append(bit);
        }
        levelInputs.append(inputGroup);
    }

    for (qint32 i = 0; i < numberOfInputSets; i++) //initialize outputs
    {
        expectedOutputs.append(QVector<bool>());
        for (int j = 0; j < outputCount; j++)
            expectedOutputs[i].append(false);
    }

    for (qint32 i = 0; i < numberOfInputSets; i++)
        levelFunction(levelInputs[i], expectedOutputs[i]); //generate appropriate outputs
}

QString Level::getDescription(){
    return description;
}

QVector<bool> Level::getExpectedOutput(qint32 inputIndex){
    return expectedOutputs[inputIndex];
}

QVector<bool> Level::getLevelInput(qint32 inputIndex)
{
    return levelInputs[inputIndex];
}

QVector<Level> Level::getLevelList(){
    QVector<Level> levels;
    levels.append(Level("In the table to the right, you can see which outputs map to which inputs."
        " Below is the workspace; on the left side of the workspace are the input ports, and on the right are the output ports. Ports can be connected just like logic gates.\n"
        "For this level, there is only one input and output, and the signal should match the output exactly, so all that's necessary is connecting the input to the output directly."
        "Once you've connected the each side, press the Start Simulation! button to simulate the circuit.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = inputSet[0];},
        1, 1));
    levels.append(Level("Now for something slightly more complex, logic gates can be added using the buttons on the right. Click to pick up a gate, and click again to set it down.\n"
        "In this level, the output should only be ON when BOTH inputs are ON."
        " This precisely matches the behavior of an AND gate, which only outputs ON (on the right side of the gate) when both of its inputs are ON (on the left side of the gate).",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = inputSet[0] && inputSet[1];},
        2, 1));
    levels.append(Level("Next the OR gate; an OR gate outputs ON when EITHER OR BOTH of its inputs are ON. ",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = inputSet[0] || inputSet[1];},
        2, 1));
    levels.append(Level("Lastly, the NOT gate. A NOT gate inverts the incoming signal: if its input is ON, it outputs OFF; if its input is OFF, it outputs ON.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = !inputSet[0];},
        1, 1));
    levels.append(Level("Unfortunately, some logic can only be accomplished with a combination of multiple gates.\nNotice that the outputs here are the opposite of "
        "a gate we've seen before.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = !(inputSet[0] || inputSet[1]);},
        2, 1));
    levels.append(Level("To add more complexity, now we have a third input port! Note how in this circuit, the OUTPUT is alway ON when eithe rof the first two INPUTs are on, "
        "but is always OFF if the third INPUT is ON.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = (inputSet[0] || inputSet[1]) && !inputSet[2];},
        3, 1));
    levels.append(Level("With digital logic, there is never just one way of solving a problem. The INPUT and OUTPUT sequence here matches an AND gate, but can be made without"
        "using an AND gate at all. Can you figure out how?",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = inputSet[0] && inputSet[1];},
        2, 1));
    levels.append(Level("Logic gates themselves can even be more complex than just these basic 3. For example, this sequence of OUTPUTS matches and XOR gate. An XOR is similar"
        "to an OR gate, but only outputs ON if either INPUT is ON, not BOTH. Hence the name XOR, standing for Exclusive Or.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = (inputSet[0] || inputSet[1]) && !(inputSet[0] && inputSet[1]);},
        2, 1));
    levels.append(Level("Another example of a compound gate is the NAND gate. NAND gates are the inverse of AND gates, outputting OFF only when BOTH inputs are ON.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = !(inputSet[0] && inputSet[1]);},
        2, 1));
    levels.append(Level("Another basic digital component is the MUX, or multiplexer. This circuit chooses between two INPUTs using the third INPUT. If the third input is OFF, "
        "the first INPUT is used as the OUTPUT, whereas if the third INPUT is ON, the the second INPUT routed to the OUTPUT instead.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){
            if (inputSet[2])
                outputSet[0] = inputSet[1];
            else
                outputSet[0] = inputSet[0];
        },
        3, 1));
    levels.append(Level("Here, this INPUT to OUTPUT mapping matches that of a 1-bit adder.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){
            outputSet[0] = false;
            outputSet[1] = false;
        },
        3, 2));
    return levels;
}
