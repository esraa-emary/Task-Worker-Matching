#include "profileworker.h"
#include "ui_profileworker.h"

ProfileWorker::ProfileWorker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProfileWorker)
{
    ui->setupUi(this);
}

ProfileWorker::~ProfileWorker()
{
    delete ui;
}
