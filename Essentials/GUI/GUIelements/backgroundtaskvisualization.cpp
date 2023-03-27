#include "Essentials/GUI/GUIelements/backgroundtaskvisualization.h"

#include <QTime>

BackgroundTaskVisualization::BackgroundTaskVisualization(QWidget *parent, QString message)
    : QFrame{parent}
{
    this -> setFrameStyle(QFrame::Panel | QFrame::Raised);
    task_start_time = new QLabel(this);
    progress_bar = new QProgressBar(this);
    task_message = new QLabel(this);
    layout = new QVBoxLayout(this);

    task_start_time->setText("Task start time: " + QTime::currentTime().toString());
    progress_bar->setRange(0,100);
    task_message->setText(message);

    layout->addWidget(task_start_time);
    layout->addWidget(progress_bar);
    layout->addWidget(task_message);
}

BackgroundTaskVisualization::~BackgroundTaskVisualization()
{
    //TODO: Check deletion order, assert that everything is destroyed
    delete task_message;
    delete task_start_time;
    delete progress_bar;
    delete layout;
}

void BackgroundTaskVisualization::connect_task_to_visualization(BackgroundTaskEnabledObject *task)
{
    connect(task, &BackgroundTaskEnabledObject::signal_update_task_message, this, &BackgroundTaskVisualization::slot_update_task_message);
    connect(task, &BackgroundTaskEnabledObject::signal_update_task_progress, progress_bar, &QProgressBar::setValue);
}

void BackgroundTaskVisualization::slot_update_task_message(QString message)
{
    task_message->setText(message);
}

