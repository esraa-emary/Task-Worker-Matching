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
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimeZone>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QTextEdit>
#include <QDateTime>

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
    void setClient(int &id,QString &name,QString &password,QString &address,QString &email,QString &phone,QString &feedback);

signals:
    void backToMainWindow();

private slots:
    void requests();
    void workers();
    void profile();
    void logout();
    void tasks();
    void statistics();

    void on_workersPageBtn_clicked();
    void on_requestsPageBtn_2_clicked();
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
    void on_deleteAccount_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_9_clicked();
    void on_taskBtn_clicked();
    void on_taskBtn_2_clicked();
    void on_taskBtn_3_clicked();
    void on_taskBtn_4_clicked();
    void on_taskBtn_5_clicked();
    void on_taskBtn_6_clicked();
    void on_taskBtn_7_clicked();
    void on_workersPageBtn_5_clicked();
    void on_requestsPageBtn_5_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_13_clicked();
    void on_taskBtn_8_clicked();
    void on_workersPageBtn_8_clicked();
    void on_requestsPageBtn_8_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_15_clicked();
    void on_statisticBtn_2_clicked();
    void on_statisticBtn_3_clicked();
    void on_statisticBtn_4_clicked();
    void on_statisticBtn_5_clicked();
    void on_statisticBtn_clicked();
    void on_statisticBtn_6_clicked();
    void on_statisticBtn_7_clicked();
    void on_statisticBtn_8_clicked();
    void on_deleteRequest_clicked();
    void on_editRequest_clicked();
    void on_comboBox_2_currentTextChanged(const QString &arg1);
    void on_cancel_edit_clicked();
    void on_edit_edit_2_clicked();

private:
    void onWorkerStartDateSelected(const QDate &date);
    void onWorkerEndDateSelected(const QDate &date);
    ClientData clientData;
    void loadDataInProfile();
    QFrame* createWorkerCard(int workerId, QString name, QString &taskName,
                                   QString &locations, const float &rating, const float &calculatedWage);
    QDate startDateValue = QDate(2000, 1, 1);
    QDate endDateValue = QDate::currentDate();
    QDate workerStartDateValue = QDate(2000, 1, 1);
    QDate workerEndDateValue = QDate::currentDate();
    QCalendarWidget *workerStartDateCalendar = nullptr;
    QCalendarWidget *workerEndDateCalendar = nullptr;
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
    int currentRequestId = -1;
    // for workers page
    void loadAllWorkers();
    void setupWorkerCards();
    QString getAddressForWorker(int workertId);
    QFrame* createWorkerCard(int workerId,QString name, QString &taskName,
                             QString &locations, const float &rating);
    QFrame* createWorkerCard(int workerId,QString &taskName,
                             QString &locations,const float &rating);

    // for worker page
    void viewWorkerDetails(int workerID);
    void loadAllClients(int workerId);
    QFrame* createClientCardForWorker(int clientId,QString name,
                                   QString &feedback, const float &rating);

    // for client page
    QFrame* createWorkerCardForClient(int workerId, QString name, QString &feedback, const float &rating);
    void loadAllWorkersinClientPage();

    // for tasks page
    void loadAllTasks();
    QFrame* createTaskCard(int &taskId, QString &taskName,int &timeFinish, float &fee);
    void goToRequestPage(int taskId,QString taskName);

    // for statistics page
    void loadAllstatistics();

};
#endif // HOME_H
