#include "Essentials/corecontroller.h"
#include "ui_corecontroller.h"
#include "Implementations/csvdataimportandexportmanager.h"

#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>

CoreController::CoreController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoreController),
    mutex(new QMutex())
{
    prepare_logs_manager();

    prepare_data_import_and_export_managers();
    determine_supported_dataset_formats();

    //Prepare GUI
    ui->setupUi(this);
    prepare_GUI();

    prepare_log_connected_objects();
}

CoreController::~CoreController()
{
    delete ui;
    delete logs_manager;
    delete mutex;
    delete active_dataset;

    qDeleteAll(log_connected_objects);
    log_connected_objects.clear();
}

void CoreController::determine_supported_dataset_formats()
{
    QMap<QString, DataImportAndExportManager*>::const_iterator i;

    for(i = supported_formats_mapped_to_data_managers.constBegin(); i != supported_formats_mapped_to_data_managers.constEnd(); i++)
        supported_dataset_loading_and_saving_formats.append(i.key());

    Log log;
    log.function_info = Q_FUNC_INFO;
    log.message = "Supported dataset file formats: ";

    foreach(QString file_format, supported_dataset_loading_and_saving_formats)
        log.message += file_format;

    logs_manager -> log_message(log, "CoreController");
}

void CoreController::prepare_logs_manager()
{
    logs_manager = new LogsManager();
    uint default_logs_amount_permitted = 10;
    logs_manager -> set_log_file_number_limit(default_logs_amount_permitted);
    logs_manager -> delete_old_log_files();
}

void CoreController::prepare_log_connected_objects()
{
    for(int i = 0; i < log_connected_objects.length(); i++)
        connect(log_connected_objects[i], SIGNAL(signal_log_message(Log,QString)), this, SLOT(slot_receive_and_relay_log(Log,QString)));
}

void CoreController::prepare_data_import_and_export_managers()
{
    DataImportAndExportManager *csv_data_import_and_export_manager = new CsvDataImportAndExportManager();

    connect(csv_data_import_and_export_manager, &DataImportAndExportManager::signal_dataset_loaded, this, &CoreController::slot_change_active_dataset);
    supported_formats_mapped_to_data_managers.insert(csv_data_import_and_export_manager -> get_supported_format(), csv_data_import_and_export_manager);
    log_connected_objects.append(csv_data_import_and_export_manager);
}

void CoreController::prepare_dataset_tab_pages()
{
    dataset_editor = new DatasetEditorPage(dataset_tab_widget);
    dataset_tab_widget -> addTab(dataset_editor, "Dataset editor");
    dataset_details = new DatasetDetails(dataset_tab_widget);
    dataset_tab_widget -> addTab(dataset_details, "Dataset details");
    dataset_plots = new DatasetPlotsPage(dataset_tab_widget);
    dataset_tab_widget -> addTab(dataset_plots, "Dataset plots");

    LogRelay *dataset_editors_logs_connector = dataset_editor -> get_log_relay();
    log_connected_objects.append(dataset_editors_logs_connector);
    LogRelay *dataset_details_logs_connector = dataset_details -> get_log_relay();
    log_connected_objects.append(dataset_details_logs_connector);
    LogRelay *dataset_plots_logs_connector = dataset_plots -> get_log_relay();
    log_connected_objects.append(dataset_plots_logs_connector);
}

void CoreController::prepare_background_tasks_page(QTabWidget *central_tab_widget)
{
    background_tasks = new BackgroundTasksPage(central_tab_widget);
    central_tab_widget -> addTab(background_tasks, "Background tasks");

    LogRelay *background_tasks_log_connector = dataset_editor -> get_log_relay();
    log_connected_objects.append(background_tasks_log_connector);
}

void CoreController::prepare_GUI()
{
    //Populating menu bar
    QMenu *file_menu = menuBar() -> addMenu(tr("&File"));
    file_menu -> setTitle("File");

    //Adding actions to menus
    //TODO: separate it into a method
    QAction *load_file_action = new QAction(tr("&Load dataset"), this);
    load_file_action -> setShortcuts(QKeySequence::Open);
    load_file_action -> setStatusTip(tr("Load file as a new dataset"));
    connect(load_file_action, &QAction::triggered, this, &CoreController::slot_load_dataset);
    file_menu -> addAction(load_file_action);

    file_menu -> show();

    //Preparing essential tabs to be filled by classes responsible for handling corresponding functionalities
    //TODO: separate it into a method
    //TODO: connect all LogRelays owned by pages to logs_manager
    QTabWidget *central_tab_widget = new QTabWidget(this);
    setCentralWidget(central_tab_widget);

    dataset_tab_widget = new QTabWidget();
    QTabWidget *models_tab_widget = new QTabWidget();

    central_tab_widget -> addTab(dataset_tab_widget, "Dataset");
    central_tab_widget -> addTab(models_tab_widget, "Models");

    prepare_dataset_tab_pages();
    prepare_background_tasks_page(central_tab_widget);
}

