#ifndef READONLYDATASET_H
#define READONLYDATASET_H

#include <QObject>

/**
 * Informations about dataset signal. To be extended.
 */
struct SignalInfo
{
    QString name;
    QString unit;
    double min_value;
    double max_value;

    /** Default, empty constructor */
    explicit SignalInfo(){}

    /** Constructor with just the signal name. Useful when starting dataset creation in DataImportAndExportManager. */
    SignalInfo(QString signal_name)
    {
        this -> name = signal_name;
    }

    /** Full constructor */
    SignalInfo(QString signal_name, QString unit, double min_value, double max_value)
    {
        this -> name = signal_name;
        this -> unit = unit;
        this -> min_value = min_value;
        this -> max_value = max_value;
    }

    virtual ~SignalInfo(){}
};
Q_DECLARE_METATYPE(SignalInfo)

/**
 * Base dataset class which does not allow creation or modification of dataset.
 * It is to be used only as an access point to data.
 */
class ReadOnlyDataset: public QObject
{
    Q_OBJECT
public:
    //Public methods
    virtual ~ReadOnlyDataset();

    //Getters for dataset informations
    bool get_timestamps_present() const;
    int get_rows_count() const;
    int get_columns_count() const;
    double get_sampling_time_in_seconds () const;
    QString get_dataset_name() const;

    ////Getters for dataset containers/values
    //Container getters
    QVector<QVector<double>*>* get_dataset_signals_container() const;
    /**
     * Returns a pointer to timestamps container. If timestamps are not present it will return nullptr.
     * This case should be handled by the caller.
     */
    QVector<QString>* get_dataset_timestamps_container() const;

    //Individual value getters
    SignalInfo get_signal_info(int signal_index) const;
    double get_sample_value(int signal_index, int row_index);
    QString get_timestamp(int row_index);

protected:
    ////Private attributes
    //Dataset containers
    /**
     * Main dataset container.
     */
    QVector<QVector<double>*> *data_columns = nullptr;
    /**
     * Timestamps container.
     * TODO: create timestamps format
     */
    QVector<QString> *timestamps_column = nullptr;

    //Additional dataset informations
    //Timestamp shall not be treated as a signal but is appended to signals_info just to make getting headers less complicated.
    QList<SignalInfo> signals_info;

    /**
     * Data sampling time in seconds - either automatically calculated from timestamps during dataset loading or provided by the user.
     */
    double sampling_time_in_seconds = 0;
    /**
     * ReadOnlyDataset name - extracted from file name during dataset loading or provided by the user.
     */
    QString dataset_name = "";

    int columns_count = 0;
    int rows_count = 0;
    bool timestamps_present = false;
};

#endif // READONLYDATASET_H
