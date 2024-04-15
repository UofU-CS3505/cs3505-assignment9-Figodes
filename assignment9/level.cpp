#include "level.h"
#include <iostream>

Level::Level(QString description, QVector<QVector<bool>> expectedOutputs, qint32 inputCount, qint32 outputCount)
    : inputCount(inputCount)
    , outputCount(outputCount)
    , description(description)
    , expectedOutputs(expectedOutputs)
{

}

QString Level::getDescription(){
    return description;
}

QVector<bool> Level::getExpectedOutput(qint32 input){
    return expectedOutputs[input];
}
