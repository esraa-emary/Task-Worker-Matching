#include "workersection.h"
#include "ui_workersection.h"

WorkerSection::WorkerSection(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WorkerSection)
{
    ui->setupUi(this);
}

WorkerSection::~WorkerSection()
{
    delete ui;
}
