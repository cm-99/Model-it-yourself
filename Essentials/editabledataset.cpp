#include "editabledataset.h"
#include "qdebug.h"

EditableDataset::EditableDataset(){}

EditableDataset::~EditableDataset()
{
    if(data_columns != nullptr)
    {
        qDeleteAll(data_columns->begin(), data_columns->end());
        delete data_columns;
    }

    if(timestamps_column != nullptr)
        delete timestamps_column;
}

void EditableDataset::prepare_dataset_container(int columns, int rows, bool timestamps_present)
{
    //Columns - how many vectors should be prepared in data_columns
    //Columns_count - how many columns there are in reality - add timestamps_column if it will be present
    int offset_for_timestamps_column = 1*timestamps_present;
    this -> columns_count = columns + offset_for_timestamps_column;
    this -> rows_count = rows;

    data_columns = new QVector<QVector<double>*>;
    for(int i = 0; i < columns; i++)
    {
        QVector<double> *new_column_container = new QVector<double>;
        new_column_container -> reserve(rows);
        data_columns -> append(new_column_container);
    }

    if(!timestamps_present)
        return;

    this -> timestamps_present = true;
    timestamps_column = new QVector<QString>;
    timestamps_column -> reserve(rows);

    signals_info.insert(0, SignalInfo("Timestamp"));
    signals_info.removeLast();
}

bool EditableDataset::get_timestamps_present() const
{
    return timestamps_present;
}

int EditableDataset::get_rows_count() const
{
    return rows_count;
}

int EditableDataset::get_columns_count() const
{
    return columns_count;
}

double EditableDataset::get_sampling_time_in_seconds() const
{
    return sampling_time_in_seconds;
}

QString EditableDataset::get_dataset_name() const
{
    return dataset_name;
}

SignalInfo EditableDataset::get_signal_info(int signal_index) const
{
    return signals_info.at(signal_index);
}

double EditableDataset::get_sample_value(int signal_index, int row_index)
{
    return data_columns->at(signal_index)->at(row_index);
}

QString EditableDataset::get_timestamp(int row_index)
{
    return timestamps_column->at(row_index);
}

void EditableDataset::set_sampling_time_in_seconds(double sampling_time_in_seconds)
{
    this -> sampling_time_in_seconds = sampling_time_in_seconds;
}

void EditableDataset::set_dataset_name(QString dataset_name)
{
    this -> dataset_name = dataset_name;
}

void EditableDataset::append_sample(int signal_index, double sample_value)
{
    data_columns -> at(signal_index) -> append(sample_value);
}

void EditableDataset::replace_sample(int signal_index, int sample_index, double new_sample_value)
{
    //Signal_index =/= column_index. Signal_index is considered in the context of data_columns only
    //TODO: If will be slower with all those boundary checks, search for alternative (unsafe overload?), at least for tests
    if(signal_index >= data_columns->length())
        return;

    if(sample_index >= data_columns -> at(signal_index)->length())
        return;

    data_columns -> at(signal_index) -> replace(sample_index, new_sample_value);

}

void EditableDataset::append_timestamp(QString timestamp)
{
    timestamps_column -> append(timestamp);
}

void EditableDataset::append_signal_info(SignalInfo signal_info)
{
    signals_info.append(signal_info);
}

void EditableDataset::replace_signal_info(int signal_index, SignalInfo signal_info)
{
    signals_info.replace(signal_index, signal_info);
}

void EditableDataset::remove_columns(int first_column_to_remove, int columns_to_remove_count)
{
    qDebug()<<first_column_to_remove;
    qDebug()<<columns_to_remove_count;

    //Columns - data_columns + timestamps_column
    if(first_column_to_remove + columns_to_remove_count > this -> columns_count || columns_to_remove_count == 0)
        return;

    if(timestamps_present)
        remove_columns_from_dataset_with_timestamps(first_column_to_remove, columns_to_remove_count);
    else
        remove_columns_from_dataset_without_timestamps(first_column_to_remove, columns_to_remove_count);

    if(columns_count == 0)
        emit signal_all_data_removed();
    else
        emit signal_dataset_edited();
}

void EditableDataset::remove_rows(int first_row, int rows_count)
{
    for(int i = 0; i < data_columns->length(); i++)
    {
        data_columns -> at(i) -> remove(first_row, rows_count);
        data_columns -> at(i) -> squeeze();
    }

    if(timestamps_present)
    {
        timestamps_column -> remove(first_row, rows_count);
        timestamps_column -> squeeze();
    }

    this -> rows_count -= rows_count;

    if(columns_count == 0)
        emit signal_all_data_removed();
    else
        emit signal_dataset_edited();
}

void EditableDataset::calculate_signals_statistics()
{

}

void EditableDataset::remove_columns_from_dataset_with_timestamps(int first_column_to_remove, int columns_to_remove_count)
{
    int original_columns_to_remove_count = columns_to_remove_count;

    //Offset is needed if timestamps column is removed and then data columns are to be removed
    int offset = 0;
    if(first_column_to_remove == 0)
    {
        //Remove timestamps column
        timestamps_present = false;
        delete timestamps_column;

        columns_count--;
        columns_to_remove_count--;
        offset = 1;
    }

    if(columns_to_remove_count == 0)
    {
        signals_info.removeFirst();
        return;
    }

    //data_columns in EditableDataset with timestamps are shifted by one in relation to columns displayed by EditableDataset_TableModel
    QVector<QVector<double>*> columns_to_delete = data_columns -> mid(first_column_to_remove - 1 + offset, columns_to_remove_count);

    data_columns -> remove(first_column_to_remove - 1 + offset, columns_to_remove_count);
    data_columns -> squeeze();

    qDeleteAll(columns_to_delete.begin(), columns_to_delete.end());
    columns_to_delete.clear();

    //Remove signals correspoding to data_columns removed
    //Signals are not shifted!
    for(int i = 0; i < original_columns_to_remove_count; i++)
        signals_info.removeAt(first_column_to_remove);

    this -> columns_count -= columns_to_remove_count;
}

void EditableDataset::remove_columns_from_dataset_without_timestamps(int first_column_to_remove, int columns_to_remove_count)
{
    QVector<QVector<double>*> columns_to_delete = data_columns -> mid(first_column_to_remove, columns_to_remove_count);

    data_columns -> remove(first_column_to_remove, columns_to_remove_count);
    data_columns -> squeeze();

    qDeleteAll(columns_to_delete.begin(), columns_to_delete.end());
    columns_to_delete.clear();

    //Remove signals correspoding to data_columns removed
    for(int i = 0; i < columns_to_remove_count; i++)
        signals_info.removeAt(first_column_to_remove);

    this -> columns_count -= columns_to_remove_count;
}

void EditableDataset::slot_update_dataset_name(QString new_name)
{
    this -> dataset_name = new_name;
}

void EditableDataset::slot_update_dataset_sampling_time(double new_sampling_time)
{
    this -> sampling_time_in_seconds = new_sampling_time;
}

