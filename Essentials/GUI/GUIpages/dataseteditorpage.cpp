#include "Essentials/GUI/GUIpages/DatasetEditorPage.h"

#include <QDialog>
#include <QCheckBox>
#include <QHeaderView>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>

DatasetEditorPage::DatasetEditorPage(QWidget *parent)
    : TabWidgetPage{parent}
{
    // Set buttons parameters and connect them to corresponding slots
    button_remove_selection.setText("Remove selection");
    button_remove_selection.setDisabled(true);
    button_remove_in_range.setText("Remove in range");
    button_remove_in_range.setDisabled(true);

    connect(&button_remove_selection, &QPushButton::clicked, this, &DatasetEditorPage::slot_remove_selection);
    connect(&button_remove_in_range, &QPushButton::clicked, this, &DatasetEditorPage::slot_remove_in_range);

    // Prepare dataset_view settings
    dataset_view.setStyleSheet("QHeaderView::section { background-color:grey } QTableCornerButton::section { background-color:grey }");
    dataset_view.setSelectionBehavior(QAbstractItemView::SelectRows);
    dataset_view.setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Connect changing of selection behavior depending on user action (vertical/horizontal header pressed)
    connect(dataset_view.horizontalHeader(), &QHeaderView::sectionClicked, this, &DatasetEditorPage::slot_horizontal_header_clicked);
    connect(dataset_view.verticalHeader(), &QHeaderView::sectionClicked, this, &DatasetEditorPage::slot_vertical_header_clicked);

    // Add widgets to layout
    this->setLayout(&main_layout);
    main_layout.addWidget(&dataset_view);
    main_layout.addLayout(&buttons_layout);
    buttons_layout.addWidget(&button_remove_selection);
    buttons_layout.addWidget(&button_remove_in_range);
}

DatasetEditorPage::~DatasetEditorPage()
{
    if(dataset_model != nullptr)
        dataset_model -> deleteLater();
}

void DatasetEditorPage::set_dataset(EditableDataset *dataset)
{
    auto *previous_dataset_model = dataset_model;
    dataset_model = new Dataset_TableModel(dataset);

    dataset_view.setModel(dataset_model);
    button_remove_in_range.setDisabled(false);
    button_remove_selection.setDisabled(false);

    if(previous_dataset_model != nullptr)
        previous_dataset_model->deleteLater();
}

void DatasetEditorPage::slot_restore_to_default()
{
    Dataset_TableModel *previous_dataset_model = dataset_model;
    dataset_model = new Dataset_TableModel();

    dataset_view.setModel(dataset_model);
    button_remove_in_range.setDisabled(true);
    button_remove_selection.setDisabled(true);

    if(previous_dataset_model != nullptr)
        previous_dataset_model -> deleteLater();
}

QList<QPair<int, int>> DatasetEditorPage::divide_rows_selection_into_sections(QModelIndexList &selection)
{
    int new_selection_first_row_index = selection.first().row();
    int new_selection_last_row_index;
    QList<QPair<int, int>> list_of_selections_to_remove;

    for(int i = 0; i < selection.length() - 1; i++)
    {
        if(selection[i].row() + 1 != selection[i + 1].row())
        {
            new_selection_last_row_index = selection[i].row();
            int rows_count = new_selection_last_row_index - new_selection_first_row_index + 1;
            list_of_selections_to_remove.append(QPair<int, int>(new_selection_first_row_index, rows_count));

            new_selection_first_row_index = selection[i + 1].row();
        }
    }

    // Append last selection to list
    new_selection_last_row_index = selection.last().row();
    int rows_count = new_selection_last_row_index - new_selection_first_row_index + 1;
    list_of_selections_to_remove.append(QPair<int, int>(new_selection_first_row_index, rows_count));

    return list_of_selections_to_remove;
}

QList<QPair<int, int> > DatasetEditorPage::divide_columns_selection_into_sections(QModelIndexList &selection)
{
    int new_selection_first_column_index = selection.first().column();
    int new_selection_last_column_index;
    QList<QPair<int, int>> list_of_selections_to_remove;

    for(int i = 0; i < selection.length() - 1; i++)
    {
        if(selection[i].column() + 1 != selection[i + 1].column())
        {
            new_selection_last_column_index = selection[i].column();
            int columns_count = new_selection_last_column_index - new_selection_first_column_index + 1;
            list_of_selections_to_remove.append(QPair<int, int>(new_selection_first_column_index, columns_count));

            new_selection_first_column_index = selection[i + 1].column();
        }
    }

    // Append last selection to list
    new_selection_last_column_index = selection.last().column();
    int columns_count = new_selection_last_column_index - new_selection_first_column_index + 1;
    list_of_selections_to_remove.append(QPair<int, int>(new_selection_first_column_index, columns_count));

    return list_of_selections_to_remove;
}

