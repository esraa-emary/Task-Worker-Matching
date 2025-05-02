#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

namespace Ui {
class LogIn;
}

class LogIn : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);
    ~LogIn();

private:
    Ui::LogIn *ui;
};

#endif // LOGIN_H
