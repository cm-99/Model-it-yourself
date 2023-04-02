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

void DatasetPlotsPage::set_dataset(ReadOnlyDataset *dataset)
{
    this -> current_dataset = dataset;
    QChart *previous_chart = chart;
    chart = new QChart();

    // Clear check_boxes_layout
    while(QLayoutItem *item = check_boxes_layout.takeAt(0))
    {
        if(QWidget* widget = item -> widget())
        {
            widget -> deleteLater();
        }
    }

    // Set axis labels
    chart -> setTitle(current_dataset -> get_dataset_name());
    chart -> legend() -> hide();

    // Create y-axis and set title
    QValueAxis* y_axis = new QValueAxis();
    y_axis -> setTitleText("Signals sample value");
    chart -> addAxis(y_axis, Qt::AlignLeft);

    //Create x-axis and set title
    auto dataset_signals_container = current_dataset -> get_dataset_signals_container();
    int possible_data_sampling_time = current_dataset -> get_sampling_time_in_seconds();

    int x_axis_increment =  possible_data_sampling_time != 0 ? possible_data_sampling_time : 1;
    int max_x_value = x_axis_increment *dataset_signals_container -> at(0) -> length();

    QValueAxis* x_axis = new QValueAxis();
    x_axis -> setRange(0, max_x_value);
    QString x_axis_title_text = possible_data_sampling_time != 0 ? "Time from first timestamp [s]" : "Index";
    x_axis -> setTitleText(x_axis_title_text);
    chart -> addAxis(x_axis, Qt::AlignBottom);

    // Create series for each signal in dataset (excluding timestamp)
    for (int i = 0; i < dataset_signals_container -> length(); i++)
    {
        QLineSeries* series = new QLineSeries(this);
        chart->addSeries(series);

        for (int j = 0; j < dataset_signals_container -> at(i) -> length(); j++)
        {
            series -> append(QPointF(j*x_axis_increment, dataset_signals_container -> at(i) -> at(j)));
        }

        series -> attachAxis(x_axis);
        series -> attachAxis(y_axis);
    }

    // Create checkboxes for showing/hiding individual plots
    int timestamps_offset = current_dataset -> get_timestamps_present();
    int dataset_signals_count = current_dataset -> get_columns_count() - timestamps_offset;

    for (int i = 0; i < dataset_signals_count; i++)
    {
        QCheckBox* check_box = new QCheckBox(current_dataset -> get_signal_info(i + timestamps_offset).name, this);
        check_box -> setChecked(true);
        connect(check_box, &QCheckBox::stateChanged, this, [=](int state)
        {
            chart->series().at(i)->setVisible(state == Qt::Checked);
        });
        check_boxes_layout.addWidget(check_box);
    }

    //Change displayed chart, delete previous
    chart_view.setChart(chart);
    if(previous_chart != nullptr)
    {
        previous_chart -> deleteLater();
    }
}
