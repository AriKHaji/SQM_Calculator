#ifndef UNTITLED_SQMMODEL_H
#define UNTITLED_SQMMODEL_H
#include <QAbstractTableModel>
#include <qvector.h>
#include <set>
#include <utility>

/**
 * @struct SQMTableRow
 * @brief Represents a row in the square-and-multiply table.
 *
 * Stores the current bit of the exponent as well as the intermediate
 * results for squaring and multiplying.
 */
struct SQMTableRow {
    bool current_bit;  ///< Current bit of the exponent
    long square_res;   ///< Result of the squaring operation
    long mul_res;      ///< Result of the multiplication
};

/**
 * @class SqmModel
 * @brief Qt table model for displaying the square-and-multiply table.
 *
 * This class calculates the SQM table and provides its data
 * to a QTableView.
 */
class SqmModel : public QAbstractTableModel {
    Q_OBJECT

    QVector<SQMTableRow> sqm_QTable;  ///< Stores the SQM table

    /// SQM parameters
    int base;
    int exponent;
    int modulus;

    std::set<std::pair<int, int>> errorCells;  ///< Stores the edited cells

public:
    SqmModel(QObject *parent = nullptr);

    /**
     * @brief Calculates the complete SQM table.
     * @param base The base.
     * @param exponent The exponent.
     * @param modulus The modulus.
     */
    void calculateSQMTable(int base, int exponent, int modulus);

    /**
     * @brief Recalculates all dependent entries after a modified cell.
     * @param index The index of the edited cell, including its row and column.
     * @param base The base.
     * @param exponent The exponent.
     * @param modulus The modulus.
     */
    void calculateSQMTableWithError(const QModelIndex &index, int base, int exponent, int modulus);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};


#endif //UNTITLED_SQMMODEL_H