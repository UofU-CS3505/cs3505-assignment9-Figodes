#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <QVector>

class Level
{
public:
    Level(QString description, QVector<QVector<bool>> expectedOutputs, qint32 inputCount, qint32 outputCount);
    Level(QString description, std::function<void(QVector<bool> , QVector<bool>&)> levelFunction, qint32 inputCount, qint32 outputCount);
    qint32 inputCount;
    qint32 outputCount;
    QString getDescription();
    QVector<bool> getExpectedOutput(qint32 inputIndex);
    QVector<bool> getExpectedInput(qint32 inputIndex);

private:
    QString description;
    QVector<QVector<bool>> expectedOutputs;
    QVector<QVector<bool>> expectedInputs;
};

#endif // LEVEL_H