void CoreController::create_background_task(BackgroundTaskEnabledObject *task_object, int task_index)
{
    background_tasks -> add_new_task_visualization(task_object);
    connect(task_object, &BackgroundTaskEnabledObject::signal_task_finished, this, &CoreController::slot_process_task_completion);
    //TODO: Check if task can even be started
    //Maybe create an enum in place of task_index to make it cleaner?
    QtConcurrent::run(task_object, &BackgroundTaskEnabledObject::start_background_task, task_index);
}

void CoreController::slot_receive_and_relay_log(Log log, QString senderName)
{
    QMutexLocker locker(mutex);
    if (senderName == "")
        logs_manager -> log_message(log, QObject::sender()->metaObject()->className());
    else
        logs_manager -> log_message(log, senderName);

    if(log.importance_level == enum_message_importance_level::WARNING)
        QMessageBox::warning(this, "MIY warning", "Warning: " + log.message, QMessageBox::Ok | QMessageBox::NoButton);
    else if(log.importance_level == enum_message_importance_level::ERRO)
    {
        QMessageBox::critical(this, "MIY ERROR", "MIY encountered critical error and will close now: " + log.message, QMessageBox::Ok | QMessageBox::NoButton);
        QCoreApplication::quit();
    }
}

void CoreController::slot_change_active_dataset(EditableDataset *new_dataset)
{
    EditableDataset *previous_dataset = active_dataset;
    active_dataset = new_dataset;

    //Calculate signals statistics now that dataset is complete
    //active_dataset -> calculate_signals_statistics();

    //Prepare all dataset connections
    connect(active_dataset, &EditableDataset::signal_all_data_removed, dataset_editor, &DatasetEditorPage::slot_restore_to_default);

    connect(active_dataset, &EditableDataset::signal_all_data_removed, dataset_details, &DatasetDetails::slot_restore_to_default);
    connect(active_dataset, &EditableDataset::signal_dataset_edited, dataset_details, &DatasetDetails::slot_update_widgets);

    //Change dataset being represented by dataset_tab_widget
    dataset_editor -> set_dataset(new_dataset);
    dataset_details -> set_dataset(new_dataset);
    dataset_plots -> set_dataset(new_dataset);

    if(previous_dataset != nullptr)
        previous_dataset -> deleteLater();
}

void CoreController::slot_process_task_completion()
{
    QObject *sender = QObject::sender();

    BackgroundTaskEnabledObject *object_which_finished_task = dynamic_cast<BackgroundTaskEnabledObject*>(sender);
    if(object_which_finished_task != nullptr)
        background_tasks -> remove_task_visualization(object_which_finished_task);
}

void CoreController::slot_restore_dataset_tabs_to_default_state()
{

}

void CoreController::delegate_dataset_loading(QString dataset_file_path)
{
    if(dataset_file_path.isEmpty())
        return;

    QString sender_name = "CoreController";
    Log log;
    log.function_info = Q_FUNC_INFO;

    QString dataset_file_extension = dataset_file_path.split(".").last();
    QString dataset_file_extension_in_managers_format = "*." + dataset_file_extension;

    if(supported_formats_mapped_to_data_managers.contains(dataset_file_extension_in_managers_format))
    {
        log.message = "Starting loading file: " + dataset_file_path;
        logs_manager -> log_message(log, sender_name);

        DataImportAndExportManager *selected_data_manager = supported_formats_mapped_to_data_managers.value(dataset_file_extension_in_managers_format);
        selected_data_manager -> set_dataset_file_path(dataset_file_path);

        int task_index = 0;
        create_background_task(selected_data_manager, task_index);
    }
    else
    {
        log.message = "MIY could not load file: " + dataset_file_path;
        log.importance_level = enum_message_importance_level::WARNING;
        this -> slot_receive_and_relay_log(log, sender_name);
    }
}

void CoreController::slot_load_dataset()
{
    QString dataset_file_path;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(qApp -> applicationDirPath());
    dialog.setNameFilters(supported_dataset_loading_and_saving_formats);
    dialog.setWindowTitle("Load file");

    QStringList file_paths = QStringList();

    if (dialog.exec())
        file_paths = dialog.selectedFiles();

    if (file_paths.size() > 0)
        dataset_file_path = file_paths.at(0);

    delegate_dataset_loading(dataset_file_path);
}