void DatasetEditorPage::slot_remove_selection()
{
    if(dataset_model == nullptr)
        return;

    QString sender_name = "DatasetEditorPage";
    Log log;
    log.function_info = Q_FUNC_INFO;

    QModelIndexList selection = dataset_view.selectionModel() -> selectedIndexes();

    if(selection.isEmpty())
    {
        log.message = "Selection removal for an empty selection was called";
        emit signal_relay_log_message(log, sender_name);
        return;
    }

    if(dataset_view.selectionBehavior() == QAbstractItemView::SelectRows)
        remove_selected_rows();
    else
        remove_selected_columns();

    if(dataset_model->rowCount() == 0)
    {
        button_remove_in_range.setDisabled(true);
        button_remove_selection.setDisabled(true);
    }
}

void DatasetEditorPage::remove_selected_rows()
{
    QModelIndexList selection = dataset_view.selectionModel() -> selectedRows();
    std::sort(selection.begin(), selection.end(), std::less<QModelIndex>());

    QString sender_name = "DatasetEditorPage";
    Log log;
    log.function_info = Q_FUNC_INFO;
    QString temp_log_message;

    QList<QPair<int, int>> list_of_selections_to_remove;

    if(selection.last().row() - selection.first().row() + 1 != selection.length())
    {
        // If it's a non-continuous selection it needs to be divided into sections - using QPair<first row index, count of rows>
        list_of_selections_to_remove = divide_rows_selection_into_sections(selection);
        temp_log_message = "Selection removal for non-continuous selection was called. Selections to remove: ";
    }
    else
    {
        list_of_selections_to_remove.append(QPair<int, int>(selection.first().row(), selection.length()));
        temp_log_message = "Selection removal was called. Selection to remove: ";
    }

    QString list_of_selections_as_string;
    // Remove selections starting from last
    for(int i = list_of_selections_to_remove.length() - 1; i >= 0; i--)
    {
        int first_row = list_of_selections_to_remove[i].first;
        int count = list_of_selections_to_remove[i].second;
        dataset_view.model() -> removeRows(first_row, count);
        list_of_selections_as_string += "(" + QString(first_row) + ", " + QString(count) + ")";
    }

    log.message = temp_log_message + list_of_selections_as_string;
    emit signal_relay_log_message(log, sender_name);
}

void DatasetEditorPage::remove_selected_columns()
{
    QModelIndexList selection = dataset_view.selectionModel() -> selectedColumns();
    std::sort(selection.begin(), selection.end(), std::less<QModelIndex>());

    QString sender_name = "DatasetEditorPage";
    Log log;
    log.function_info = Q_FUNC_INFO;
    QString temp_log_message;

    QList<QPair<int, int>> list_of_selections_to_remove;

    if(selection.last().column() - selection.first().column() + 1 != selection.length())
    {
        //If it's a non-continuous selection it needs to be divided into sections - using QPair<first column index, count of columns>
        list_of_selections_to_remove = divide_columns_selection_into_sections(selection);
        temp_log_message = "Selection removal for non-continuous selection was called. Selections to remove: ";
    }
    else
    {
        list_of_selections_to_remove.append(QPair<int, int>(selection.first().column(), selection.length()));
        temp_log_message = "Selection removal was called. Selection to remove: ";
    }

    QString list_of_selections_as_string;
    //Remove selections starting from last
    for(int i = list_of_selections_to_remove.length() - 1; i >= 0; i--)
    {
        int first_column = list_of_selections_to_remove[i].first;
        int count = list_of_selections_to_remove[i].second;
        dataset_view.model() -> removeColumns(first_column, count);
        list_of_selections_as_string += "(" + QString(first_column) + ", " + QString(count) + ")";
    }

    log.message = temp_log_message + list_of_selections_as_string;
    emit signal_relay_log_message(log, sender_name);
}

