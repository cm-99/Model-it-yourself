#include "Essentials/backgroundtasksgui.h"

#include <QDebug>
#include <QProgressBar>
#include <QScrollArea>
#include <QSpinBox>

BackgroundTasksGUI::BackgroundTasksGUI(QWidget *parent)
    : QScrollArea{parent}
{
    central_widget = new QWidget(this);
    layout = new QVBoxLayout(central_widget);

    this -> setWidget(central_widget);
    this -> setWidgetResizable(true);

    layout -> setAlignment(Qt::AlignTop);
}

BackgroundTasksGUI::~BackgroundTasksGUI()
{
    //TODO: Check deletion order, assert that everything is destroyed
    delete central_widget;
}

void BackgroundTasksGUI::add_new_task_visualization(BackgroundTaskEnabledObject *new_task)
{
    BackgroundTaskVisualization *new_visualization = new BackgroundTaskVisualization(this);
    new_visualization -> connect_task_to_visualization(new_task);
    tasks_mapped_to_visualizations.insert(new_task, new_visualization);
    layout -> addWidget(new_visualization);
}

void BackgroundTasksGUI::remove_task_visualization(BackgroundTaskEnabledObject *task_completed)
{
    if(tasks_mapped_to_visualizations.contains(task_completed))
    {
        BackgroundTaskVisualization *visualization_to_remove = tasks_mapped_to_visualizations.value(task_completed);
        tasks_mapped_to_visualizations.remove(task_completed);
        this -> layout -> removeWidget(visualization_to_remove);
        visualization_to_remove -> hide();
        visualization_to_remove -> deleteLater();
    }
}
