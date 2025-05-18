#include "../Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "../Headers/login.h"
#include "../Headers/signup.h"
#include "hoverbutton.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

#include <QTimeZone>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>

bool connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    QString serverName = ".";
    QString dbName = "TaskWorkerMatching";

    QString connectionString = QString(
                                   "Driver={ODBC Driver 17 for SQL Server};"
                                   "Server=%1;"
                                   "Database=%2;"
                                   "Trusted_Connection=yes;"
                                   "Encrypt=yes;"
                                   "TrustServerCertificate=yes;"
                                   ).arg(serverName).arg(dbName);

    db.setDatabaseName(connectionString);

    if (!db.open()) {
        qDebug() << "Error connecting to database:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connected to database successfully";
    qDebug() << "System timezone:" << QTimeZone::systemTimeZoneId();
    return true;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , login(nullptr)
    , signup(nullptr)
{
    ui->setupUi(this);


    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Connection Error",
                              "Failed to connect to the database.");
    }
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
