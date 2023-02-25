#ifndef BACKGROUNDTASKSGUI_H
#define BACKGROUNDTASKSGUI_H

#include "Interfaces/backgroundtaskenabledobject.h"
#include "Essentials/backgroundtaskvisualization.h"

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>

/**
 * Class managing GUI for viewing currently performed background tasks.
 */
class BackgroundTasksGUI : public QScrollArea
{
    Q_OBJECT
public:
    explicit BackgroundTasksGUI(QWidget *parent = nullptr);
    ~BackgroundTasksGUI();

    /**
     * Adds new object of type BackgroundTaskVisualization connected to @param new_task.
     * Inserts those in the map tasks_mapped_to_visualizations.
     */
    void add_new_task_visualization(BackgroundTaskEnabledObject *new_task);
    /**
     * Removes visualization corresponding to @param task_completed if found in tasks_mapped_to_visualizations.
     */
    void remove_task_visualization(BackgroundTaskEnabledObject *task_completed);

private:
    QWidget *central_widget = nullptr;
    QVBoxLayout *layout = nullptr;
    QMap<BackgroundTaskEnabledObject*, BackgroundTaskVisualization*> tasks_mapped_to_visualizations;

};

#endif // BACKGROUNDTASKSGUI_H
