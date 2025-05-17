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
    int clientId;
    QString confirm,overAllFeedback,name,phone,address,email,password;

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

    clientId = query.value("clientID").toInt();
    name = query.value("name").toString();
    address = query.value("address").toString();
    overAllFeedback = query.value("overAllFeedback").toString();
    phone = query.value("phone").toString();

    QMessageBox::information(this, "Success", "logedin successfully!");

    // send data to home
    clientData.id = clientId;
    clientData.name = name;
    clientData.email = email;
    clientData.address = address;
    clientData.phone = phone;
    clientData.password = password;
    clientData.feedback = overAllFeedback;

    home = new Home(this);
    home->setClient( clientData.id, clientData.name, clientData.password, clientData.address, clientData.email, clientData.phone, clientData.feedback);
    home->show();
    this->close();
}

