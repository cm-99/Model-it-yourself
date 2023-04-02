#ifndef DATASET_TABLEMODEL_H
#define DATASET_TABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>
#include <QFont>
#include <QBrush>

#include "editabledataset.h"

/**
 * Currently the only supported dataset representation.
 */
class Dataset_TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Dataset_TableModel();
    Dataset_TableModel(EditableDataset *dataset);
    ~Dataset_TableModel();

    void set_dataset_represented(EditableDataset *dataset);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //Dataset edition
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    /**
     * Dataset being represented.
     */
    EditableDataset *dataset = nullptr;

    int dataset_rows_count = 0;
    int dataset_columns_count = 0;
    bool dataset_timestamps_present = false;
};

#endif // DATASET_TABLEMODEL_H
