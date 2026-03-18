#ifndef UNTITLED_SQMMODEL_H
#define UNTITLED_SQMMODEL_H
#include <QAbstractTableModel>
#include <qvector.h>
#include <set>
#include <utility>

struct SQMTableRow {
    bool binary_digit;
    long square_res;
    long mul_res;
};

class SqmModel : public QAbstractTableModel {
    Q_OBJECT
    QVector<SQMTableRow> sqm_QTable;

    int base;
    int exponent;
    int modulus;

    std::set<std::pair<int,int>> errorCells;

public:
    SqmModel(QObject *parent = nullptr);
    void calculateSQMTable(int base, int exponent, int modulus);
    void calculateSQMTableWithError(const QModelIndex &index, int base, int exponent, int modulus);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    signals:
        void editCompleted(const QString &);
};


#endif //UNTITLED_SQMMODEL_H