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

class DatasetPlotsPage : public TabWidgetPage
{
    Q_OBJECT
public:
    explicit DatasetPlotsPage(QWidget *parent = nullptr);

    /**
     * Creates signals plots from provided dataset and relays this change to QChartView.
     * For each signal there is checkbox created which can be used to show/hide corresponding plot.
     */
    void set_dataset(ReadOnlyDataset *dataset);

private:
    const ReadOnlyDataset *current_dataset = nullptr;

    QChart* chart = nullptr;
    QChartView chart_view;
    QHBoxLayout check_boxes_layout;
    QVBoxLayout main_layout;
};

#endif // DATASETPLOTSPAGE_H
