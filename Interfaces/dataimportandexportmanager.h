#ifndef DATAIMPORTANDEXPORTMANAGER_H
#define DATAIMPORTANDEXPORTMANAGER_H

#include "Interfaces/backgroundtaskenabledobject.h"
#include "Essenstials/dataset_tablemodel.h"

/**
 * Interface to be inherited by all classes performing dataset loading and/or saving.
 * Proper implementation allows the CoreController to automatically create list of supported formats and invoke load/save_dataset method
 * of suitable class when user chooses a file in one of those formats.
 *
 * Each implementation's object needs to be manually initialized and added to container in the CoreController.
 */
class DataImportAndExportManager : public BackgroundTaskEnabledObject
{
    Q_OBJECT
public:
    virtual ~DataImportAndExportManager(){};

    virtual QString get_supported_format() = 0;

    /**
     * To be implemented as dataset loading from file.
     * It should be consistent with file format specified by supported_format.
     */
    virtual void load_dataset() = 0;
    /**
     * To be implemented as dataset saving to file.
     * It should be consistent with file format specified by supported_format.
     */
    virtual void save_dataset() = 0;
    /**
     * To be implemented as dataset_file_path setter method.
     */
    virtual void set_dataset_file_path(QString dataset_file_path) = 0;

protected:
    /**
     * Absoulute path to file used in dataset loading/saving.
     */
    QString dataset_file_path;
    /**
     * Format supported by particular child class of DataImportAndExportManager.
     */
    QString supported_format;

signals:
    /**
     * Signal to be connected to slot_change_active_dataset of CoreController.
     * Must be emitted at the end of load_dataset method if currently used dataset is to be changed.
     * @param loaded_dataset - dataset created and filled with data by load_dataset method.
     */
    void signal_dataset_loaded(Dataset_TableModel *loaded_dataset);
};

#endif // DATAIMPORTANDEXPORTMANAGER_H
