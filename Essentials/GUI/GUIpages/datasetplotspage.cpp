#include "datasetplotspage.h"

QT_CHARTS_USE_NAMESPACE

DatasetPlotsPage::DatasetPlotsPage(QWidget *parent)
    : TabWidgetPage{parent}
{
    // Prepare empty page - create chart view and layouts, add them to main layout
    check_boxes_layout.setAlignment(Qt::AlignCenter);
    main_layout.addLayout(&check_boxes_layout);
    main_layout.addWidget(&chart_view);
    this -> setLayout(&main_layout);
}

DatasetPlotsPage::~DatasetPlotsPage()
{
    clear_layout(&check_boxes_layout);
    chart -> deleteLater();
}

void DatasetPlotsPage::clear_layout(QBoxLayout *layout_to_clear)
{
    while(QLayoutItem *item = layout_to_clear -> takeAt(0))
    {
        if(QWidget* widget = item -> widget())
        {
            widget -> deleteLater();
        }
    }
}

void DatasetPlotsPage::update_widgets_on_signals_removal(DatasetSection dataset_section_updated)
{
    int first_signal_removed = dataset_section_updated.first_column;
    bool timestamps_currently_present = current_dataset -> get_timestamps_present(); //If timestamps are still present after update
    int series_to_remove_count = dataset_section_updated.columns_count;

    bool timestamps_were_removed = old_dataset_timestamps_present == timestamps_currently_present ? false : true;
    // Now this attribute can be updated
    old_dataset_timestamps_present = current_dataset -> get_timestamps_present();

    if(timestamps_were_removed)
    {
        series_to_remove_count--;
    }

    int timestamps_offset = timestamps_currently_present;
    // Remove invalid series and corresponding check boxes
    for(int i = 0; i < series_to_remove_count; i++)
    {
        QAbstractSeries *series_to_remove = chart -> series().at(first_signal_removed - timestamps_offset);
        chart -> removeSeries(series_to_remove);
        series_to_remove -> deleteLater();

        check_boxes_layout.takeAt(first_signal_removed - timestamps_offset) -> widget() -> deleteLater();
    }

    // Reconnect remaining check boxes to valid series
    int check_boxes_left_count = check_boxes_layout.count();
    for(int i = 0; i < check_boxes_left_count; i++)
    {
        QLayoutItem *item = check_boxes_layout.itemAt(i);
        if(QWidget* widget = item -> widget())
        {
            QCheckBox *visibility_check_box = dynamic_cast<QCheckBox*>(widget);
            visibility_check_box -> disconnect();
            connect(visibility_check_box, &QCheckBox::stateChanged, this, [=](int state)
            {
                chart -> series().at(i)->setVisible(state == Qt::Checked);
                slot_update_y_axis_range();
            });
        }
    }
    slot_update_y_axis_range();
}

void DatasetPlotsPage::update_widgets_on_rows_removal(DatasetSection dataset_section_updated)
{
    // Rows range removed from first to last would trigger slot_restore_to_default, not this slot.
    // If rows range was removed from first or to last then x-axis can really be adjusted to a new range
    // otherwise it should not be changed, because timeline would not make any sense. Just remove data points.

    bool rows_range_removed_from_first = dataset_section_updated.first_row == 0 ? true : false;
    bool rows_range_removed_to_last = dataset_section_updated.first_row + dataset_section_updated.rows_count ==
            old_dataset_rows_count ? true : false;

    //Now this can be updated
    old_dataset_rows_count = current_dataset -> get_rows_count();

    // Update x-axis
    // TODO: If data sampling time changes from DatasetDetails then all the points in series need to be changed... but for now
    int possible_data_sampling_time = current_dataset -> get_sampling_time_in_seconds();
    int x_axis_increment =  possible_data_sampling_time != 0 ? possible_data_sampling_time : 1;

    if(rows_range_removed_from_first)
    {
        x_axis -> setRange(x_axis -> min() + x_axis_increment*dataset_section_updated.rows_count, x_axis -> max());
    }
    else if(rows_range_removed_to_last)
    {
        x_axis -> setRange(x_axis -> min(), x_axis -> max() - x_axis_increment*dataset_section_updated.rows_count);
    }

    // Update series
    QList<QAbstractSeries*> series_displayed = chart -> series();

    for (int i = 0; i < series_displayed.length(); i++)
    {
        QXYSeries *xy_series = qobject_cast<QXYSeries*>(series_displayed.at(i));
        if(xy_series != nullptr)
        {
            xy_series -> removePoints(dataset_section_updated.first_row, dataset_section_updated.rows_count);
        }
    }
}

void DatasetPlotsPage::slot_update_widgets(DatasetSection dataset_section_updated)
{
    //Signals removed == invalid series displayed
    if(dataset_section_updated.columns_count != 0)
    {
        update_widgets_on_signals_removal(dataset_section_updated);
    }

    // Rows removed == invalid points in series displayed
    if(dataset_section_updated.rows_count != 0)
    {
        update_widgets_on_rows_removal(dataset_section_updated);
    }

    //Update y_axis, minimum and maximum values could have changed and statistics should have been calculated first after dataset update
    y_axis -> setRange(current_dataset -> get_min_signals_sample_value(), current_dataset -> get_max_signals_sample_value());
    chart_view.update();
}

