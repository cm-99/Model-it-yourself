#ifndef TABWIDGETPAGE_H
#define TABWIDGETPAGE_H

#include "Essentials/logrelay.h"

#include <QWidget>

/**
 * Base class to be inherited by all classes meant to be used as main QTabWidget's pages.
 */
//It seems that for pages heavily dependent on valid Dataset being present TabWidgetPage could be subclassed as DatasetTabWidgetPage
//to extract slot_restore_to_default and set_dataset.
//...but DatasetEditorPage has direct feedback from its remove methods and does not need slot_update* like other two pages.
//TODO: think about it

class TabWidgetPage : public QWidget
{
    Q_OBJECT    
public:
    explicit TabWidgetPage(QWidget *parent = nullptr);
    /**
     * Returns pointer to LogRelay object.
     */
    LogRelay *get_log_relay();

private:
    LogRelay log_relay;

signals:
    void signal_relay_log_message(Log log, QString sender_name);
};

#endif // TABWIDGETPAGE_H
