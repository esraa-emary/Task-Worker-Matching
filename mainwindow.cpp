#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_exit_clicked()
{
    this->deleteLater();
}


void MainWindow::on_login_clicked()
{
    this->login.showFullScreen();
    this->hide();
}


void MainWindow::on_signup_clicked()
{
    this->signup.showFullScreen();
    this->hide();
}

