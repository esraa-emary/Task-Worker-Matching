#include "headers/requesttask.h"
#include "ui_requesttask.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

RequestTask::RequestTask(QMainWindow *parent)
    : QMainWindow(parent)
    , ui(new Ui::RequestTask)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        QString serverName = "HEFNY";
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
            QMessageBox::critical(this, "Database Error",
                                  "Failed to connect to database: " + db.lastError().text());
            return;
        }
    }

    QSqlQuery query;
    query.prepare("SELECT taskName FROM task");
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch tasks: " + query.lastError().text());
        return;
    }

    while (query.next()) {
        QString task = query.value("taskName").toString();
        ui->comboBox->addItem(task);
    }
}

RequestTask::~RequestTask()
{
    delete ui;
}

void RequestTask::on_cancel_clicked()
{
    emit backToHome();
    this->close();
}

void RequestTask::on_back_clicked()
{
    emit backToHome();
    this->close();
}

void RequestTask::on_add_clicked()
{
    if (ui->comboBox->currentIndex() == -1) {
        QMessageBox::warning(this, "Error", "Please select a task!");
        return;
    }

    QDateTime startDate = ui->startDate->dateTime();
    QDateTime endDate = ui->endDate->dateTime();

    if (startDate >= endDate) {
        QMessageBox::warning(this, "Error", "End time must be after start time!");
        return;
    }

    QString description = ui->description->text();
    QString task = ui->comboBox->currentText();
    QDateTime preferredTimeSlots = ui->preferredTimeSlots->dateTime();

    QSqlQuery query;
    if (!query.exec("SELECT MAX(requestId) FROM request")) {
        QMessageBox::critical(this, "Error", "Database error: " + query.lastError().text());
        return;
    }

    int requestId = 1;
    if (query.next()) {
        QVariant maxIdVar = query.value(0);
        if (maxIdVar.isValid() && !maxIdVar.isNull()) {
            requestId = maxIdVar.toInt() + 1;
        }
    }

    query.prepare("SELECT taskId FROM task WHERE taskName = :task");
    query.bindValue(":task", task);
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Invalid task selected");
        return;
    }
    int taskId = query.value(0).toInt();

    query.prepare("INSERT INTO request (requestId, clientId, taskId, address, "
                  "requestTime, preferredTimeSlot, requestDescription) "
                  "VALUES (:requestId, :clientId, :taskId, :address, "
                  ":requestTime, :preferredTimeSlot, :description)");

    query.bindValue(":requestId", requestId);
    query.bindValue(":clientId", 1);
    query.bindValue(":taskId", taskId);
    query.bindValue(":address", "");
    query.bindValue(":requestTime", QDateTime::currentDateTime());
    query.bindValue(":preferredTimeSlot", preferredTimeSlots);
    query.bindValue(":description", description);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to save request: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", "Request added successfully!");
    emit backToHome();
    this->close();
}
