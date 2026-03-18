#include "mainwindow.h"
#include "SqmModel.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("SQM Calculator");

    this->sqmModel = new SqmModel;
    ui->sqmTable->setModel(sqmModel);

    connect(ui->calculateButton, &QPushButton::clicked, [this] {sqmModel->calculateSQMTable(getBase(), getExponent(), getModulus()); ui->sqmTable->setModel(sqmModel);} );

    // Styling - my ideas, realized with help of ChatGPT
    setFixedWidth(width());

    ui->calculateButton->setMinimumSize(140, 64);

    ui->calculateButton->setCursor(Qt::PointingHandCursor);

    ui->sqmTable->verticalHeader()->hide();
    ui->sqmTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->sqmTable->setShowGrid(false);
    ui->sqmTable->setFrameShape(QFrame::NoFrame);
    ui->sqmTable->setFont(QFont("Menlo", 14));
}

MainWindow::~MainWindow() {
    delete ui;
}

int MainWindow::getBase() {
    return std::stoi(ui->baseInput->text().toStdString());
}

int MainWindow::getExponent() {
    return std::stoi(ui->exponentInput->text().toStdString());
}

int MainWindow::getModulus() {
    return std::stoi(ui->modulusInput->text().toStdString());
}
