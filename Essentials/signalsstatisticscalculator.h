#ifndef SIGNALSSTATISTICSCALCULATOR_H
#define SIGNALSSTATISTICSCALCULATOR_H

#include "Essentials/editabledataset.h"

#include <QObject>

//TODO: Think about changing this class to namespace instead if it will be used in more places
/**
 * As the name implies - this class is meant for signals statistics calculation.
 */
class SignalsStatisticsCalculator: public QObject
{
    Q_OBJECT
public:
    SignalsStatisticsCalculator();

public slots:
    void slot_calculate_signals_statistics(EditableDataset *dataset);

private:
    void calculate_min_and_max_signals_values(QVector<QVector<double>*>* dataset_container, EditableDataset *dataset);
};

#endif // SIGNALSSTATISTICSCALCULATOR_H
