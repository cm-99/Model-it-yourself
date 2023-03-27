#ifndef DATASETEDITORPAGE_H
#define DATASETEDITORPAGE_H

#include "Essentials/dataset_tablemodel.h"
#include "Essentials/GUI/GUIpages/tabwidgetpage.h"

#include <QTableView>
#include <QGridLayout>
#include <QPushButton>
#include <QDialogButtonBox>

/**
 * Class providing dataset viewing and editing functionality.
 */
class DatasetEditorPage : public TabWidgetPage
{
    Q_OBJECT
public:
    DatasetEditorPage(QWidget *parent = nullptr);
    ~DatasetEditorPage();

    /**
     * Relays model change request to dataset_view.
     */
    void set_model(Dataset_TableModel *dataset_model);

private:
    //GUI attributes
    QVBoxLayout *main_layout = nullptr;
    QHBoxLayout *buttons_layout = nullptr;
    QTableView *dataset_view = nullptr;

    QPushButton *button_remove_selection = nullptr;
    QPushButton *button_remove_in_range = nullptr;

    //Dataset edition methods

    /**
     * Removes selected rows. Works both with continous and non-continous selections.
     */
    void remove_selected_rows();
    void remove_selected_columns();

    /**
     * Divides selected rows into sections usable for calling removeRows.
     * @return - returns list of int pairs representing sections - starting row index and rows count.
     */
    QList<QPair<int, int>> divide_rows_selection_into_sections(QModelIndexList selection);
    QList<QPair<int, int>> divide_columns_selection_into_sections(QModelIndexList selection);

    void prepare_dialog_for_data_edition();

private slots:
    /**
     * Removes current selection - works both for rows and columns.
     */
    void slot_remove_selection();
    /**
     * Shows a dialog to user which allows to choose a range of rows and columns to remove.
     */
    void slot_remove_in_range();
    /**
     * If current selection behavior is selectColumns, does nothing.
     * Otherwise, changes selection behavior to selectColumns.
     */
    void slot_horizontal_header_clicked(int column_index);
    /**
     * If current selection behavior is selectRows, does nothing.
     * Otherwise, changes selection behavior to selectRows.
     */
    void slot_vertical_header_clicked(int row_index);
};

#endif // DATASETEDITORPAGE_H
