#include "SqmModel.h"
#include <bitset>
#include <qbrush.h>
#include <qfont.h>


/// Helper Functions

/**
 * @brief Converts a decimal integer to its binary representation.
 *
 * Leading zeros are removed. If the input is 0, the function returns "0".
 *
 * @param dec The decimal integer to convert.
 * @return A string containing the binary representation of the input value.
 */
std::string decToBin(int dec) {
    std::string bin = std::bitset<32>(dec).to_string();
    size_t first_one = bin.find_first_not_of('0');
    return (first_one == std::string::npos) ? "0" : bin.substr(first_one);
}

/**
 * @brief Returns the i-th bit of the binary exponent string.
 *
 * @param exponent_bin The binary representation of the exponent.
 * @param i The zero-based index of the bit to return.
 * @return The bit at position i as an integer value (0 or 1).
 *
 * @pre i must be a valid index in exponent_bin.
 */
int getIthBitOfExponent(const std::string &exponent_bin, int i) {
    return exponent_bin[i] - '0';
}

SqmModel::SqmModel(QObject *parent)
    : QAbstractTableModel(parent) {

}

void SqmModel::calculateSQMTable(int base, int exponent, int modulus) {
    beginResetModel();
    sqm_QTable.clear();
    this->errorCells.clear();

    // Set the parameters in the model object
    this->base = base;
    this->exponent = exponent;
    this->modulus = modulus;

    std::string exponent_bin = decToBin(exponent);

    // Calculate the table
    for (int i = 0; i < exponent_bin.size(); i++) {
        SQMTableRow row;
        row.current_bit = getIthBitOfExponent(exponent_bin, i);
        if (i == 0) {
            row.square_res = 1;
            row.mul_res = base % modulus;
        } else {
            row.square_res = sqm_QTable[i - 1].mul_res * sqm_QTable[i - 1].mul_res % modulus;
            row.mul_res = row.current_bit ? (row.square_res * base % modulus) : row.square_res;
        }
        sqm_QTable.append(row);
    }
    endResetModel();
}

void SqmModel::calculateSQMTableWithError(const QModelIndex &index, int base, int exponent, int modulus) {
    beginResetModel();

    int i = index.row();

    // Recalculate the row of the edited cell
    // Note that, only the cells that come after the edited cell have to be recalculated
    switch (index.column()) {
        case 0:
            if (i == 0) {
                sqm_QTable[i].square_res = 1;
            } else {
                sqm_QTable[i].square_res = sqm_QTable[i - 1].mul_res * sqm_QTable[i - 1].mul_res % modulus;
            }
            sqm_QTable[i].mul_res = sqm_QTable[i].current_bit
                ? (sqm_QTable[i].square_res * base % modulus)
                : sqm_QTable[i].square_res;
            break;

        case 1:
            sqm_QTable[i].mul_res = sqm_QTable[i].current_bit
                ? (sqm_QTable[i].square_res * base % modulus)
                : sqm_QTable[i].square_res;
            break;

        case 2:
            break;
    }

    std::string exponent_bin = decToBin(exponent);

    // Recalculate all other rows
    for (int j = i + 1; j < exponent_bin.size(); j++) {
        sqm_QTable[j].current_bit = getIthBitOfExponent(exponent_bin, j);
        sqm_QTable[j].square_res = sqm_QTable[j - 1].mul_res * sqm_QTable[j - 1].mul_res % modulus;
        sqm_QTable[j].mul_res = sqm_QTable[j].current_bit
            ? (sqm_QTable[j].square_res * base % modulus)
            : sqm_QTable[j].square_res;
    }

    endResetModel();
}

int SqmModel::rowCount(const QModelIndex & /*parent*/) const {
    return this->sqm_QTable.size();
}

int SqmModel::columnCount(const QModelIndex & /*parent*/) const {
    return 3;
}

QVariant SqmModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::fromStdString(std::to_string(sqm_QTable[index.row()].current_bit));
            case 1:
                return QString::fromStdString(std::to_string(sqm_QTable[index.row()].square_res));
            case 2:
                return QString::fromStdString(std::to_string(sqm_QTable[index.row()].mul_res));
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case 0:
                return int(Qt::AlignCenter);
            case 1:
            case 2:
                return int(Qt::AlignCenter | Qt::AlignRight);
        }
    }

    if (role == Qt::ForegroundRole) {
        if (errorCells.contains(std::pair(index.row(), index.column()))) {
            return QBrush(Qt::red);
        }
    }

    return QVariant();
}

QVariant SqmModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return QString("BIN");
            case 1:
                return QString("SQ");
            case 2:
                return QString("MUL");
        }
    }
    return QVariant();
}

bool SqmModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;

        // Check whether the entered value is valid
        bool ok;
        const int n = value.toInt(&ok);
        if (!ok || n < 0 || n > 99999)
            return false;
        if (index.column() == 0 && n > 1)
            return false;

        // Track the edited cells in a table.
        // Remove all edited cells that come after the current one,
        // because they are recalculated now.
        errorCells.insert(std::pair(index.row(), index.column()));
        for (auto it = errorCells.begin(); it != errorCells.end(); ) {
            int row = it->first;
            int column = it->second;

            if (row > index.row() || (row == index.row() && column > index.column())) {
                it = errorCells.erase(it);
            } else {
                ++it;
            }
        }

        // Set the value of the edited cell in the sqm table
        switch (index.column()) {
            case 0:
                this->sqm_QTable[index.row()].current_bit = n;
                break;
            case 1:
                this->sqm_QTable[index.row()].square_res = n;
                break;
            case 2:
                this->sqm_QTable[index.row()].mul_res = n;
                break;
        }

        // Recalculate the rest of the table
        calculateSQMTableWithError(index, base, exponent, modulus);

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

// Based on an example from: https://arc.net/l/quote/kffyrcct
Qt::ItemFlags SqmModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}