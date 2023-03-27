#ifndef DATASET_TABLEMODEL_H
#define DATASET_TABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>
#include <QFont>
#include <QBrush>

/**
 * Currently the only supported dataset representation.
 */
class Dataset_TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Dataset_TableModel();
    ~Dataset_TableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //Dataset edition
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

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
     * Dataset headers. Found inside loaded file, set automatically or set by the user through dataset GUI.
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

    //Some methods extracted for the sake of better code readability
    void remove_columns_from_dataset_with_timestamps(int column, int count);
    void remove_columns_from_dataset_without_timestamps(int column, int count);
};

#endif // DATASET_TABLEMODEL_H
