#include "datasetsignals_tablemodel.h"

DatasetSignals_TableModel::DatasetSignals_TableModel(){}

DatasetSignals_TableModel::DatasetSignals_TableModel(EditableDataset *dataset)
{
    set_dataset_represented(dataset);
}

void DatasetSignals_TableModel::set_dataset_represented(EditableDataset *dataset)
{
    if(dataset == nullptr)
        return;

    this -> dataset = dataset;
    this -> dataset_timestamps_present = dataset -> get_timestamps_present();
    this -> dataset_signals_number = dataset -> get_columns_count() - dataset_timestamps_present;
    //WARNING: This may change, keep it up to date or TODO: implement automatic getter
    this -> signals_attributes_number = 4;
}

void DatasetSignals_TableModel::update_dataset_informations()
{
    this -> dataset_timestamps_present = dataset -> get_timestamps_present();
    this -> dataset_signals_number = dataset -> get_columns_count() - dataset_timestamps_present;

    //Refresh table model
    QModelIndex top_left = createIndex(0,0);
    QModelIndex bottom_right = createIndex(rowCount(), 0);

    emit dataChanged(top_left, bottom_right);
    emit headerDataChanged(Qt::Orientation::Vertical, 0, rowCount());
}

int DatasetSignals_TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return dataset_signals_number;
}

int DatasetSignals_TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return signals_attributes_number;
}

QVariant DatasetSignals_TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= dataset_signals_number || index.column() >= signals_attributes_number || role != Qt::DisplayRole)
        return QVariant();

    //Offset used not to display timestamp informations
    int timestamps_offset = dataset_timestamps_present;
    SignalInfo signal_info = dataset -> get_signal_info(index.row() + timestamps_offset);

    if(index.column() == 0)
        return QVariant(signal_info.name);
    else if(index.column() == 1)
        return QVariant(signal_info.unit);
    else if(index.column() == 2)
        return QVariant(signal_info.min_value);
    else if(index.column() == 3)
        return QVariant(signal_info.max_value);

    return QVariant();
}

QVariant DatasetSignals_TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Vertical && section <= dataset_signals_number)
        return QVariant(section);

    if(orientation == Qt::Horizontal && section <= signals_attributes_number)
    {
        if(section == 0)
            return QVariant("Name");
        else if(section == 1)
            return QVariant("Unit");
        else if(section == 2)
            return QVariant("Min value");
        else if(section == 3)
            return QVariant("Max value");
        //To be extended
    }

    return QVariant();
}
