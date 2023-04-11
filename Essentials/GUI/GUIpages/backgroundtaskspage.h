#ifndef BACKGROUNDTASKSPAGE_H
#define BACKGROUNDTASKSPAGE_H

#include "Interfaces/backgroundtaskenabledobject.h"
#include "Essentials/GUI/GUIelements/backgroundtaskvisualization.h"
#include "Essentials/GUI/GUIpages/tabwidgetpage.h"

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>

/**
 * Class managing GUI for viewing currently performed background tasks.
 */
class BackgroundTasksPage : public TabWidgetPage
{
    Q_OBJECT
public:
    explicit BackgroundTasksPage(QWidget *parent = nullptr);
    ~BackgroundTasksPage();

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
    QVBoxLayout layout;
    QMap<BackgroundTaskEnabledObject*, BackgroundTaskVisualization*> tasks_mapped_to_visualizations;

};

#endif // BACKGROUNDTASKSPAGE_H
