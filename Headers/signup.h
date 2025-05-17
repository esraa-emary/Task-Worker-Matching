#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>
#include "../Headers/home.h"

namespace Ui {
class SignUp;
}

class SignUp : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();
    struct  ClientData {
        QString name;
        QString password;
        QString address;
        QString email;
        QString phone;
        QString feedback;
        int id;
    };

signals:
    void backToMainWindow();

private slots:
    void on_back_clicked();
    void on_signup_clicked();

private:
    Ui::SignUp *ui;
    Home *home;
    ClientData clientData;
};

#endif // SIGNUP_H
