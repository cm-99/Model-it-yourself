#ifndef BACKGROUNDTASKVISUALIZATION_H
#define BACKGROUNDTASKVISUALIZATION_H

#include "Interfaces/backgroundtaskenabledobject.h"
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

/**
 * GUI for viewing progress of a single bacground task - it consists of task's start time, progress bar and a message.
 * Last two can be altered by corresponding task - see \l BackgroundTaskVisualization::connect_task_to_visualization().
 */
class BackgroundTaskVisualization: public QFrame
{
    Q_OBJECT
public:
    explicit BackgroundTaskVisualization(QWidget *parent = nullptr, QString message = "");
    ~BackgroundTaskVisualization();

    /**
     * Connects all signals of @param task to corresponding slots of BackgroundTaskVisualization or its member objects
     * to enable task visualization updates.
     */
    void connect_task_to_visualization(BackgroundTaskEnabledObject *task);

private:
    QVBoxLayout *layout;

    QLabel *task_start_time;
    QProgressBar *progress_bar;
    QLabel *task_message;

private slots:
    /**
     * Slot which updates visualizations task_message with @param message.
     * To be connected to corresponding object's of type BackgroundTaskEnabledObject signal_update_task_message.
     */
    void slot_update_task_message(QString message);
};

#endif // BACKGROUNDTASKVISUALIZATION_H
