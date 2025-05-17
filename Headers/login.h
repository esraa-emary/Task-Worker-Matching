#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "../Headers/home.h"

namespace Ui {
class LogIn;
}

class LogIn : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);
    ~LogIn();
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
    void on_login_clicked();

private:
    Ui::LogIn *ui;
    Home *home;
    ClientData clientData;
};

#endif // LOGIN_H
