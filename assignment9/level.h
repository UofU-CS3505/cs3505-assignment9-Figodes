#ifndef LEVEL_H
#define LEVEL_H

#include <QString>
#include <QVector>

///\brief This class represents a level of the game,
/// containing its description and the correct outputs for each set of inputs.
class Level
{
public:
    Level(QString description, QVector<QVector<bool>> expectedOutputs, qint32 inputCount, qint32 outputCount);
    ///
    /// \brief Level Creates a level w/ the specified description and input/output counts.
    /// \param levelFunction A function that turns the inputs in the first argument into the outputs, which are put in the list in the second argument.
    /// This function operates on a single input set at a time, i.e. 011 in -> 10 out, not the list of input/output sets.
    /// This should never access elements at indexes >= inputCount in inputSet, or outputCount in outputSet.
    /// I recommend writing these as a series of statements that assign one output value per line based on inputs, like:
    /// { outputSet[0] = inputSet[0] && inputSet[1]; outputSet[1] = inputSet[0] || inputSet[1]; }
    /// rather than as a for-loop.
    ///
    Level(QString description, std::function<void(QVector<bool> inputSet, QVector<bool>& outputSet)> levelFunction, qint32 inputCount, qint32 outputCount);
    qint32 inputCount;
    qint32 outputCount;
    QString getDescription();
    ///
    /// \brief getExpectedOutput Gets the matching correct output for the level's input at the specified index.
    ///
    QVector<bool> getExpectedOutput(qint32 inputIndex);
    ///
    /// \brief getLevelInput Gets the level input set at the specified index.
    ///
    QVector<bool> getLevelInput(qint32 inputIndex);

    static QVector<Level> getLevelList();

private:
    QString description;
    QVector<QVector<bool>> expectedOutputs;
    QVector<QVector<bool>> levelInputs;
};

#endif // LEVEL_H
