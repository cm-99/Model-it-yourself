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
    prepare_log_connected_objects();

    //Prepare GUI
    ui->setupUi(this);
    prepare_GUI();
}

CoreController::~CoreController()
{
    delete ui;
    delete logs_manager;
    delete mutex;
    delete active_dataset_model;

    qDeleteAll(log_connected_objects);
    log_connected_objects.clear();

    qDeleteAll(supported_formats_mapped_to_data_managers);
    supported_formats_mapped_to_data_managers.clear();
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

    logs_manager ->log_message(log, "CoreController");
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
    QVectorIterator<LogConnectedObject*> log_connected_objects_iterator(log_connected_objects);
    while(log_connected_objects_iterator.hasNext())
        connect(log_connected_objects_iterator.next(), SIGNAL(signal_log_message(Log,QString)), this, SLOT(slot_receive_and_relay_log(Log,QString)));
}

void CoreController::prepare_data_import_and_export_managers()
{
    DataImportAndExportManager *csv_data_import_and_export_manager = new CsvDataImportAndExportManager();

    connect(csv_data_import_and_export_manager, SIGNAL(signal_dataset_loaded(Dataset_TableModel*)), this, SLOT(slot_change_active_dataset(Dataset_TableModel*)));
    supported_formats_mapped_to_data_managers.insert(csv_data_import_and_export_manager -> get_supported_format(), csv_data_import_and_export_manager);
    log_connected_objects.append(csv_data_import_and_export_manager);
}

void CoreController::prepare_dataset_editor_tab(QTabWidget *dataset_tab_widget)
{
    dataset_editor = new QTableView();

    dataset_editor ->setStyleSheet("QHeaderView::section { background-color:grey }"
                                   "QTableCornerButton::section { background-color:grey }");
    dataset_tab_widget -> addTab(dataset_editor, "Dataset editor");
}

void CoreController::prepare_background_tasks_tab(QTabWidget *central_tab_widget)
{
    background_tasks = new BackgroundTasksGUI(central_tab_widget);
    central_tab_widget -> addTab(background_tasks, "Background tasks");
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
    QTabWidget *central_tab_widget = new QTabWidget(this);
    setCentralWidget(central_tab_widget);

    QTabWidget *dataset_tab_widget = new QTabWidget();
    QTabWidget *models_tab_widget = new QTabWidget();

    central_tab_widget -> addTab(dataset_tab_widget, "Dataset");
    central_tab_widget -> addTab(models_tab_widget, "Models");

    prepare_background_tasks_tab(central_tab_widget);
    prepare_dataset_editor_tab(dataset_tab_widget);
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
    else if(log.importance_level == enum_message_importance_level::ERROR)
    {
        QMessageBox::critical(this, "MIY ERROR", "MIY encountered critical error and will close now: " + log.message, QMessageBox::Ok | QMessageBox::NoButton);
        QCoreApplication::quit();
    }
}

void CoreController::slot_change_active_dataset(Dataset_TableModel *new_model)
{
    Dataset_TableModel *previous_dataset_model = active_dataset_model;
    active_dataset_model = new_model;

    //TODO: Update all widgets using Dataset_TableModel when current one is changed, extract it into a new method
    dataset_editor -> setModel(active_dataset_model);

    if(previous_dataset_model != nullptr)
        previous_dataset_model -> deleteLater();
}

void CoreController::slot_process_task_completion()
{
    QObject *sender = QObject::sender();

    BackgroundTaskEnabledObject *object_which_finished_task = dynamic_cast<BackgroundTaskEnabledObject*>(sender);
    if(object_which_finished_task != nullptr)
    {
        object_which_finished_task ->disconnect();
        background_tasks -> remove_task_visualization(object_which_finished_task);
    }
}

void CoreController::delegate_dataset_loading(QString dataset_file_path)
{
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
