#include "../Headers/login.h"
#include "ui_login.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

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

void LogIn::on_login_clicked()
{
    QString email;
    QString password;

    email = ui->email->text();
    password = ui->password->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM client WHERE email = :email AND password = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    if (!query.next()) {
        QMessageBox::information(this, "Login Failed", "Invalid email or password.");
        return;
    }

    if (query.next()) {
        int clientId = query.value("clientID").toInt();
        QString name = query.value("name").toString();
        QString address = query.value("address").toString();
    }

    QMessageBox::information(this, "Success", "logedin successfully!");
    home = new Home(this);
    home->show();
}

