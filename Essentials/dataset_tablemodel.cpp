#include "Essentials/dataset_tablemodel.h"

Dataset_TableModel::Dataset_TableModel(){}

Dataset_TableModel::~Dataset_TableModel()
{
    if(data_columns != nullptr)
    {
        qDeleteAll(data_columns->begin(), data_columns->end());
        delete data_columns;
    }

    if(timestamps_column != nullptr)
        delete timestamps_column;
}

int Dataset_TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return rows_count;
}

int Dataset_TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return columns_count;
}

QVariant Dataset_TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= rows_count || index.column() >= columns_count || role != Qt::DisplayRole)
        return QVariant();

    if(timestamps_present)
    {
        if(index.column() == 0)
            return QVariant(timestamps_column -> at(index.row()));
        //If timestamps are present data_columns are shifted relative to index.column
        return QVariant(data_columns -> at(index.column() - 1) -> at(index.row()));
    }
    return QVariant(data_columns -> at(index.column()) -> at(index.row()));
}

QVariant Dataset_TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Vertical && section <= rows_count)
        return QVariant(section);

    if(orientation == Qt::Horizontal && section <= columns_count)
        return QVariant(headers.at(section));

    return QVariant();
}

void Dataset_TableModel::prepare_dataset_container(int columns, int rows)
{
    //Columns - how many vectors should be prepared in data_columns
    //Columns_count - how many columns there are in reality - add timestamps_column if it will be present

    int offset_for_timestamps_column = 1*timestamps_present;
    columns_count = columns + offset_for_timestamps_column;
    rows_count = rows;

    data_columns = new QVector<QVector<double>*>;
    for(int i = 0; i < columns; i++)
    {
        QVector<double> *new_column_container = new QVector<double>;
        new_column_container -> reserve(rows);
        data_columns -> append(new_column_container);
    }
}

void Dataset_TableModel::prepare_timestamps_container(int rows)
{
    timestamps_present = true;
    timestamps_column = new QVector<QString>;
    timestamps_column -> reserve(rows);
}