void DatasetEditorPage::slot_remove_in_range()
{
    if(dataset_model == nullptr)
        return;

    QDialog ranges_to_remove_dialog(this);
    ranges_to_remove_dialog.setModal(true);
    ranges_to_remove_dialog.setWindowTitle("Remove elements in range");

    QGridLayout dialog_layout(&ranges_to_remove_dialog);

    //Prepare layout widgets
    QLabel remove_rows_label("Remove rows"), remove_columns_label("Remove columns");
    QCheckBox remove_rows_checkbox, remove_columns_checkbox;
    QLabel first_row_to_remove_label("First row to remove"), last_row_to_remove_label("Last row to remove");
    QLabel first_column_to_remove_label("First column to remove"), last_column_to_remove_label("Last column to remove");
    QSpinBox first_row_to_remove_spinbox, last_row_to_remove_spinbox;
    QSpinBox first_column_to_remove_spinbox, last_column_to_remove_spinbox;

    first_row_to_remove_spinbox.setRange(0, dataset_model->rowCount());
    last_row_to_remove_spinbox.setRange(0, dataset_model->rowCount());
    first_column_to_remove_spinbox.setRange(0, dataset_model->columnCount());
    last_column_to_remove_spinbox.setRange(0, dataset_model->columnCount());

    QDialogButtonBox button_box(QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal, &ranges_to_remove_dialog));
    QObject::connect(&button_box, &QDialogButtonBox::accepted, &ranges_to_remove_dialog, &QDialog::accept);
    QObject::connect(&button_box, &QDialogButtonBox::rejected, &ranges_to_remove_dialog, &QDialog::reject);

    //Add all widgets
    dialog_layout.addWidget(&remove_rows_label, 0, 0, 1, 1), dialog_layout.addWidget(&remove_columns_label, 1, 0, 1, 1);
    dialog_layout.addWidget(&remove_rows_checkbox, 0, 1, 1, 1), dialog_layout.addWidget(&remove_columns_checkbox, 1, 1, 1, 1);
    dialog_layout.addWidget(&first_row_to_remove_label, 0, 2, 1, 1), dialog_layout.addWidget(&first_row_to_remove_spinbox, 0, 3, 1, 1);
    dialog_layout.addWidget(&last_row_to_remove_label, 0, 4, 1, 1), dialog_layout.addWidget(&last_row_to_remove_spinbox, 0, 5, 1, 1);
    dialog_layout.addWidget(&first_column_to_remove_label, 1, 2, 1, 1), dialog_layout.addWidget(&first_column_to_remove_spinbox, 1, 3, 1, 1);
    dialog_layout.addWidget(&last_column_to_remove_label, 1, 4, 1, 1), dialog_layout.addWidget(&last_column_to_remove_spinbox, 1, 5, 1, 1);
    dialog_layout.addWidget(&button_box, 2, 0);

    if(ranges_to_remove_dialog.exec() == QDialog::Accepted)
    {
        //Remove ranges of selected rows/columns
        if(remove_rows_checkbox.isChecked())
        {
            int real_first_row_to_remove_index = (last_row_to_remove_spinbox.value() < first_row_to_remove_spinbox.value()) ?
                       last_row_to_remove_spinbox.value() : first_row_to_remove_spinbox.value();

            int rows_to_remove_count = abs(last_row_to_remove_spinbox.value() - first_row_to_remove_spinbox.value()) + 1;
            dataset_view.model() -> removeRows(real_first_row_to_remove_index, rows_to_remove_count);
        }

        if(remove_columns_checkbox.isChecked())
        {
            int real_first_column_to_remove_index = (last_column_to_remove_spinbox.value() < first_column_to_remove_spinbox.value()) ?
                        last_column_to_remove_spinbox.value() : first_column_to_remove_spinbox.value();


            int columns_to_remove_count = abs(last_column_to_remove_spinbox.value() - first_column_to_remove_spinbox.value()) + 1;
            dataset_view.model() -> removeColumns(real_first_column_to_remove_index, columns_to_remove_count);
        }
    }

    if(dataset_model->rowCount() == 0)
    {
        button_remove_in_range.setDisabled(true);
        button_remove_selection.setDisabled(true);
    }
}

void DatasetEditorPage::slot_horizontal_header_clicked(int column_index)
{
    if(dataset_view.selectionBehavior() == QAbstractItemView::SelectColumns)
        return;

    dataset_view.setSelectionBehavior(QAbstractItemView::SelectColumns);
    dataset_view.selectColumn(column_index);
}

void DatasetEditorPage::slot_vertical_header_clicked(int row_index)
{
    if(dataset_view.selectionBehavior() == QAbstractItemView::SelectRows)
        return;

    dataset_view.setSelectionBehavior(QAbstractItemView::SelectRows);
    dataset_view.selectRow(row_index);
}
