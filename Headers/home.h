#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "requesttask.h"
#include <QFrame>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QObject>
#include <QEvent>
#include "ui_home.h"

namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();
    struct  ClientData {
        QString name;
        QString password;
        QString address;
        QString email;
        QString phone;
        QString feedback;
        int id;
    };
    void setClient(int &id,QString &name,QString &password,QString &address,QString &email,QString &phone,QString &feedback)
    {
        clientData.id = id;
        clientData.name = name;
        clientData.password = password;
        clientData.address = address;
        clientData.email = email;
        clientData.phone = phone;
        clientData.feedback = feedback;

        // put data into profile;
        ui->pushButton_2->setText(clientData.name);
        ui->pushButton_3->setText(clientData.name);
        ui->pushButton_4->setText(clientData.name);
        ui->pushButton_5->setText(clientData.name);
        ui->pushButton_6->setText(clientData.name);
        ui->pushButton_7->setText(clientData.name);
    }

signals:
    void backToMainWindow();

private slots:
    void on_workersPageBtn_clicked();
    void on_requestsPageBtn_2_clicked();
    void on_addRequest_clicked();
    void on_request_clicked();
    void on_logout_clicked();
    void on_pushButton_clicked();
    void on_startDate_clicked();
    void on_endDate_clicked();
    void onStartDateSelected(const QDate &date);
    void onEndDateSelected(const QDate &date);
    void on_filterName_clicked();
    void on_requestsPageBtn_4_clicked();
    void on_workersPageBtn_4_clicked();
    void on_workersPageBtn_3_clicked();
    void on_requestsPageBtn_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_edit_clicked();
    void on_update_clicked();
    void on_requestsPageBtn_6_clicked();
    void on_workersPageBtn_6_clicked();
    void on_pushButton_6_clicked();
    void on_workersPageBtn_7_clicked();
    void on_requestsPageBtn_7_clicked();
    void on_pushButton_7_clicked();
    void on_requests_clicked();

    void on_endDate_3_clicked();

    void on_startDate_3_clicked();

private:
    ClientData clientData;
    void loadDataInProfile();

    QDate startDateValue = QDate(2000, 1, 1);
    QDate endDateValue = QDate::currentDate();
    Ui::Home *ui;
    RequestTask *requestTask;
    QCalendarWidget *startDateCalendar = nullptr;
    QCalendarWidget *endDateCalendar = nullptr;

    bool connectToDatabase();
    void loadAllRequests();
    QFrame* createRequestCard(int clientId, int requestId, const QString &taskName,
                              const QDateTime &requestTime, const QString &status);
    void viewRequestDetails(int requestId);
    void setupRequestCards();
    void setupUiElements();

    bool eventFilter(QObject *watched, QEvent *event) override;

    QString getWorkersForRequest(int requestId);
    QString getAddressForRequest(int requestId);

    void setup_request_page();

    // for workers page
    void loadAllWorkers();
    void setupWorkerCards();
    QString getAddressForWorker(int workertId);
<<<<<<< HEAD
    QFrame* createWorkerCard(int workerId,QString name, QString &taskName,
                             QString &locations, const float &rating);
=======
    QFrame* createWorkerCard(int workerId,QString &taskName,
                             QString &locations,const float &rating);
    QDate workerStartDateValue;
    QDate workerEndDateValue;
    QCalendarWidget *workerStartDateCalendar = nullptr;
    QCalendarWidget *workerEndDateCalendar = nullptr;
>>>>>>> 1cd6aca (before merge)

    // for worker page
    void viewWorkerDetails(int workerID);
    void loadAllClients(int workerId);
    QFrame* createClientCardForWorker(int clientId,QString name,
                                   QString &feedback, const float &rating);

    // for client page
    QFrame* createWorkerCardForClient(int workerId, QString name, QString &feedback, const float &rating);
    void loadAllWorkersinClientPage();
};

#endif // HOME_H
