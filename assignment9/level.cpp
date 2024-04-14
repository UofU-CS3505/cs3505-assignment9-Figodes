#include "level.h"
#include <iostream>

Level::Level(QString description, QVector<QVector<bool>> expectedOutputs) {
    this->description = description;
    this->expectedOutputs = expectedOutputs;
}

QString Level::getDescription(){
    return description;
}

QVector<bool> Level::getExpectedOutput(qint32 input){
    return expectedOutputs[input];
}
