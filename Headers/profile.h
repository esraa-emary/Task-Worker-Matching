#ifndef PROFILE_H
#define PROFILE_H

#include <QMainWindow>

namespace Ui {
class Profile;
}

class Profile : public QMainWindow
{
    Q_OBJECT

public:
    explicit Profile(QWidget *parent = nullptr);
    ~Profile();

signals:
    void backToHome();

private slots:
    void on_back_clicked();

private:
    Ui::Profile *ui;
};

#endif // PROFILE_H
