#include "profileclient.h"
#include "ui_profileclient.h"

ProfileClient::ProfileClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProfileClient)
{
    ui->setupUi(this);
}

ProfileClient::~ProfileClient()
{
    delete ui;
}
