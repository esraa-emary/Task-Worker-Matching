#include "../Headers/signup.h"
#include "ui_signup.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>

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

void SignUp::on_signup_clicked()
{
    int clientId;
    QString confirm,overAllFeedback="",fName,lName,name,phone,address,email,password;

    QSqlQuery query1;
    if (!query1.exec("SELECT MAX(clientID) FROM client")) {
        QMessageBox::critical(this, "Error", "Database error: " + query1.lastError().text());
        return;
    }

    if (query1.next()) {
        QVariant maxIdVar = query1.value(0);
        if (maxIdVar.isValid() && !maxIdVar.isNull()) {
            clientId = maxIdVar.toInt() + 1;
        }
        else clientId = 1;
    }

    fName = ui->firstName->text();
    lName = ui->lastName->text();
    name = fName + " " +lName;

    phone = ui->phone->text();
    address = ui->address->text();
    email = ui->email->text();
    password = ui->password->text();
    confirm = ui->confirm->text();

    // name
    if (fName.isEmpty() ||lName.isEmpty() ) {
        QMessageBox::warning(this, "Error", "name is empty.");
        return;
    }

    QSqlQuery query2;
    if (!query2.exec("SELECT email FROM client")) {
        QMessageBox::critical(this, "Error", "Database error: " + query2.lastError().text());
        return;
    }

    while (query2.next()) {
        QString existingEmail = query2.value(0).toString().trimmed();
        if (existingEmail.compare(email, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "Email Exists", "This email is already registered.");
            return;
        }
    }

    // Email format
    QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    if (!emailRegex.match(email).hasMatch() || email.isEmpty()) {
        QMessageBox::warning(this, "Invalid Email", "Please enter a valid email address.");
        return;
    }

    // Phone must be digits only
    QRegularExpression phoneRegex(R"(^\d+$)");
    if (!phoneRegex.match(phone).hasMatch() || phone.isEmpty()) {
        QMessageBox::warning(this, "Invalid Phone", "Phone number must contain digits only.");
        return;
    }

    // address
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Error", "address is empty.");
        return;
    }

    // Password match
    if (password != confirm || password.isEmpty()) {
        QMessageBox::warning(this, "Password Mismatch", "Passwords do not match.");
        return;
    }

    QSqlQuery query3;
    query3.prepare("INSERT INTO client (clientId, name, phone, address, "
                  "email, password) "
                  "VALUES (:clientId, :name, :phone, :address, "
                  ":email, :password)");

    query3.bindValue(":clientId", clientId);
    query3.bindValue(":name", name);
    query3.bindValue(":phone", phone);
    query3.bindValue(":address", address);
    query3.bindValue(":email", email);
    query3.bindValue(":password", password);

    while (!query3.exec()) {
        QMessageBox::critical(this, "Error", "Failed to save request: " + query3.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", "signedup successfully!");

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

