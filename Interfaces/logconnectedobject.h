#ifndef LOGCONNECTEDOBJECT_H
#define LOGCONNECTEDOBJECT_H

enum enum_message_importance_level
{
    DEBUG = 0, //general information
    WARNING = 1, //information about events that the user might want to pay special attention to
    ERRO = 2 //notification about an error, potentially disabling further object use. No, it's not misspelled. It was throwing errors.
};

#include <QObject>

/**
 * Struct binding together components of log - function info, message, importance level.
 * If importance level is higher than DEBUG a QMessageBox of corresponding type should be shown to the user with Log.message.
 */
struct Log
{
    QString function_info;
    QString message;
    enum_message_importance_level importance_level;

    /** Default constructor - log importance_level is DEBUG*/
    explicit Log()
    {
        this -> function_info = "";
        this -> message = "";
        this -> importance_level = enum_message_importance_level::DEBUG;
    }

    /** Full constructor */
    Log(QString function_info, QString message, enum_message_importance_level importance_level)
    {
        this -> function_info = function_info;
        this -> message = message;
        this -> importance_level = importance_level;
    }

    virtual ~Log(){}
};
Q_DECLARE_METATYPE(Log)

class LogConnectedObject : public QObject
{
    Q_OBJECT
public:
    virtual ~LogConnectedObject(){};

signals:
    /**
     * Signal to be connected to slot_receive_and_relay_log.
     * Emit it to save a log to file.
     */
    void signal_log_message(Log log, QString sender_name);
};

#endif // LOGCONNECTEDOBJECT_H
