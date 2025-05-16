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

private:
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

    // for workers page
    void loadAllWorkers();
    void setupWorkerCards();
    QString getAddressForWorker(int workertId);
    QFrame* createWorkerCard(int workerId,QString &taskName,
                             QString &locations,const float &rating);

    // for making a request

};

#endif // HOME_H
