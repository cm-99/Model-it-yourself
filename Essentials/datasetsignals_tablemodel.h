#ifndef DATASETSIGNALS_TABLEMODEL_H
#define DATASETSIGNALS_TABLEMODEL_H

#include "editabledataset.h"

#include <QAbstractTableModel>
#include <QObject>

/**
 * Dataset's signals representation.
 */
class DatasetSignals_TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DatasetSignals_TableModel();
    DatasetSignals_TableModel(EditableDataset *dataset);

    void set_dataset_represented(EditableDataset *dataset);
    void update_dataset_informations();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    /**
     * Dataset which signals are being represented.
     */
    EditableDataset *dataset = nullptr;

    int dataset_signals_number = 0;
    int signals_attributes_number = 0;
    bool dataset_timestamps_present = false;
};

#endif // DATASETSIGNALS_TABLEMODEL_H
