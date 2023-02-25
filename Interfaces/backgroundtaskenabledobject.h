#ifndef BACKGROUNDTASKENABLEDOBJECT_H
#define BACKGROUNDTASKENABLEDOBJECT_H

#include "Interfaces/logconnectedobject.h"

/**
 * Interface to be inherited by all classes performing time-consuming tasks which are supposed to be moved to different threads.
 * Proper implementation allows the CoreController to manage multithreading.
 */
class BackgroundTaskEnabledObject : public LogConnectedObject
{
    Q_OBJECT
public:
    virtual ~BackgroundTaskEnabledObject(){};
    /**
     * Method which is meant to be perceived by the CoreController as a background task.
     * @param task_index - in case the child class of BackgroundTaskEnabledObject has more than one method intended to be background task
     * use this parameter to invoke the proper one.
     */
    virtual void start_background_task(int task_index) = 0;

signals:
    /**
     * Signal to be connected to progress bar of BackgroundTaskVisualization.
     * Emit it to update progress shwon to the user.
     * @param progress - current progress in range 0-100.
     */
    void signal_update_task_progress(int progress);
    /**
     * Signal to be connected to slot_update_task_message of BackgroundTaskVisualization.
     * Emit it to change currently shown message.
     * @param message - text to be shown as a part of task visualization.
     */
    void signal_update_task_message(QString message);
    /**
     * Signal to be connected to slot_process_task_completion of CoreController.
     * Emit it to signal that task has finished and corresponding visualization can be removed.
     */
    void signal_task_finished();
};

#endif // BACKGROUNDTASKENABLEDOBJECT_H
