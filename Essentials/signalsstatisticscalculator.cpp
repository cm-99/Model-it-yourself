#include "signalsstatisticscalculator.h"
#include <algorithm>

SignalsStatisticsCalculator::SignalsStatisticsCalculator(){}

void SignalsStatisticsCalculator::calculate_min_and_max_signals_values(QVector<QVector<double>*>* dataset_container, EditableDataset *dataset)
{
    int timestamps_offset = dataset -> get_timestamps_present();

    double current_global_min_value = dataset_container -> at(0) -> at(0);
    double current_global_max_value = dataset_container -> at(0) -> at(0);

    // Calculate min and max values for each signal, set them in corresponding SignalInfo
    for(int i = 0; i < dataset_container -> length(); i++)
    {
        std::pair<double*, double*> min_max_pair = std::minmax_element(dataset_container -> at(i) -> begin(),
                                                                       dataset_container -> at(i) -> end());
        double min_value = *min_max_pair.first;
        double max_value = *min_max_pair.second;

        SignalInfo *signal_info = dataset -> get_signal_info_pointer(i + timestamps_offset);
        signal_info -> min_value = min_value;
        signal_info -> max_value = max_value;

        // Calculate global (across all the signals) min and max values
        if(min_value < current_global_min_value)
        {
            current_global_min_value = min_value;
        }
        if(max_value > current_global_max_value)
        {
            current_global_max_value = max_value;
        }
    }

    // Set global min/max values
    dataset ->set_min_signals_sample_value(current_global_min_value);
    dataset ->set_max_signals_sample_value(current_global_max_value);
}

void SignalsStatisticsCalculator::slot_calculate_signals_statistics(EditableDataset *dataset)
{
    QVector<QVector<double>*>* dataset_container = dataset -> get_dataset_signals_container();
    if(dataset_container == nullptr)
    {
        return;
        if(dataset_container -> at(0) == nullptr)
        {
            return;
            if(dataset_container -> at(0)->isEmpty())
                return;
        }
    }

    calculate_min_and_max_signals_values(dataset_container, dataset);
}
