#include "offeredtasks.h"
#include "ui_offeredtasks.h"

OfferedTasks::OfferedTasks(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OfferedTasks)
{
    ui->setupUi(this);
}

OfferedTasks::~OfferedTasks()
{
    delete ui;
}
