#include "clientsection.h"
#include "ui_clientsection.h"

ClientSection::ClientSection(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientSection)
{
    ui->setupUi(this);
}

ClientSection::~ClientSection()
{
    delete ui;
}
