#include "signalsstatisticscalculator.h"
#include <algorithm>

SignalsStatisticsCalculator::SignalsStatisticsCalculator(){}

void SignalsStatisticsCalculator::slot_calculate_signals_statistics(EditableDataset *dataset)
{
    QVector<QVector<double>*>* dataset_container = dataset -> get_dataset_signals_container();
    int timestamps_offset = dataset -> get_timestamps_present();

    QVector<double> min_signals_values;
    QVector<double> max_signals_values;

    // Calculate min and max values for each signal
    for(int i = 0; i < dataset_container -> length(); i++)
    {
        std::pair<double*, double*> min_max_pair = std::minmax_element(dataset_container -> at(i) -> begin(),
                                                                       dataset_container -> at(i) -> end());
        double min_value = *min_max_pair.first;
        double max_value = *min_max_pair.second;

        SignalInfo *signal_info = dataset -> get_signal_info_pointer(i + timestamps_offset);
        signal_info -> min_value = min_value;
        signal_info -> max_value = max_value;

        //Searching for global min/max could proparbly be done here already
        min_signals_values.append(min_value);
        max_signals_values.append(max_value);
    }

    // Calculate global min and max values across signals
    double global_min_signals_value = *std::min_element(min_signals_values.begin(), min_signals_values.end());
    double global_max_signals_value = *std::max_element(max_signals_values.begin(), max_signals_values.end());

    dataset ->set_min_signals_sample_value(global_min_signals_value);
    dataset ->set_max_signals_sample_value(global_max_signals_value);
}
