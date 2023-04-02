#include "Essentials/dataset_tablemodel.h"

Dataset_TableModel::Dataset_TableModel(){}

Dataset_TableModel::Dataset_TableModel(EditableDataset *dataset)
{
    set_dataset_represented(dataset);
}

Dataset_TableModel::~Dataset_TableModel(){}

void Dataset_TableModel::set_dataset_represented(EditableDataset *dataset)
{
    if(dataset == nullptr)
        return;

    this -> dataset = dataset;
    this -> dataset_columns_count = dataset -> get_columns_count();
    this -> dataset_rows_count = dataset->get_rows_count();
    this -> dataset_timestamps_present = dataset->get_timestamps_present();
}

int Dataset_TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return dataset_rows_count;
}

int Dataset_TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return dataset_columns_count;
}

QVariant Dataset_TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= dataset_rows_count || index.column() >= dataset_columns_count || role != Qt::DisplayRole)
        return QVariant();

    if(dataset_timestamps_present)
    {
        if(index.column() == 0)
            return QVariant(dataset -> get_timestamp(index.row()));

        //If timestamps are present data_columns are shifted relative to index.column
        return QVariant(dataset -> get_sample_value(index.column() - 1, index.row()));
    }
    return QVariant(dataset -> get_sample_value(index.column(), index.row()));
}

QVariant Dataset_TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Vertical && section <= dataset_rows_count)
        return QVariant(section);

    if(orientation == Qt::Horizontal && section <= dataset_columns_count)
        return QVariant(dataset->get_signal_info(section).name);

    return QVariant();
}

bool Dataset_TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    dataset -> remove_rows(row, count);
    this -> dataset_rows_count = dataset -> get_rows_count();

    if(dataset_rows_count == 0)
        dataset_columns_count = 0;

    emit headerDataChanged(Qt::Orientation::Vertical, row, row + count - 1);
    endRemoveRows();

    return true;
}

bool Dataset_TableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column+count-1);

    dataset -> remove_columns(column, count);
    this -> dataset_columns_count = dataset -> get_columns_count();
    this -> dataset_timestamps_present = dataset -> get_timestamps_present();

    if(dataset_columns_count == 0)
        this -> dataset_rows_count = 0;

    emit headerDataChanged(Qt::Orientation::Horizontal, column, column + count - 1);
    endRemoveColumns();

    return true;
}
