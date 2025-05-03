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

signals:
    void backToMainWindow();

private slots:
    void on_back_clicked();

    void on_submit_clicked();

private:
    Ui::SignUp *ui;
    Home *home;
};

#endif // SIGNUP_H
