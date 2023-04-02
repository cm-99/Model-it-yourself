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
     * Creates Dataset_TableModel from provided dataset and relays model change to QTableView.
     * Enables use of remove buttons.
     */
    void set_dataset(EditableDataset *dataset);

private:
    Dataset_TableModel *dataset_model = nullptr;

    //GUI attributes
    QVBoxLayout main_layout;
    QHBoxLayout buttons_layout;
    QTableView dataset_view;

    QPushButton button_remove_selection;
    QPushButton button_remove_in_range;

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

public slots:
    /**
     * Restores object to default state with buttons disabled and Dataset_TableModel removed.
     * Should be called if all the rows/columns in dataset were removed by the user.
     */
    void slot_restore_to_default();

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
