#include "Essentials/logsmanager.h"

#include <QDebug>

LogsManager::LogsManager() :
    log_file(nullptr),
    log_stream(nullptr),
    mutex(new QMutex())
{
    QString logs_folder_path = qApp->applicationDirPath() + "/logs";
    this -> logs_folder = QDir(logs_folder_path);

    //Create logs folder if it does not already exist
    if (!(QDir().exists(logs_folder_path)))
    {
        QDir().mkdir(logs_folder_path);
    }

    QString current_date_time_as_string = QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss");
    QString log_file_name = "log_" + current_date_time_as_string + ".txt";
    log_file_path = logs_folder.absoluteFilePath(log_file_name);

    //Create the file object
    log_file = new QFile(log_file_path);

    //Create actual file and open it for writing
    if (log_file->open(QIODevice::ReadWrite))
    {
        log_stream = new QTextStream(log_file);
        *log_stream << "MIY log:" << "\r\n";
    }
}

LogsManager::~LogsManager()
{
    log_file->close();
    delete log_file;
    delete mutex;
    if(log_stream)
        delete log_stream;
}

void LogsManager::log_message(Log log, QString sender_name)
{
    QMutexLocker locker(mutex);
    QString current_date_time_as_string = QDateTime::currentDateTime().toString(MIY_logs_date_time_format);

    *log_stream << "Time: " << current_date_time_as_string << " | Log importance level: " << log.importance_level << " | Sender: "
                << sender_name << " | Log function info: " << log.function_info << " | Log message: " << log.message << "\r\n";
    qDebug() << "Time: " << current_date_time_as_string << " | Log importance level: " << log.importance_level << " | Sender: "
             << sender_name << " | Log function info: " << log.function_info << " | Log message: " << log.message << "\r\n";
}

void LogsManager::delete_old_log_files()
{
    QString sender = "LogsManager";
    Log log;
    log.function_info = Q_FUNC_INFO;

    QString path = logs_folder.absolutePath();
    QDir dir(path);
    dir.setNameFilters(QStringList() << "log_*.txt");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time | QDir::Reversed);
    QStringList logs_names_list = dir.entryList();
    int amount_of_deleted_logs = 0;

    //Remove all logs exceeding allowed amount if logs storage is set to be limited.
    if(limit_logs_storage && (uint)logs_names_list.length() > log_files_amount_limit)
    {
        while((uint)logs_names_list.length() > log_files_amount_limit)
        {
            dir.remove(logs_names_list.first());
            logs_names_list.removeFirst();
            amount_of_deleted_logs ++;
        }

        log.message = "Amount of deleted logs: " + QString::number(amount_of_deleted_logs) + ". Remaining: "
                + QString::number(logs_names_list.length());
    }
    else
        log.message = "No logs required to be deleted";

    this -> log_message(log, sender);
}

void LogsManager::set_log_file_number_limit(uint log_files_amount_limit)
{
    this->log_files_amount_limit = log_files_amount_limit;
}

uint LogsManager::get_log_files_amount_limit()
{
    return this -> log_files_amount_limit;
}
