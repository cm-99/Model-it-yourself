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

bool Dataset_TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);

    for(int i = 0; i < data_columns->length(); i++)
    {
        data_columns -> at(i) -> remove(row, count);
        data_columns -> at(i) -> squeeze();
    }

    if(timestamps_present)
    {
        timestamps_column -> remove(row, count);
        timestamps_column -> squeeze();
    }

    endRemoveRows();

    emit headerDataChanged(Qt::Orientation::Vertical, row, row + count - 1);

    return true;
}

bool Dataset_TableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column+count-1);

    if(timestamps_present)
        remove_columns_from_dataset_with_timestamps(column, count);
    else
        remove_columns_from_dataset_without_timestamps(column, count);

    endRemoveColumns();

    //Remove headers
    for(int i = count - 1; i >= 0; i--)
    {
        headers.removeAt(column + i);
    }

    emit headerDataChanged(Qt::Orientation::Horizontal, column, column + count - 1);

    return true;
}

void Dataset_TableModel::remove_columns_from_dataset_with_timestamps(int column, int count)
{
    //Offset is needed if timestamps column is removed and then data columns are to be removed
    int offset = 0;

    if(column == 0)
    {
        //Remove timestamps column
        timestamps_present = false;
        delete timestamps_column;

        columns_count--;
        count--;
        offset++;
    }

    if(count != 0)
    {
        //If timestamps are present vectors inside data_columns are shifted by one relative to column
        QVector<QVector<double>*> columns_to_delete = data_columns -> mid(column - 1 + offset, count);

        data_columns -> remove(column - 1 + offset, count);
        data_columns -> squeeze();

        qDeleteAll(columns_to_delete.begin(), columns_to_delete.end());
        columns_to_delete.clear();

        columns_count = columns_count - count;
    }
}

void Dataset_TableModel::remove_columns_from_dataset_without_timestamps(int column, int count)
{
    //// Delete part of the vector - TODO: extract it?
    QVector<QVector<double>*> columns_to_delete = data_columns -> mid(column, count);
    data_columns -> remove(column, count);
    data_columns -> squeeze();

    qDeleteAll(columns_to_delete.begin(), columns_to_delete.end());
    columns_to_delete.clear();
    ////

    columns_count = columns_count - count;
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
