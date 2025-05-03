#ifndef OFFEREDTASKS_H
#define OFFEREDTASKS_H

#include <QMainWindow>

namespace Ui {
class OfferedTasks;
}

class OfferedTasks : public QMainWindow
{
    Q_OBJECT

public:
    explicit OfferedTasks(QWidget *parent = nullptr);
    ~OfferedTasks();

signals:
    void backToHome();

private slots:
    void on_back_clicked();

private:
    Ui::OfferedTasks *ui;
};

#endif // OFFEREDTASKS_H
