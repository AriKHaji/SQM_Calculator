#ifndef UNTITLED_MAINWINDOW_H
#define UNTITLED_MAINWINDOW_H

#include <QWidget>

#include "SqmModel.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT
    SqmModel *sqmModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    Ui::MainWindow *ui;
    int getModulus();
    int getExponent();
    int getBase();
};


#endif //UNTITLED_MAINWINDOW_H