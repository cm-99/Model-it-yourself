#include "readonlydataset.h"

ReadOnlyDataset::~ReadOnlyDataset()
{
    if(data_columns != nullptr)
    {
        qDeleteAll(data_columns->begin(), data_columns->end());
        delete data_columns;
    }

    if(timestamps_column != nullptr)
        delete timestamps_column;
}

bool ReadOnlyDataset::get_timestamps_present() const
{
    return timestamps_present;
}

int ReadOnlyDataset::get_rows_count() const
{
    return rows_count;
}

int ReadOnlyDataset::get_columns_count() const
{
    return columns_count;
}

double ReadOnlyDataset::get_sampling_time_in_seconds() const
{
    return sampling_time_in_seconds;
}

QString ReadOnlyDataset::get_dataset_name() const
{
    return dataset_name;
}

double ReadOnlyDataset::get_min_signals_sample_value() const
{
    return min_signals_sample_value;
}

double ReadOnlyDataset::get_max_signals_sample_value() const
{
    return max_signals_sample_value;
}

QVector<QString> *ReadOnlyDataset::get_dataset_timestamps_container() const
{
    return timestamps_column;
}

QVector<QVector<double> *> *ReadOnlyDataset::get_dataset_signals_container() const
{
    return data_columns;
}

SignalInfo ReadOnlyDataset::get_signal_info(int signal_index) const
{
    return signals_info.at(signal_index);
}

double ReadOnlyDataset::get_sample_value(int signal_index, int row_index)
{
    return data_columns->at(signal_index)->at(row_index);
}

QString ReadOnlyDataset::get_timestamp(int row_index)
{
    return timestamps_column->at(row_index);
}
