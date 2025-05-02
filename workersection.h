#ifndef WORKERSECTION_H
#define WORKERSECTION_H

#include <QMainWindow>

namespace Ui {
class WorkerSection;
}

class WorkerSection : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkerSection(QWidget *parent = nullptr);
    ~WorkerSection();

private:
    Ui::WorkerSection *ui;
};

#endif // WORKERSECTION_H
