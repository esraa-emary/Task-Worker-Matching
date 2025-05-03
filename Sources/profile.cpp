#include "../Headers/profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Profile)
{
    ui->setupUi(this);
}

Profile::~Profile()
{
    delete ui;
}

void Profile::on_back_clicked()
{
    emit backToHome();
    this->close();
}
