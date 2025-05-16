#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "offeredtasks.h"
#include "profile.h"
#include "requesttask.h"
#include <QFrame>
#include <QSqlQuery>
#include <QSqlError>
namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

signals:
    void backToMainWindow();

private slots:
    void on_offeredTasks_clicked();

    void on_profile_clicked();

    void on_request_clicked();

    void on_logout_clicked();

    void on_pushButton_clicked();

private:
    Ui::Home *ui;
    Profile *profile;
    RequestTask *requestTask;
    OfferedTasks *offeredTasks;
    void loadAllRequests();
    QFrame* createRequestCard(int clientId, int requestId, const QString &taskName,
                              const QDateTime &requestTime, const QString &status);
    void viewRequestDetails(int requestId);
    void setupRequestCards();

    bool eventFilter(QObject *watched, QEvent *event) override;
    QString getWorkersForRequest(int requestId) {
        QString workers;
        QSqlQuery query;
        query.prepare("SELECT w.NAME "
                      "FROM ASSIGNMENT a "
                      "JOIN WORKER w ON a.WORKERID = w.WORKERID "
                      "WHERE a.REQUESTID = :id");
        query.bindValue(":id", requestId);

        if (query.exec()) {
            QStringList workerNames;
            while (query.next()) {
                workerNames << query.value("NAME").toString();
            }
            workers = workerNames.join(", ");
        } else {
            qDebug() << "Error fetching workers:" << query.lastError().text();
        }
        return workers.isEmpty() ? "None" : workers;
    }

    QString getAddressForRequest(int requestId) {
        QString address;
        QSqlQuery query;
        query.prepare("SELECT ADDRESS FROM REQUEST WHERE REQUESTID = :id");
        query.bindValue(":id", requestId);

        if (query.exec() && query.next()) {
            address = query.value("ADDRESS").toString();
        } else {
            qDebug() << "Error fetching address:" << query.lastError().text();
            address = "Unknown";
        }
        return address;
    }
};

#endif // HOME_H
