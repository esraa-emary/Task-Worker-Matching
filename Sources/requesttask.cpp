#include "../Headers/requesttask.h"
#include "ui_requesttask.h"

RequestTask::RequestTask(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RequestTask)
{

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

