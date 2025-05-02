#ifndef REQUESTPAGE_H
#define REQUESTPAGE_H

#include <QMainWindow>

namespace Ui {
class RequestPage;
}

class RequestPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit RequestPage(QWidget *parent = nullptr);
    ~RequestPage();

private:
    Ui::RequestPage *ui;
};

#endif // REQUESTPAGE_H
