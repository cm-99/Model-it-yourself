#ifndef DATASETDETAILS_H
#define DATASETDETAILS_H

#include "Essentials/datasetsignals_tablemodel.h"
#include "Essentials/editabledataset.h"
#include "tabwidgetpage.h"

#include <QObject>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QTableView>

/**
 * Class complementary to DatasetEditor, allows viewing and editing additional dataset informations and signals details.
 */
class DatasetDetails : public TabWidgetPage
{
    Q_OBJECT
public:
    DatasetDetails(QWidget *parent = nullptr);
    ~DatasetDetails();

    //TODO: Subclass TabWidgetPage as DatasetTabWidgetPage and extract those two methods from DatasetDetails and DatasetEditorPage
    /**
     * Sets dataset represented, creates DatasetSignals_TableModel from it.
     * Relays model change to QTableView and updates all widgets displaying dataset informations.
     */
    void set_dataset(EditableDataset *dataset);

private:
    const EditableDataset *current_dataset = nullptr;
    DatasetSignals_TableModel *signals_table_model = nullptr;

    ////Private GUI attributes
    QVBoxLayout main_layout;

    //Attributes representing general dataset informations
    QGroupBox general_dataset_attributes_box;
    QFormLayout general_dataset_attributes_form_layout;

    QLineEdit dataset_name_edit;
    QLabel dataset_signals_count_label;
    QLabel dataset_rows_count_label;
    QDoubleSpinBox dataset_sampling_time_spinbox;

    //TODO:Coming next... attributes representing signals in detail
    QGroupBox signals_attributes_box;
    QHBoxLayout signals_attributes_layout;

    QTableView signals_view;

    ////Private methods
    /**
     * Connects widgets which are editable by the user with slots changing values of corresponding attributes of the current_dataset
     */
    void connect_editable_widgets_to_dataset();

public slots:
    /**
     * Updates texts/values displayed by widgets using attributes of current_dataset_model.
     * To be used after the dataset has been set/changed/modified.
     */
    void slot_update_widgets();
    /**
     * Restores object to default state with edition widgets disabled and DatasetSignals_TableModel removed.
     * Should be called if all the rows/columns in dataset were removed by the user and thus dataset has become unusable.
     */
    void slot_restore_to_default();
};

#endif // DATASETDETAILS_H
