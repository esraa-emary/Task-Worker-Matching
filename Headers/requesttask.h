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

signals:
    void backToHome();

private slots:
    void on_back_clicked();
    void on_add_clicked();
    void on_cancel_clicked();

private:
    Ui::RequestTask *ui;
};

#endif // REQUESTTASK_H
