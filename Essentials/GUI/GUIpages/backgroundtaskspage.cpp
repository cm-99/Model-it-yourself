#include "Essentials/GUI/GUIpages/BackgroundTasksPage.h"

#include <QDebug>
#include <QProgressBar>
#include <QScrollArea>
#include <QSpinBox>

BackgroundTasksPage::BackgroundTasksPage(QWidget *parent)
    : TabWidgetPage{parent}
{
    this -> setLayout(&layout);
    layout.setAlignment(Qt::AlignTop);
}

BackgroundTasksPage::~BackgroundTasksPage(){}

void BackgroundTasksPage::add_new_task_visualization(BackgroundTaskEnabledObject *new_task)
{
    BackgroundTaskVisualization *new_visualization = new BackgroundTaskVisualization(this);
    new_visualization -> connect_task_to_visualization(new_task);
    tasks_mapped_to_visualizations.insert(new_task, new_visualization);
    layout.addWidget(new_visualization);
}

void BackgroundTasksPage::remove_task_visualization(BackgroundTaskEnabledObject *task_completed)
{
    if(tasks_mapped_to_visualizations.contains(task_completed))
    {
        BackgroundTaskVisualization *visualization_to_remove = tasks_mapped_to_visualizations.value(task_completed);
        tasks_mapped_to_visualizations.remove(task_completed);
        this -> layout.removeWidget(visualization_to_remove);
        visualization_to_remove -> hide();
        visualization_to_remove -> deleteLater();
    }
}
