#ifndef DATASETPLOTSPAGE_H
#define DATASETPLOTSPAGE_H

#include <QtCharts/QtCharts>
#include <QtWidgets/QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QCheckBox>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Essentials/readonlydataset.h"
#include "tabwidgetpage.h"

QT_CHARTS_USE_NAMESPACE

/**
 * Class creating and displaying plots of active dataset't signals.
 */
// TODO: Extract class responsible for creating plots.
// Make it inherent BackgroundTaskEnabledObject and create plots in background task because it can cause significant delays.
class DatasetPlotsPage : public TabWidgetPage
{
    Q_OBJECT
public:
    explicit DatasetPlotsPage(QWidget *parent = nullptr);
    ~DatasetPlotsPage();

    /**
     * Creates signals plots from provided dataset and relays this change to QChartView.
     * For each signal there is a checkbox created which can be used to show/hide corresponding plot.
     */
    void set_dataset(ReadOnlyDataset *dataset);

private:
    // Private attributes
    const ReadOnlyDataset *current_dataset = nullptr;
    /**
     * Attributes meant to hold informations about dataset from before the update.
     */
    bool old_dataset_timestamps_present = false;
    int old_dataset_rows_count = 0;

    QChart* chart = nullptr;
    QValueAxis* y_axis = nullptr;
    QValueAxis* x_axis = nullptr;

    QChartView chart_view;
    QHBoxLayout check_boxes_layout;
    QVBoxLayout main_layout;

    //// Private methods
    void clear_layout(QBoxLayout* layout_to_clear);

    // Extracted from slot_update_widgets
    void update_widgets_on_signals_removal(DatasetSection dataset_section_updated);
    void update_widgets_on_rows_removal(DatasetSection dataset_section_updated);

    // Extracted from set_dataset
    void create_plots_for_current_dataset();
    void create_visibility_check_boxes();

public slots:
    /**
     * Updates checkboxes and charts to match changes done to current_dataset.
     * To be used after the dataset has been modified.
     */
    void slot_update_widgets(DatasetSection dataset_section_updated);
    //TODO: Implement update on sampling time change. It will propably require calculating again all x-axis values for points in series.
    /**
     * Update checkboxes/chart names to match changes done to current_dataset.
     * To be used after the dataset information has been modified.
     */
    void slot_update_dataset_informations();
    /**
     * Restores object to default state (before set_dataset was called).
     * Should be called if all the rows/columns in dataset were removed by the user and thus dataset has become unusable.
     */
    void slot_restore_to_default();
    /**
     * Updates y_axis range when plots are shown/hidden through check boxes.
     */
    void slot_update_y_axis_range();
};

#endif // DATASETPLOTSPAGE_H
