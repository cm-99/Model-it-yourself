#ifndef CORECONTROLLER_H
#define CORECONTROLLER_H

#include "Essentials/GUI/GUIpages/DatasetEditorPage.h"
#include "Essentials/GUI/GUIpages/datasetplotspage.h"
#include "Essentials/GUI/GUIpages/backgroundtaskspage.h"
#include "Essentials/GUI/GUIpages/datasetdetails.h"
#include "Essentials/logsmanager.h"
#include "Essentials/editabledataset.h"
#include "Essentials/signalsstatisticscalculator.h"
#include "Interfaces/dataimportandexportmanager.h"

#include <QMap>
#include <QMessageBox>
#include <QMainWindow>
#include <QTableView>

namespace Ui {
class CoreController;
}

/**
 * The CoreController class is MVC architecture's controller.
 * It provides main GUI to be filled by classes responsible for particular functionalities,
 * handles forwarding of user's actions to those classes and manages memory.
 */
class CoreController : public QMainWindow
{
    Q_OBJECT

public:
    explicit CoreController(QWidget *parent = nullptr);
    ~CoreController();

private:
    // Private attributess

    /**
     * Main application GUI.
     * QMenuBar and QTabWidgets are to be filled with QActions/QWidgets prepared by classes responsible for handling particular functionalities.
     */
    Ui::CoreController *ui;
    /**
     * Mutex to assert that relaying messages to LogsManager is done by one thread at a time
     */
    QMutex *mutex;

    // Parameters

    /**
     * Default message display time in status bar. It can be overriden by showing a new message.
     */
    int default_message_timeout_msc = 3000;

    // Objects/containers holding objects providing different functionalities

    /**
     * Provides message logging capability, stores them in a file.
     */
    LogsManager logs_manager;
    /**
     * Currently active dataset. It can be altered, displayed using plots [TODO: or used to create a new model].
     */
    EditableDataset *active_dataset = nullptr;
    /**
     * Provides signals statistics calculating capability.
     * Should be connected to signals emitted from active_dataset on its edition in order to perfrom a recalculation.
     */
    SignalsStatisticsCalculator signals_statistics_calculator;
    /**
     * QMap holding pairs of file extensions and pointers to objects providing dataset loading and saving functionalities for those extensions.
     */
    QMap<QString, DataImportAndExportManager*> supported_formats_mapped_to_data_managers;
    /**
     * List holding pointers to objects which need to be connected to slot_receive_and_relay_log
     */
    QList<LogConnectedObject*> log_connected_objects;
    /**
     * Dataset file formats supported by existing implementaions of DataImportAndExportManager interface.
     */
    QStringList supported_dataset_loading_and_saving_formats;

    ////GUI attributes
    QTabWidget central_tab_widget;

    QTabWidget dataset_tab_widget;
    QTabWidget models_tab_widget;

    // Dataset tab pages
    DatasetEditorPage dataset_editor;
    DatasetDetails dataset_details;
    DatasetPlotsPage dataset_plots;

    // Models tab pages

    // Background tasks pages
    BackgroundTasksPage background_tasks;

    ////Private methods

    /**
     * Starts implementation of start_background_task in a different thread,
     * creates instance of BackgroundTaskVisualization to keep track of its progress.
     * @param task_object - object for which a new background task is created.
     * @param task_index - index of method which should be started from start_background_task.
     */
    void create_background_task(BackgroundTaskEnabledObject *task_object, int task_index);

    /**
     * Determines supported dataset formats depending on existing implementations of DataImportAndExportManager pointed to by
     * supported_formats_mapped_to_data_managers. Creates list supported_dataset_loading_and_saving_formats.
     */
    void determine_supported_dataset_formats();
    /**
     * Prepares logs_manager - which means its initialization and deletion of old log files depending on current settings.
     */
    void prepare_logs_manager();
    /**
     * Delegates dataset loading to proper implementation of DataImportAndExportManager depending on format of @param:dataset_file_path.
     * If no implementation supporting this format is found a log of WARNING importance level is created.
     * This could mean that either list supported_dataset_loading_and_saving_formats was not created properly
     * or object supporting selected file's format was already destroyed which should not be possible.
     * For more information see slot_load_dataset.
     */
    void delegate_dataset_loading(QString dataset_file_path);
    /**
     * Connect all objects allowed to send logs to slot_receive_and_relay_log
     */
    void prepare_log_connected_objects();
    void prepare_data_import_and_export_managers();

    // GUI methods

    void prepare_GUI();
    void prepare_dataset_tab_pages();
    void prepare_background_tasks_pages();

private slots:
    /**
     * Receives log and relays it to logs_manager to be written into a file.
     * Depending on log's importance level it may also:
     * WARNING - show user QMessageBox of corresponding importance level with log's message
     * ERROR - show user QMessageBox of corresponding importance level with log's message and close the application
     */
    void slot_receive_and_relay_log(Log log, QString sender_name);
    /**
     * Changes active dataset in response to emission of signal_dataset_loaded.
     * Relays this change to all classes referencing Dataset_TableModel.
     * Previous dataset is deleted.
     * @param new_model - new dataset model to replace active_dataset_model.
     */
    void slot_change_active_dataset(EditableDataset *new_dataset);
    /**
     * For now - removes task visualization for completed task.
     */
    void slot_process_task_completion();

    // GUI slots

    /**
     * Loads dataset from file specified by user through file dialog.
     * File is selectable if its extension is contained by supported_dataset_loading_and_saving_formats.
     */
    void slot_load_dataset();
};

#endif // CORECONTROLLER_H
