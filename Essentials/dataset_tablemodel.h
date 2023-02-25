#ifndef DATASET_TABLEMODEL_H
#define DATASET_TABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>
#include <QFont>
#include <QBrush>

/**
 * Default and currently only way of viewing dataset.
 */
class Dataset_TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Dataset_TableModel();
    ~Dataset_TableModel();

    //Methods required to be implemented by QAbstractTableModel

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //TODO: Secure it. It is exposed only for test purposes.

    /**
     * Creates dataset container(s) - data_columns - depending on @param:columns.
     * Reserves memory for @param:rows data samples inside the container.
     */
    void prepare_dataset_container(int columns, int rows);
    /**
     * Creates timestamps container - timestamps_column - and reserves memory for @param:rows timestamps.
     */
    void prepare_timestamps_container(int rows);
    /**
     * Dataset container.
     */
    QVector<QVector<double>*>* data_columns = nullptr;
    /**
     * Timestamps container.
     */
    QVector<QString>* timestamps_column = nullptr;

    //TODO: Add ability to set headers from user GUI

    /**
     * Dataset headers. Either found inside loaded file or set by the user through GUI.
     */
    QStringList headers;
    /**
     * Data sampling time in miliseconds - either automatically calculated from timestamps or [TODO: provided by user]
     */
    int sampling_time_in_miliseconds = 0;

private:
    int columns_count = 0;
    int rows_count = 0;
    bool timestamps_present = false;

};

#endif // DATASET_TABLEMODEL_H
