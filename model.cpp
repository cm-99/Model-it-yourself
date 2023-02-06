#include "model.h"

Model::Model(const QStringList &rows, QObject *parent)
{
    this -> data_rows = rows;
    this -> columns_count = data_rows.first().split(',').size();
    this -> rows_count = data_rows.size();
}

int Model::rowCount(const QModelIndex &/*parent*/) const
{
    return data_rows.size();
}

int Model::columnCount(const QModelIndex &/*parent*/) const
{
    return this ->columns_count;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= rows_count)
        return QVariant();

    if(role == Qt::DisplayRole)
        return data_rows.at(index.row());
    else
        return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}
