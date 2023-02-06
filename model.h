#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>
#include <QFont>
#include <QBrush>

class Model : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Model(const QStringList &rows, QObject *parent = nullptr);
    ~Model() = default;

    //Methods required to be implemented by QAbstractTableModel

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QStringList data_rows;
    int columns_count = 0;
    int rows_count = 0;
    bool headers_present = false;
signals:
    void editCompleted(const QString &);

};

#endif // MODEL_H
