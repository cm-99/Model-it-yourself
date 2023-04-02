#ifndef EDITABLEDATASET_H
#define EDITABLEDATASET_H

#include "readonlydataset.h"

/**
 * Main application data model class.
 * Provides methods for editing everything in loaded dataset.
 * Should only be available to selected classes providing data preprocessing functionality.
 */
class EditableDataset: public ReadOnlyDataset
{
    Q_OBJECT
public:
    //Public methods
    EditableDataset();
    ~EditableDataset() override;

    /**
     * Creates dataset container(s) - signals and timestamps containers - depending on:
     * @param: columns - how many columns should be prepared
     * @param: rows - how many rows should be prepared
     * @param: timestamps_present - if true, prepares timestamps container
     * Reserves memory for all the data.
     */
    void prepare_dataset_container(int columns, int rows, bool timestamps_present);

    //Getters for dataset informations
    bool get_timestamps_present() const;
    int get_rows_count() const;
    int get_columns_count() const;
    double get_sampling_time_in_seconds () const;
    QString get_dataset_name() const;

    ////Getters for dataset containers/values inside those
    //Container getters

    //Individual value getters
    SignalInfo get_signal_info(int signal_index) const;
    double get_sample_value(int signal_index, int row_index);
    QString get_timestamp(int row_index);

    //Setters for dataset informations
    void set_sampling_time_in_seconds(double sampling_time_in_seconds);
    void set_dataset_name(QString dataset_name);

    //Setters for dataset containers
    //TODO: Depending on what preprocessing plugins may be taken into account - add methods for containers values replacement
    //e.g. plugin removing noise from the data would propably need to replace data instead of appending it,
    //or maybe clearing containers and then recreating them would be faster? It would be... but then all the new data and old data
    //would need to be in memory at the same time, not ideal for bigger datasets.

    /**
     * Appends @param:sample_value to vector at @param:signal_index in data_columns.
     */
    void append_sample(int signal_index, double sample_value);
    void replace_sample(int signal_index, int sample_index, double new_sample_value);
    /**
     * Appends @param:timestamp to timestamps_column.
     */
    void append_timestamp(QString timestamp);
    void append_signal_info(SignalInfo signal_info);
    void replace_signal_info(int signal_index, SignalInfo signal_info);

    //Remove methods of dataset containers
    void remove_columns(int first_column_to_remove, int columns_to_remove_count);
    void remove_rows(int first_row, int rows_count);

    /**
     * Calculates statistics for signals in dataset and appends those informations to signals_info.
     *
     */
    void calculate_signals_statistics();

public slots:
    void slot_update_dataset_name(QString new_name);
    void slot_update_dataset_sampling_time(double new_sampling_time);

private:
    //Some methods extracted from remove_columns for better code readability
    void remove_columns_from_dataset_with_timestamps(int first_column_to_remove, int columns_to_remove_count);
    void remove_columns_from_dataset_without_timestamps(int first_column_to_remove, int columns_to_remove_count);

signals:
    //TODO: It could propably pass an argument (enum?) providing information about what was changed. To be considered.
    /**
     * Signal to be emitted each time a Dataset with already prepared containers is edited,
     * which means each time remove method of dataset containers is called.
     *
     * Should be connected by CoreController to slots updating informations about dataset in each object using it.
     */
    void signal_dataset_edited();
    /**
     * Signal to be emitted if user removes all the data through dataset edition.
     *
     * Should be routed by CoreController to slots restoring objects to default states,
     * in case of objects heavily dependent on valid dataset being present (dataset tab pages).
     */
    void signal_all_data_removed();

};

#endif // EDITABLEDATASET_H
