#include "../Headers/requesttask.h"
#include "ui_requesttask.h"

RequestTask::RequestTask(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RequestTask)
{
    ui->setupUi(this);
}

RequestTask::~RequestTask()
{
    delete ui;
}

void RequestTask::on_back_clicked()
{
    emit backToHome();
    this->close();
}


void RequestTask::on_submit_clicked()
{
    emit backToHome();
    this->close();
}

