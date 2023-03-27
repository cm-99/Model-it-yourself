#ifndef CSVDATAIMPORTANDEXPORTMANAGER_H
#define CSVDATAIMPORTANDEXPORTMANAGER_H

#include "Interfaces/dataimportandexportmanager.h"

#include <QString>
#include <QObject>
#include <QFile>

/**
 * DataImportAndExportManager implementation specialized in handling *.csv files.
 * At the moment it handles comma or tab separated files.
 */
class CsvDataImportAndExportManager: public DataImportAndExportManager
{
    Q_OBJECT
public:
    CsvDataImportAndExportManager();

    /**
     * Returs file format supported by CsvDataImportAndExportManager as QString.
     * To be used by CoreController to organize supported file formats.
     */
    QString get_supported_format() override;
    /**
     * Starts method depending on passed @param task_index.
     * 0 - load_dataset
     * 1 - save_dataset
     * To be used by CoreController to invoke dataset loading/saving in a new thread.
     */
    void start_background_task(int task_index) override;

    //See DataExportAndImportManager for generic descriptions. Details can be found in private section under extracted methods.
    void load_dataset() override;
    void save_dataset() override;
    void set_dataset_file_path(QString dataset_file_path) override;

private:
    //Methods extracted from load_dataset for better readability

    /**
     * Load first three rows of dataset needed for further dataset validity checks.
     * @return - returns QStringList with three first dataset rows if possible. Otherwise, returns empty list to be handled by caller.
     */
    QStringList get_first_three_rows(QTextStream *dataset_file_text_stream, QString dataset_file_path);
    /**
     * Attemps to calculate dataset's data sampling time in miliseconds based on two consecutive timestamps.
     * @return - if timestamps are valid and consecutive (presumably_next_timestamp - presumably_timestamp > 0),
     *  returns sampling time in miliseconds. Otherwise, returns 0.
     */
    int get_dataset_sampling_time_in_miliseconds(QString presumably_timestamp, QString presumably_next_timestamp);
    /**
     * Checks if provided QString can be recognized as valid timestamp - it contains part that can be converted to valid QTime object.
     * @param presumably_timestamp - dataset cell to be checked.
     * @return - if presumably_timestamp can be converted to valid QTime object, returns QPair<int, QString> where:
     * int - 0 or 1, depending if presumably_timestamp contains DateTime or Time alone.
     * QString - time format.
     * Otherwise, returns QPair with int member equal to -1.
     */
    QPair<int, QString> check_timestamps_validity(QString presumably_timestamp);
    /**
     * Checks if given timestamp_to_check can be converted to valid QTime object using predefined time formats.
     * @return - returns time format if resulting QTime is valid. Otherwise, returns empty QString.
     */
    QString check_timestamps_compatiblity_with_time_formats(QString timestamp_to_check);
    /**
     * Loads dataset into prepared containers inside @param dataset through @param dataset_file_text_stream.
     * Detected timestamps are loaded into separate container.
     */
    void load_dataset_with_timestamps_into_containers(char separator, Dataset_TableModel *dataset, QTextStream *dataset_file_text_stream);
    /**
     * Loads dataset into prepared containers inside @param dataset through @param dataset_file_text_stream.
     */
    void load_dataset_without_timestamps_into_containers(char separator, Dataset_TableModel *dataset, QTextStream *dataset_file_text_stream);
    /**
     * Loads headers from dataset file if those are found in the @param first_row.
     * Otherwise, prepares default headers. Headers are saved in @param dataset.
     * @return - returns true if headers were found in the file. Otherwise, returns false.
     */
    bool prepare_dataset_headers(Dataset_TableModel *dataset, char separator, QString first_row);

    //Methods extracted from save_dataset
};

#endif // CSVDATAIMPORTANDEXPORTMANAGER_H
