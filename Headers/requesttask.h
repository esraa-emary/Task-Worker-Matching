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
    explicit RequestTask(QWidget *parent = nullptr);
    ~RequestTask();

signals:
    void backToHome();

private slots:
    void on_back_clicked();

    void on_submit_clicked();

private:
    Ui::RequestTask *ui;
};

#endif // REQUESTTASK_H
