#ifndef PROFILECLIENT_H
#define PROFILECLIENT_H

#include <QMainWindow>

namespace Ui {
class ProfileClient;
}

class ProfileClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfileClient(QWidget *parent = nullptr);
    ~ProfileClient();

private:
    Ui::ProfileClient *ui;
};

#endif // PROFILECLIENT_H
