#ifndef LOGSMANAGER_H
#define LOGSMANAGER_H

#include "Interfaces/logconnectedobject.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMutex>
#include <QApplication>
#include <QDate>
#include <QTime>

/**
 * Class managing logs.
 * This mainly means creating and deleting log files and saving logs sent by permitted objects.
 */
class LogsManager
{
public:
    LogsManager();
    virtual ~LogsManager();
    /**
     * Logs given message appending sender_name and current date.
     * @param log to be stored
     * @param sender_name of the class that send the message
     */
    void log_message(Log log, QString sender_name);

    /**
     * Deletes log files that exceed log_file_number_limit. If log_file_number_limit == 0, does not delete files.
     * @return number of deleted log files
     */
    void delete_old_log_files();

    /**
     * Sets limit of stored log files. Any file above it will be deleted when delete_old_log_files() is invoked.
     * @param logFileNumberLimit - limit of amount of stored log files
     */
    void set_log_file_number_limit(uint log_files_amount_limit);

    /**
     * Returns limit of amount of stored log files.
     * @return log_files_amount_limit
     */
    uint get_log_files_amount_limit();

private:

    /**
     * The file object to log into
     */
    QFile* log_file;
    /**
     * Stream adding strings to text file
     */
    QTextStream* log_stream;
    /**
     * Mutex to assert that file writing is done by one thread at the time
     */
    QMutex* mutex;
    /**
     * Folder containing log files
     */
    QDir logs_folder;
    /**
     * Stores directory of the logfile
     */
    QString log_file_path;
    /**
     * Determines if log_file_number_limit should be applied.
     */
    bool limit_logs_storage = true;
    /**
     * Limits amount of stored log files.
     */
    uint log_files_amount_limit;
    /**
     * Time format to be used in log files.
     */
    QString MIY_logs_date_time_format = "dd.MM.yyyy hh:mm:ss";
};

#endif // LOGSMANAGER_H
