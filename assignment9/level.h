#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <QVector>

class Level
{
public:
    Level(QString description, QVector<QVector<bool>> expectedOutputs, qint32 inputCount, qint32 outputCount);
    qint32 inputCount;
    qint32 outputCount;
    QString getDescription();
    QVector<bool> getExpectedOutput(qint32 input);

private:
    QString description;
    QVector<QVector<bool>> expectedOutputs;
};

#endif // LEVEL_H
