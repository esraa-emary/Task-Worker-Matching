#include "../Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "../Headers/login.h"
#include "../Headers/signup.h"
#include "hoverbutton.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , login(nullptr)
    , signup(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete login;
    delete signup;
}

void MainWindow::on_login_clicked()
{
    login = new LogIn(this);
    login->show();
}

void MainWindow::on_signup_clicked()
{
    signup = new SignUp(this);
    signup->show();
}
