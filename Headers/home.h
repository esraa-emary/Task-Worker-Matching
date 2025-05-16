#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
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
    void on_workersPageBtn_clicked();
    void on_requestsPageBtn_2_clicked();
    void on_addRequest_clicked();

private:
    Ui::Home *ui;
    RequestTask *requestTask;

    // for requests page
    void loadAllRequests();
    void viewRequestDetails(int requestId);
    void setupRequestCards();
    QString getWorkersForRequest(int requestId);
    QString getAddressForRequest(int requestId);
    bool eventFilter(QObject *watched, QEvent *event) override;
    QFrame* createRequestCard(int clientId, int requestId, const QString &taskName,
                              const QDateTime &requestTime, const QString &status);

    // for workers page
    void loadAllWorkers();
    void setupWorkerCards();
    QString getAddressForWorker(int workertId);
    QFrame* createWorkerCard(int workerId,QString &taskName,
                             QString &locations,const float &rating);

    // for making a request

};

#endif // HOME_H
