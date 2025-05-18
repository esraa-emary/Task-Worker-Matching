#include "headers/requesttask.h"
#include "ui_requesttask.h"
#include "../Headers/home.h"
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

    QSqlDatabase::database().transaction();

    QSqlQuery query;
    if (!query.exec("SELECT MAX(requestId) FROM request")) {
        qDebug() << "Error getting max requestId:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Database error: " + query.lastError().text());
        QSqlDatabase::database().rollback();
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
        qDebug() << "Error fetching taskId for task:" << task;
        QMessageBox::critical(this, "Error", "Invalid task selected");
        QSqlDatabase::database().rollback();
        return;
    }
    int taskId = query.value(0).toInt();

    query.prepare(
        "INSERT INTO request (requestId, clientId, taskId, address, "
        "requestTime, preferredTimeSlot, requestDescription) "
        "VALUES (:requestId, :clientId, :taskId, :address, "
        ":requestTime, :preferredTimeSlot, :description)"
        );
    query.bindValue(":requestId", requestId);
    query.bindValue(":clientId", clientData.id);
    query.bindValue(":taskId", taskId);
    query.bindValue(":address", clientData.address);
    query.bindValue(":requestTime", QDateTime::currentDateTime().toTimeZone(QTimeZone("Europe/Athens")));
    query.bindValue(":preferredTimeSlot", preferredTimeSlots);
    query.bindValue(":description", description.isEmpty() ? QVariant(QVariant::String) : description);

    if (!query.exec()) {
        qDebug() << "Error inserting request:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to save request: " + query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    query.prepare(
        "SELECT w.WorkerID, w.Rating, COUNT(a.RequestID) as AssignmentCount "
        "FROM Worker w "
        "JOIN Specialty s ON w.WorkerID = s.WorkerID "
        "LEFT JOIN Assignment a ON w.WorkerID = a.WorkerID "
        "WHERE s.TaskID = :taskId "
        "GROUP BY w.WorkerID, w.Rating "
        "HAVING COUNT(a.RequestID) < 5 "
        "ORDER BY w.Rating DESC"
        );
    query.bindValue(":taskId", taskId);
    if (!query.exec()) {
        qDebug() << "Error finding eligible worker:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to find eligible worker: " + query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    int workerId = -1;
    if (query.next()) {
        workerId = query.value("WorkerID").toInt();
    } else {
        qDebug() << "No eligible worker found for taskId:" << taskId;
        QMessageBox::warning(this, "Warning", "No available worker found with the required specialty. Request added without assignment.");
        if (!QSqlDatabase::database().commit()) {
            qDebug() << "Transaction commit failed.";
            QMessageBox::critical(this, "Error", "Failed to commit transaction.");
            QSqlDatabase::database().rollback();
            return;
        }
        QMessageBox::information(this, "Success", "Request added successfully!");
        emit backToHome();
        this->close();
        return;
    }

    query.prepare(
        "INSERT INTO Assignment (WorkerID, ClientID, RequestID, Status) "
        "VALUES (:workerId, :clientId, :requestId, :status)"
        );
    query.bindValue(":workerId", workerId);
    query.bindValue(":clientId", clientData.id);
    query.bindValue(":requestId", requestId);
    query.bindValue(":status", "Pending");

    if (!query.exec()) {
        qDebug() << "Error inserting assignment:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to create assignment: " + query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    if (!QSqlDatabase::database().commit()) {
        qDebug() << "Transaction commit failed.";
        QMessageBox::critical(this, "Error", "Failed to commit transaction.");
        QSqlDatabase::database().rollback();
        return;
    }

    QMessageBox::information(this, "Success", "Request and assignment added successfully!");

    emit backToHome();
    this->close();
}