void DatasetPlotsPage::slot_update_dataset_informations()
{
    chart -> setTitle(current_dataset -> get_dataset_name());

    QList<QCheckBox*> check_boxes_corresponding_to_signals = this -> findChildren<QCheckBox*>();
    int timestamps_offset = current_dataset -> get_timestamps_present();

    for(int i = 0; i < check_boxes_corresponding_to_signals.length(); i++)
    {
        check_boxes_corresponding_to_signals.at(i)->setText(current_dataset->get_signal_info(i + timestamps_offset).name);
    }
}

void DatasetPlotsPage::slot_restore_to_default()
{
    QChart *previous_chart = chart;
    this -> current_dataset = nullptr;
    this -> chart = new QChart();

    chart_view.setChart(chart);
    clear_layout(&check_boxes_layout);

    if(previous_chart != nullptr)
        previous_chart -> deleteLater();
}

void DatasetPlotsPage::slot_update_y_axis_range()
{
    QVector<double> min_max_values;

    int timestamps_offset = current_dataset -> get_timestamps_present();
    int dataset_signals_count = current_dataset -> get_columns_count() - timestamps_offset;

    QList<QCheckBox*> check_boxes_corresponding_to_signals = this -> findChildren<QCheckBox*>();
    for(int i = 0; i < dataset_signals_count; i++)
    {
        //Append to min_max_values vector only values corresponding to currently displayed signals
        if(check_boxes_corresponding_to_signals[i] -> isChecked())
        {
            SignalInfo temp_signal_info = current_dataset -> get_signal_info(i + timestamps_offset);
            min_max_values.append(temp_signal_info.min_value);
            min_max_values.append(temp_signal_info.max_value);
        }
    }

    std::pair<double*, double*> new_y_axis_range = std::minmax_element(min_max_values.begin(), min_max_values.end());
    y_axis -> setRange(*new_y_axis_range.first, *new_y_axis_range.second);
}

void DatasetPlotsPage::create_plots_for_current_dataset()
{
    auto dataset_signals_container = current_dataset -> get_dataset_signals_container();
    int possible_data_sampling_time = current_dataset -> get_sampling_time_in_seconds();
    int x_axis_increment =  possible_data_sampling_time != 0 ? possible_data_sampling_time : 1;

    // Create series for each signal in dataset (excluding timestamp)
    for (int i = 0; i < dataset_signals_container -> length(); i++)
    {
        QLineSeries* series = new QLineSeries(this);
        series->setUseOpenGL(true);

        for (int j = 0; j < dataset_signals_container -> at(i) -> length(); j++)
        {
            series -> append(QPointF(j*x_axis_increment, dataset_signals_container -> at(i) -> at(j)));
        }

        chart->addSeries(series);
        series -> attachAxis(x_axis);
        series -> attachAxis(y_axis);
    }
}

void DatasetPlotsPage::create_visibility_check_boxes()
{
    // Create checkboxes for showing/hiding individual plots
    int timestamps_offset = current_dataset -> get_timestamps_present();
    int dataset_signals_count = current_dataset -> get_columns_count() - timestamps_offset;
    for (int i = 0; i < dataset_signals_count; i++)
    {
        QCheckBox* check_box = new QCheckBox(current_dataset -> get_signal_info(i + timestamps_offset).name, this);
        check_box -> setChecked(true);
        connect(check_box, &QCheckBox::stateChanged, this, [=](int state)
        {
            chart -> series().at(i)->setVisible(state == Qt::Checked);
            slot_update_y_axis_range();
        });
        check_boxes_layout.addWidget(check_box);
    }
}

void DatasetPlotsPage::set_dataset(ReadOnlyDataset *dataset)
{
    this -> current_dataset = dataset;
    this -> old_dataset_timestamps_present = dataset -> get_timestamps_present();
    this -> old_dataset_rows_count = dataset -> get_rows_count();

    QChart *previous_chart = chart;
    chart = new QChart();

    // Clear check_boxes_layout
    clear_layout(&check_boxes_layout);

    // Set chart title
    chart -> setTitle(current_dataset -> get_dataset_name());
    chart -> legend() -> hide();

    // Create y-axis, set title and range
    y_axis = new QValueAxis();

    y_axis -> setRange(current_dataset -> get_min_signals_sample_value(), current_dataset -> get_max_signals_sample_value());
    y_axis -> setTitleText("Signals sample value");
    chart -> addAxis(y_axis, Qt::AlignLeft);

    // Create x-axis, set title and range
    // Why check sampling time and not timestamps_present?
    // Because dataset can be without timestamps but user can still know the sampling time and want to use it.
    int possible_data_sampling_time = current_dataset -> get_sampling_time_in_seconds();

    int x_axis_increment =  possible_data_sampling_time != 0 ? possible_data_sampling_time : 1;
    int dataset_rows_count = dataset -> get_rows_count();
    int max_x_value = x_axis_increment*dataset_rows_count;

    x_axis = new QValueAxis();
    x_axis -> setRange(0, max_x_value - x_axis_increment);
    QString x_axis_title_text = possible_data_sampling_time != 0 ? "Time from first timestamp [s]" : "Row index";
    x_axis -> setTitleText(x_axis_title_text);
    chart -> addAxis(x_axis, Qt::AlignBottom);

    create_plots_for_current_dataset();
    create_visibility_check_boxes();

    //Change displayed chart, delete previous
    chart_view.setChart(chart);
    if(previous_chart != nullptr)
    {
        previous_chart -> deleteLater();
    }
}
