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

signals:
    void backToMainWindow();

private slots:
    void on_back_clicked();

    void on_login_clicked();

private:
    Ui::LogIn *ui;
    Home *home;
};

#endif // LOGIN_H
