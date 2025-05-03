#include "../Headers/signup.h"
#include "ui_signup.h"

SignUp::SignUp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SignUp)
    , home(nullptr)
{
    ui->setupUi(this);
}

SignUp::~SignUp()
{
    delete ui;
    delete home;
}

void SignUp::on_back_clicked()
{
    emit backToMainWindow();
    this->close();
}

void SignUp::on_submit_clicked()
{
    home = new Home(this);
    home->show();
}

