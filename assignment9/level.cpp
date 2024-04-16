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
        expectedInputs.append(inputGroup);
    }
}

Level::Level(QString description, std::function<void(QVector<bool> , QVector<bool>&)> levelFunction, qint32 inputCount, qint32 outputCount)
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
        expectedInputs.append(inputGroup);
    }

    for (qint32 i = 0; i < numberOfInputSets; i++) //initialize outputs
    {
        expectedOutputs.append(QVector<bool>());
        for (int j = 0; j < outputCount; j++)
            expectedOutputs[i].append(false);
    }

    for (qint32 i = 0; i < numberOfInputSets; i++)
        levelFunction(expectedInputs[i], expectedOutputs[i]); //generate appropriate outputs
}

QString Level::getDescription(){
    return description;
}

QVector<bool> Level::getExpectedOutput(qint32 input){
    return expectedOutputs[input];
}
