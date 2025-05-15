#include "../Headers/home.h"
#include "ui_home.h"
#include <QSvgWidget>
#include <QVBoxLayout>
#include "ui_requesttask.h"


Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , profile(nullptr)
    , requestTask(nullptr)
    , offeredTasks(nullptr)
{
    ui->setupUi(this);
    QSvgWidget *svgWidget = new QSvgWidget(":/new/svgs/Group.svg");
    svgWidget->setFixedSize(58, 58);
    QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(ui->profile->layout());
    if (layout) {
        layout->insertWidget(0, svgWidget);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile);
        newLayout->addWidget(svgWidget);
        ui->profile->setLayout(newLayout);
    }
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
    offeredTasks->show();
}

void Home::on_profile_clicked()
{
    profile = new Profile(this);
    profile->show();
}

void Home::on_request_clicked()
{
    requestTask = new RequestTask(this);
    requestTask->show();
}

void Home::on_logout_clicked()
{
    emit backToMainWindow();
    this->close();
}


void Home::on_pushButton_clicked()
{

}

