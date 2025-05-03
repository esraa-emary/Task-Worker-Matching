#include "../Headers/home.h"
#include "ui_home.h"

Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , profile(nullptr)
    , requestTask(nullptr)
    , offeredTasks(nullptr)
{
    ui->setupUi(this);
}

Home::~Home()
{
    delete ui;
    delete profile;
    delete requestTask;
    delete offeredTasks;
}

void Home::on_offeredTasks_clicked()
{
    offeredTasks = new OfferedTasks(this);
    offeredTasks->showFullScreen();
}

void Home::on_profile_clicked()
{
    profile = new Profile(this);
    profile->showFullScreen();
}

void Home::on_request_clicked()
{
    requestTask = new RequestTask(this);
    requestTask->showFullScreen();
}

void Home::on_logout_clicked()
{
    emit backToMainWindow();
    this->close();
}

