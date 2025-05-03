#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "offeredtasks.h"
#include "profile.h"
#include "requesttask.h"

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
    void on_offeredTasks_clicked();

    void on_profile_clicked();

    void on_request_clicked();

    void on_logout_clicked();

private:
    Ui::Home *ui;
    Profile *profile;
    RequestTask *requestTask;
    OfferedTasks *offeredTasks;
};

#endif // HOME_H
