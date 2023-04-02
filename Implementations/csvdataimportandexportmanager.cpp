#include "Implementations/csvdataimportandexportmanager.h"
#include "Essentials/readonlydataset.h"

#include <QChar>
#include <QTime>
#include <QFileInfo>

CsvDataImportAndExportManager::CsvDataImportAndExportManager()
{
    supported_format = QObject::tr("*.csv");
}

QString CsvDataImportAndExportManager::get_supported_format()
{
    return supported_format;
}

void CsvDataImportAndExportManager::start_background_task(int task_index)
{
    if(task_index == 0)
        load_dataset();
    else if(task_index == 1)
        save_dataset();
}

void CsvDataImportAndExportManager::load_dataset_without_timestamps_into_containers(char separator, EditableDataset *dataset, QTextStream *dataset_file_text_stream)
{
    int dataset_length = dataset -> get_rows_count();
    int data_columns_count = dataset -> get_columns_count();

    int progress_bar_step_value = dataset_length/100;
    int progress_value = 1;
    int current_step_loading_progress = 0;

    for(int i = 0; i < dataset_length; i++)
    {
        QStringList file_line_splitted = dataset_file_text_stream->readLine().split(separator);
        for(int j = 0; j < data_columns_count; j++)
            dataset -> append_sample(j, file_line_splitted.at(j).toDouble());

        current_step_loading_progress++;
        if(current_step_loading_progress == progress_bar_step_value)
        {
            current_step_loading_progress = 0;
            emit signal_update_task_progress(progress_value);
            progress_value++;
        }
    }
}

void CsvDataImportAndExportManager::load_dataset_with_timestamps_into_containers(char separator, EditableDataset *dataset, QTextStream *dataset_file_text_stream)
{
    int dataset_length = dataset -> get_rows_count();
    int data_columns_count = dataset -> get_columns_count() - 1; //Offset by 1 because timestamps are present
    int progress_bar_step_value = dataset_length/100;
    int progress_value = 1;
    int current_step_loading_progress = 0;

    for(int i = 0; i < dataset_length; i++)
    {
        QStringList file_line_splitted = dataset_file_text_stream->readLine().split(separator);

        dataset -> append_timestamp(file_line_splitted.at(0));
        for(int j = 0; j < data_columns_count; j++)
            dataset -> append_sample(j, file_line_splitted.at(j+1).toDouble());

        current_step_loading_progress++;
        if(current_step_loading_progress == progress_bar_step_value)
        {
            current_step_loading_progress = 0;
            emit signal_update_task_progress(progress_value);
            progress_value++;
        }
    }
}

bool CsvDataImportAndExportManager::prepare_dataset_headers(EditableDataset *dataset, char separator, QString first_row)
{
    bool headers_present = false;

    //Try to load headers from dataset file
    foreach(const QChar character, first_row.split(separator).first())
    {
        if(character.isLetter())
        {
            headers_present = true;
            QStringList headers = first_row.split(separator);

            for(int i = 0; i < headers.length(); i++)
            {
                dataset -> append_signal_info(SignalInfo(headers.at(i)));
            }

            return headers_present;
        }
    }

    //If there are no headers prepare default ones
    if(!headers_present)
        for(int i = 0; i < first_row.split(separator).length(); i++)
            dataset -> append_signal_info(SignalInfo(QString("Signal_%1").arg(i)));

    return headers_present;
}

void CsvDataImportAndExportManager::load_dataset()
{
    //TODO: Clean it, break it up into methods
    emit signal_update_task_message("Checking dataset validity of file: " + dataset_file_path);
    QString sender_name = "CsvDataImportAndExportManager";
    Log log;
    log.function_info = Q_FUNC_INFO;

    QFile dataset_file(dataset_file_path);
    if(!dataset_file.open(QIODevice::ReadOnly))
    {
        log.importance_level = enum_message_importance_level::WARNING;
        log.message = "File could not be opened: " + dataset_file_path;
        emit signal_log_message(log, sender_name);
        dataset_file.close();
        return;
    }

    QTextStream dataset_file_text_stream(&dataset_file);

    //There must be at least three rows of data to perform required checks
    QStringList first_three_rows = get_first_three_rows(&dataset_file_text_stream, dataset_file_path);
    if(first_three_rows.length() != 3)
        return;

    QString first_row = first_three_rows.at(0);
    //Check if it is comma separated file. Should also handle other typical separators.
    char separator = ',';
    if (!first_row.contains(','))
        separator = '\t';

    //Prepare data for next checks
    QString presumably_timestamp = first_three_rows.at(1).split(separator).first();
    QString presumably_next_timestamp = first_three_rows.at(2).split(separator).first();

    //Check dataset's validity
    //Check if it is a proper dataset - rows contain the same number of cells (there should be more checks during dataset loading)
    int first_row_columns_count = first_three_rows.at(0).split(separator).length();
    int second_row_columns_count = first_three_rows.at(1).split(separator).length();

    if(first_row_columns_count != second_row_columns_count)
    {
        log.importance_level = enum_message_importance_level::WARNING;
        log.message = "File " + dataset_file_path + " could not be loaded. Rows do not contain equal numbers of columns.";
        emit signal_log_message(log, sender_name);
        dataset_file.close();
        return;
    }

    EditableDataset *dataset = new EditableDataset();
    bool headers_present = prepare_dataset_headers(dataset, separator, first_three_rows.at(0));

    //Attemp to get sampling time. On fail it will be equal to 0.
    bool timestamps_present = false;
    double sampling_time_in_seconds = get_dataset_sampling_time_in_seconds(presumably_timestamp, presumably_next_timestamp);
    if(sampling_time_in_seconds > 0)
        timestamps_present = true;

    dataset -> set_sampling_time_in_seconds(sampling_time_in_seconds);

    //Getting dataset_lenght to reserve size in dataset container
    //TODO: Check if it can be done without iterating through the whole file
    int dataset_length = 3;
    dataset_file_text_stream.readLine();

    while (!dataset_file_text_stream.atEnd())
    {
        dataset_length += 1;
        dataset_file_text_stream.readLine();
    }

    //There is separate container for signals info, headers included - do not reserve size for it in dataset container
    if(headers_present)
        dataset_length --;

    int data_columns_count = first_row_columns_count;
    if(timestamps_present)
    {
        //How many data_columns should be prepared (timestamp has separate container)
        data_columns_count--;
    }

    dataset -> prepare_dataset_container(data_columns_count, dataset_length, timestamps_present);
    emit signal_update_task_message("Creating new dataset from file: " + dataset_file_path);

    //Go back to beginning of the file
    dataset_file_text_stream.seek(0);
    if(timestamps_present)
        load_dataset_with_timestamps_into_containers(separator, dataset, &dataset_file_text_stream);
    else
        load_dataset_without_timestamps_into_containers(separator, dataset, &dataset_file_text_stream);

    dataset_file.close();
    QFileInfo dataset_file_info(dataset_file);
    dataset -> set_dataset_name(dataset_file_info.baseName());

    log.message = "Dataset loaded from file: " + dataset_file_path;
    emit signal_log_message(log, sender_name);
    emit signal_dataset_loaded(dataset); //For CoreController to manager dataset change and prepare its representation
    emit signal_task_finished(); //For BackgroundTasksPage to remove the visualization
}

