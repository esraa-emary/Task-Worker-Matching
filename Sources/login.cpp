#include "../Headers/login.h"
#include "ui_login.h"

LogIn::LogIn(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LogIn)
    , home(nullptr)
{
    ui->setupUi(this);
}

LogIn::~LogIn()
{
    delete ui;
    delete home;
}

void LogIn::on_back_clicked()
{
    emit backToMainWindow();
    this->close();
}

void LogIn::on_submit_clicked()
{
    home = new Home(this);
    home->show();
}

