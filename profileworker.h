#ifndef PROFILEWORKER_H
#define PROFILEWORKER_H

#include <QMainWindow>

namespace Ui {
class ProfileWorker;
}

class ProfileWorker : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileWorker(QWidget *parent = nullptr);
    ~ProfileWorker();

private:
    Ui::ProfileWorker *ui;
};

#endif // PROFILEWORKER_H