//TODO:
void CsvDataImportAndExportManager::save_dataset()
{

}

void CsvDataImportAndExportManager::set_dataset_file_path(QString dataset_file_path)
{
    this -> dataset_file_path = dataset_file_path;
}

double CsvDataImportAndExportManager::get_dataset_sampling_time_in_seconds(QString presumably_timestamp, QString presumably_next_timestamp)
{
    double sampling_time = 0;

    QPair<int, QString> format = check_timestamps_validity(presumably_timestamp);
    if(format.first == -1)
        return sampling_time;

    //If timestamps are in Time format those will be correct
    QString timestamps_time_part = presumably_timestamp;
    QString next_timestamps_time_part = presumably_next_timestamp;

    //If timestamps are in DateTime format - take only Time part
    if(format.first == 0)
    {
        timestamps_time_part = presumably_timestamp.split(" ").last();
        next_timestamps_time_part = presumably_next_timestamp.split(" ").last();
    }

    QTime timestamp_as_time = QTime::fromString(timestamps_time_part, format.second);
    QTime next_timestamp_as_time = QTime::fromString(next_timestamps_time_part, format.second);

    //If either time is invalid 0 will be returned
    sampling_time = timestamp_as_time.msecsTo(next_timestamp_as_time);
    if(sampling_time < 0)
        sampling_time = 0;

    return sampling_time/1000;
}

QPair<int, QString> CsvDataImportAndExportManager::check_timestamps_validity(QString presumably_timestamp)
{
    QString sender_name = "CsvDataImportAndExportManager";
    QPair<int, QString> format;

    if(presumably_timestamp.contains(":"))
    {
        format.first = 1;
        //If it is in DateTime format - reduce it to only Time format
        if(presumably_timestamp.contains(" "))
        {
            presumably_timestamp = presumably_timestamp.split(" ").last();
            format.first = 0;
        }

        QString matching_time_format = check_timestamps_compatiblity_with_time_formats(presumably_timestamp);
        if(!matching_time_format.isEmpty())
        {
            format.second = matching_time_format;
            emit signal_log_message(Log(Q_FUNC_INFO, "Recognized timestamp Time format: " + matching_time_format,
                                        enum_message_importance_level::DEBUG), sender_name);
            return format;
        }
    }
    format.first = -1;
    emit signal_log_message(Log(Q_FUNC_INFO, "Timestamp's Time format was not recognized: " + presumably_timestamp,
                                enum_message_importance_level::DEBUG), sender_name);
    return format;
}

QString CsvDataImportAndExportManager::check_timestamps_compatiblity_with_time_formats(QString timestamp_to_check)
{
    QString empty_string;
    QStringList time_formats_to_check = {"hh:mm:ss:zzz", "hh:mm:ss.zzz", "h:m:s:z", "h:m:s.z"};
    foreach(QString time_format, time_formats_to_check)
    {
        QTime timestamp_to_check_as_time = QTime::fromString(timestamp_to_check, time_format);
        if(timestamp_to_check_as_time.isValid())
            return time_format;
    }
    return empty_string;;
}

QStringList CsvDataImportAndExportManager::get_first_three_rows(QTextStream *dataset_file_text_stream, QString dataset_file_path)
{
    QStringList first_three_rows;
    QString sender_name = "CsvDataImportAndExportManager";
    Log log;

    for(int i = 0; i < 3; i ++)
    {
        if(!dataset_file_text_stream -> atEnd())
            first_three_rows.append(dataset_file_text_stream->readLine());
        else
        {
            log.importance_level = enum_message_importance_level::WARNING;
            log.message = "Selected file: " + dataset_file_path + " does not contain at least three rows of data and thus could not"
                                                                  " be loaded into dataset";
            emit signal_log_message(log, sender_name);
            return first_three_rows;
        }
    }
    return first_three_rows;
}
