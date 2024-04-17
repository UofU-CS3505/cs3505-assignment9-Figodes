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
    levels.append(Level("Basics: Welcome to the basics. Let's do something nice and easy. Try to make an output by using one and gate.",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = inputSet[0] && inputSet[1];},
        2, 1));
    levels.append(Level("or gate and not gate :)",
        [](QVector<bool> inputSet, QVector<bool>& outputSet){outputSet[0] = !(inputSet[0] || inputSet[1]);},
        2, 1));
    return levels;
}
