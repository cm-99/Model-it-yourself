#include "datasetdetails.h"
#include <QSizePolicy>

void DatasetDetails::prepare_general_dataset_attributes_box()
{
    dataset_name_edit.setPlaceholderText("Dataset not loaded");
    dataset_sampling_time_spinbox.setDecimals(3);
    dataset_sampling_time_spinbox.setDisabled(true);
    dataset_name_edit.setDisabled(true);

    general_dataset_attributes_form_layout.addRow(tr("Name"), &dataset_name_edit);
    general_dataset_attributes_form_layout.addRow(tr("Signals count"), &dataset_signals_count_label);
    general_dataset_attributes_form_layout.addRow(tr("Rows count"), &dataset_rows_count_label);
    general_dataset_attributes_form_layout.addRow(tr("Sampling time [s]"), &dataset_sampling_time_spinbox);

    general_dataset_attributes_box.setTitle("General attributes");
    general_dataset_attributes_box.setLayout(&general_dataset_attributes_form_layout);
}

void DatasetDetails::prepare_signal_attributes_layout()
{
    // Grouped signal informations
    signals_attributes_box.setTitle("Signals details");
    signals_attributes_layout.addWidget(&signals_view);

    signals_view.setSelectionBehavior(QAbstractItemView::SelectRows);
    signals_view.setSelectionMode(QAbstractItemView::SingleSelection);

    signals_attributes_box.setLayout(&signals_attributes_layout);
    main_signals_attributes_layout.addWidget(&signals_attributes_box);

    // Individual signal informations editor - signal being edited should change depending on current row selection in signals_view
    individual_signals_attributes_box.setTitle("Selected signals editor");
    individual_signals_attributes_box.setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));

    signal_name_edit.setMaxLength(20);
    signal_unit_edit.setMaxLength(10);

    individual_signals_form_layout.addRow(tr("Name"), &signal_name_edit);
    individual_signals_form_layout.addRow(tr("Unit"), &signal_unit_edit);

    individual_signals_attributes_box.setLayout(&individual_signals_form_layout);
    main_signals_attributes_layout.addWidget(&individual_signals_attributes_box);
}

DatasetDetails::DatasetDetails(QWidget *parent)
    : TabWidgetPage{parent}
{
    prepare_general_dataset_attributes_box();
    prepare_signal_attributes_layout();

    main_layout.addWidget(&general_dataset_attributes_box);
    main_layout.addLayout(&main_signals_attributes_layout);

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

    if(signals_view.selectionModel() != nullptr)
    {
        disconnect(signals_view.selectionModel(), &QItemSelectionModel::selectionChanged, this,
                   &DatasetDetails::slot_on_selected_signal_changed);
    }

    signals_table_model = new DatasetSignals_TableModel(dataset);
    signals_view.setModel(signals_table_model);

    connect(signals_view.selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &DatasetDetails::slot_on_selected_signal_changed);

    dataset_name_edit.setDisabled(false);
    dataset_sampling_time_spinbox.setDisabled(false);
    slot_update_widgets();

    connect_editable_widgets_to_dataset();

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

void DatasetDetails::slot_on_selected_signal_changed()
{
    int selected_signals_index = signals_view.selectionModel()->selectedRows().first().row();
    int timestamps_offset = current_dataset -> get_timestamps_present();
    int selected_signals_index_in_signals_info_container = selected_signals_index + timestamps_offset;
    SignalInfo *current_signal_info = current_dataset -> get_signal_info_pointer(selected_signals_index_in_signals_info_container);

    signal_name_edit.disconnect();
    signal_unit_edit.disconnect();

    signal_name_edit.setText(current_signal_info->name);
    signal_unit_edit.setText(current_signal_info->unit);

    connect(&signal_name_edit, &QLineEdit::textEdited, this, [=](QString text)
    {
        current_dataset -> get_signal_info_pointer(selected_signals_index_in_signals_info_container)->name = text;
        // IMPORTANT: signal info is changed through a pointer, not a setter method and thus is not triggering edit/update signals,
        // to see the changes in DatasetPlotsPage a signal must be manually triggered.
        current_dataset -> slot_relay_signal_info_update();
    });

    connect(&signal_unit_edit, &QLineEdit::textEdited, this, [=](QString text)
    {
        current_dataset -> get_signal_info_pointer(selected_signals_index_in_signals_info_container)->unit= text;
        current_dataset -> slot_relay_signal_info_update();
    });
}

void DatasetDetails::connect_editable_widgets_to_dataset()
{
    connect(&dataset_name_edit, &QLineEdit::textEdited, this, [=](QString text)
    {
        // Direct connection to slot_update_dataset_name does not throw any error, but does not work either...
        current_dataset -> slot_update_dataset_name(text);
    });

    connect(&dataset_sampling_time_spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            current_dataset, &EditableDataset::slot_update_dataset_sampling_time);
}

void DatasetDetails::slot_update_widgets()
{
    dataset_name_edit.setText(current_dataset -> get_dataset_name());
    dataset_signals_count_label.setText(QString::number(current_dataset -> get_columns_count() - current_dataset -> get_timestamps_present()));
    dataset_rows_count_label.setText(QString::number(current_dataset -> get_rows_count()));
    dataset_sampling_time_spinbox.setValue(current_dataset->get_sampling_time_in_seconds());
    signals_table_model -> update_dataset_informations();
}
