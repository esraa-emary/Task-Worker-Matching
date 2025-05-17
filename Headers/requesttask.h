#ifndef REQUESTTASK_H
#define REQUESTTASK_H

#include <QMainWindow>

namespace Ui {
class RequestTask;
}

class RequestTask : public QMainWindow
{
    Q_OBJECT

public:
    explicit RequestTask(QMainWindow *parent = nullptr);
    ~RequestTask();
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
    }

signals:
    void backToHome();

private slots:
    void on_back_clicked();
    void on_add_clicked();
    void on_cancel_clicked();

private:
    Ui::RequestTask *ui;
    ClientData clientData;
};

#endif // REQUESTTASK_H
