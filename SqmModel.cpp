#include "SqmModel.h"
#include <bitset>
#include <qbrush.h>
#include <qfont.h>

std::string decToBin(int dec) {
    std::string bin = std::bitset<32>(dec).to_string();
    bin.erase(0, bin.find_first_not_of('0'));
    return bin;
}

int getIthBitOfExponent(const std::string &exponent_bin, int i) {
    return exponent_bin[i] - '0';
}

SqmModel::SqmModel(QObject *parent)
    : QAbstractTableModel(parent) {

}

int SqmModel::rowCount(const QModelIndex & /*parent*/) const {
    return this->sqm_QTable.size();
}

int SqmModel::columnCount(const QModelIndex & /*parent*/) const {
    return 3;
}

void SqmModel::calculateSQMTable(int base, int exponent, int modulus) {
    beginResetModel();
    sqm_QTable.clear();
    this->errorCells.clear();
    this->base = base;
    this->exponent = exponent;
    this->modulus = modulus;

    std::string exponent_bin = decToBin(exponent);

    for (int i = 0; i < exponent_bin.size(); i++) {
        SQMTableRow row;
        row.binary_digit = getIthBitOfExponent(exponent_bin, i);
        if (i == 0) {
            row.square_res = 1;
            row.mul_res = base % modulus;
        } else {
            row.square_res = sqm_QTable[i - 1].mul_res * sqm_QTable[i - 1].mul_res % modulus;
            row.mul_res = row.binary_digit ? (row.square_res * base % modulus) : row.square_res;
        }
        sqm_QTable.append(row);
    }
    endResetModel();
}

void SqmModel::calculateSQMTableWithError(const QModelIndex &index, int base, int exponent, int modulus) {
    beginResetModel();

    int i = index.row();

    switch (index.column()) {
        case 0:
            sqm_QTable[i].square_res = sqm_QTable[i - 1].mul_res * sqm_QTable[i - 1].mul_res % modulus;
            sqm_QTable[i].mul_res = sqm_QTable[i].binary_digit
                ? (sqm_QTable[i].square_res * base % modulus)
                : sqm_QTable[i].square_res;
            break;

        case 1:
            sqm_QTable[i].mul_res = sqm_QTable[i].binary_digit
                ? (sqm_QTable[i].square_res * base % modulus)
                : sqm_QTable[i].square_res;
            break;

        case 2:
            break;
    }

    std::string exponent_bin = decToBin(exponent);

    for (int j = i + 1; j < exponent_bin.size(); j++) {
        sqm_QTable[j].binary_digit = getIthBitOfExponent(exponent_bin, j);
        sqm_QTable[j].square_res = sqm_QTable[j - 1].mul_res * sqm_QTable[j - 1].mul_res % modulus;
        sqm_QTable[j].mul_res = sqm_QTable[j].binary_digit
            ? (sqm_QTable[j].square_res * base % modulus)
            : sqm_QTable[j].square_res;
    }

    endResetModel();
}


QVariant SqmModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::fromStdString(std::to_string(sqm_QTable[index.row()].binary_digit));
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

        bool ok;
        int n = value.toInt(&ok);

        if (!ok || 0 > n || n > 99999 || (index.column() == 0 && n > 1))
            return false;

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

        switch (index.column()) {
            case 0:
                this->sqm_QTable[index.row()].binary_digit = n;
                calculateSQMTableWithError(index, base, exponent, modulus);
                break;
            case 1:
                this->sqm_QTable[index.row()].square_res = n;
                calculateSQMTableWithError(index, base, exponent, modulus);
                break;
            case 2:
                this->sqm_QTable[index.row()].mul_res = n;
                calculateSQMTableWithError(index, base, exponent, modulus);
                break;
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

Qt::ItemFlags SqmModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}