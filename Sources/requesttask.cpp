#include "headers/requesttask.h"
#include "ui_requesttask.h"
#include "../Headers/home.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

RequestTask::RequestTask(QMainWindow *parent,int taskId,QString taskName)
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
    query.prepare(R"(
    SELECT DISTINCT WORKER.NAME
    FROM WORKER
    JOIN SPECIALTY ON WORKER.WORKERID = SPECIALTY.WORKERID
    JOIN TASK ON TASK.TASKID = SPECIALTY.TASKID
    WHERE TASK.TASKID = :taskId
)");

    query.bindValue(":taskId", taskId);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch workers:\n" + query.lastError().text());
        return;
    }

    ui->comboBox->clear();

    while (query.next()) {
        QString workerName = query.value(0).toString();  // Use index 0 for performance
        ui->comboBox->addItem(workerName);
    }

    ui->taskName->setText(taskName);
}

RequestTask::~RequestTask()
{
    delete ui;
}

void RequestTask::on_cancel_clicked()
{
    this->close();
}

void RequestTask::on_back_clicked()
{
    this->close();
}

void RequestTask::on_add_clicked()
{
    // Validate task selection
    if (ui->comboBox->currentIndex() == -1) {
        QMessageBox::warning(this, "Input Error", "Please select a worker.");
        return;
    }

    // Get form data
    QString description = ui->description->text();
    QDateTime preferredTimeSlots = ui->preferredTimeSlots->dateTime();

    // Get next request ID
    int requestId = 1;
    QSqlQuery maxIdQuery;
    if (!maxIdQuery.exec("SELECT MAX(requestId) FROM request")) {
        QMessageBox::critical(this, "Database Error", "Could not get request ID:\n" + maxIdQuery.lastError().text());
        return;
    }
    if (maxIdQuery.next()) {
        QVariant maxId = maxIdQuery.value(0);
        if (maxId.isValid() && !maxId.isNull()) {
            requestId = maxId.toInt() + 1;
        }
    }

    // Insert into request table
    QSqlQuery insertQuery;
    insertQuery.prepare(R"(
        INSERT INTO request (requestId, clientId, taskId, address, requestTime, preferredTimeSlot, requestDescription)
        VALUES (:requestId, :clientId, :taskId, :address, :requestTime, :preferredTimeSlot, :description)
    )");

    insertQuery.bindValue(":requestId", requestId);
    insertQuery.bindValue(":clientId", clientData.id);
    insertQuery.bindValue(":taskId", clientData.taskId);
    insertQuery.bindValue(":address", clientData.address);
    insertQuery.bindValue(":requestTime", QDateTime::currentDateTime());
    insertQuery.bindValue(":preferredTimeSlot", preferredTimeSlots);
    insertQuery.bindValue(":description", description);

    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Insert Error", "Failed to save request:\n" + insertQuery.lastError().text());
        return;
    }

    // Get selected worker's ID
    QString workerName = ui->comboBox->currentText();
    QSqlQuery workerQuery;
    workerQuery.prepare("SELECT workerId FROM worker WHERE NAME = :workerName");
    workerQuery.bindValue(":workerName", workerName);

    if (!workerQuery.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to get worker ID:\n" + workerQuery.lastError().text());
        return;
    }

    if (!workerQuery.next()) {
        QMessageBox::critical(this, "Error", "Selected worker not found in the database.");
        return;
    }

    int workerId = workerQuery.value(0).toInt();

    // Insert into assignment table
    QSqlQuery assignmentQuery;
    assignmentQuery.prepare(R"(
        INSERT INTO assignment (workerId, clientId, requestId, status)
        VALUES (:workerId, :clientId, :requestId, 'In Progress')
    )");

    assignmentQuery.bindValue(":workerId", workerId);
    assignmentQuery.bindValue(":clientId", clientData.id);
    assignmentQuery.bindValue(":requestId", requestId);

    if (!assignmentQuery.exec()) {
        QMessageBox::critical(this, "Insert Error", "Failed to create assignment:\n" + assignmentQuery.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", "Request and assignment added successfully!");
    this->close();
}
