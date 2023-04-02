#include "datasetdetails.h"
#include <QSizePolicy>

DatasetDetails::DatasetDetails(QWidget *parent)
    : TabWidgetPage{parent}
{
    //Prepare general dataset attributes in one group box
    dataset_name_edit.setPlaceholderText("Dataset not loaded");
    dataset_sampling_time_spinbox.setDecimals(3);
    dataset_sampling_time_spinbox.setDisabled(true);
    dataset_name_edit.setDisabled(true);

    general_dataset_attributes_form_layout.addRow(tr("Name"), &dataset_name_edit);
    general_dataset_attributes_form_layout.addRow(tr("Signals count"), &dataset_signals_count_label);
    general_dataset_attributes_form_layout.addRow(tr("Rows count"), &dataset_rows_count_label);
    general_dataset_attributes_form_layout.addRow(tr("Sampling time [s]"), &dataset_sampling_time_spinbox);

    general_dataset_attributes_box.setTitle("General attributes");
    main_layout.addWidget(&general_dataset_attributes_box);
    general_dataset_attributes_box.setLayout(&general_dataset_attributes_form_layout);

    //Prepare signal informations in the second group box
    signals_attributes_box.setTitle("Signals details");

    signals_attributes_layout.addWidget(&signals_view);
    signals_attributes_box.setLayout(&signals_attributes_layout);
    main_layout.addWidget(&signals_attributes_box);

    this -> setLayout(&main_layout);
}

DatasetDetails::~DatasetDetails()
{
    if(signals_table_model != nullptr)
        signals_table_model -> deleteLater();
}

void DatasetDetails::set_dataset(EditableDataset *dataset)
{
    auto *previous_signals_model = signals_table_model;
    current_dataset = dataset;

    signals_table_model = new DatasetSignals_TableModel(dataset);
    signals_view.setModel(signals_table_model);
    connect_editable_widgets_to_dataset();

    dataset_name_edit.setDisabled(false);
    dataset_sampling_time_spinbox.setDisabled(false);
    slot_update_widgets();

    if(previous_signals_model != nullptr)
        previous_signals_model->deleteLater();
}

void DatasetDetails::slot_restore_to_default()
{
    auto *previous_signals_model = signals_table_model;
    this -> current_dataset = nullptr;
    this -> signals_table_model = nullptr;

    signals_view.setModel(signals_table_model);
    dataset_name_edit.setDisabled(true);
    dataset_sampling_time_spinbox.setDisabled(true);

    dataset_rows_count_label.clear();
    dataset_name_edit.clear();
    dataset_name_edit.setPlaceholderText("Dataset not loaded");
    dataset_sampling_time_spinbox.clear();
    dataset_signals_count_label.clear();

    if(previous_signals_model != nullptr)
        previous_signals_model->deleteLater();
}

void DatasetDetails::connect_editable_widgets_to_dataset()
{
    connect(&dataset_name_edit, &QLineEdit::textEdited,
            current_dataset, &EditableDataset::slot_update_dataset_name);
    connect(&dataset_sampling_time_spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            current_dataset, &EditableDataset::slot_update_dataset_sampling_time);
}

void DatasetDetails::slot_update_widgets()
{
    dataset_name_edit.setText(current_dataset -> get_dataset_name());
    dataset_signals_count_label.setText(QString::number(current_dataset -> get_columns_count() - current_dataset -> get_timestamps_present()));
    dataset_rows_count_label.setText(QString::number(current_dataset -> get_rows_count()));
    dataset_sampling_time_spinbox.setValue(current_dataset->get_sampling_time_in_seconds());
    signals_table_model->update_dataset_informations();
}
