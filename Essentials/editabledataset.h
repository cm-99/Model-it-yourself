#ifndef EDITABLEDATASET_H
#define EDITABLEDATASET_H

#include "readonlydataset.h"

/**
 * Main application data model class.
 * Provides methods for editing everything in loaded dataset.
 * Should only be available to selected and well-tested classes providing data preprocessing functionality.
 */
class EditableDataset: public ReadOnlyDataset
{
    Q_OBJECT
public:
    // Public methods
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

    //Setters for dataset informations
    void set_sampling_time_in_seconds(double sampling_time_in_seconds);
    void set_dataset_name(QString dataset_name);
    void set_min_signals_sample_value(double new_value);
    void set_max_signals_sample_value(double new_value);

    // Setters for dataset containers
    // TODO: Depending on what preprocessing plugins may be taken into account - add methods for containers values replacement
    // e.g. plugin removing noise from the data would propably need to replace data instead of appending it,
    // or maybe clearing containers and then recreating them would be faster? ... but then all the new data and old data
    // would need to be in memory at the same time, not ideal for bigger datasets.

    /**
     * It might look like a typical getter but it is returning pointer that can be used to modify SignalInfo at signal_index.
     * This is why it is not included in ReadOnlyDataset.
     */
    SignalInfo *get_signal_info_pointer(int signal_index);
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

    // Remove methods of dataset containers
    void remove_columns(int first_column_to_remove, int columns_to_remove_count);
    void remove_rows(int first_row, int rows_count);

public slots:
    void slot_update_dataset_name(QString new_name);
    void slot_update_dataset_sampling_time(double new_sampling_time);
    /**
     * Slot used to trigger a signal_dataset_information_edited.
     * Should be used when signal informations were changed through pointers instead of individual setter methods.
     */
    void slot_relay_signal_info_update();

private:
    // Some methods extracted from remove_columns for better code readability
    void remove_columns_from_dataset_with_timestamps(int first_column_to_remove, int columns_to_remove_count);
    void remove_columns_from_dataset_without_timestamps(int first_column_to_remove, int columns_to_remove_count);

signals:
    /**
     * Signal to be emitted each time a Dataset with already prepared containers is edited,
     * which means each time remove method of dataset containers is called.
     * DatasetSection contains boundaries of data removed. If first_row/column and corresponding rows/columns_count are equal to 0
     * it means that rows or columns were not removed at all.
     *
     * Should be connected by CoreController to slots updating informations about dataset's data in each object using it.
     */
    void signal_dataset_edited(DatasetSection dataset_section_edited);
    /**
     * Signal to be emitted each time a Dataset information was edited (sampling time, name, signals names...).
     *
     * Should be connected by CoreController to slots updating informations about dataset in each object using it.
     */
    void signal_dataset_information_edited();
    /**
     * Signal to be emitted if user removes all the data through dataset edition.
     *
     * Should be routed by CoreController to slots restoring objects to default states,
     * in case of objects heavily dependent on valid dataset being present (dataset tab pages).
     */
    void signal_all_data_removed();
};

#endif // EDITABLEDATASET_H
