#ifndef CLIENTSECTION_H
#define CLIENTSECTION_H

#include <QMainWindow>

namespace Ui {
class ClientSection;
}

class ClientSection : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientSection(QWidget *parent = nullptr);
    ~ClientSection();

private:
    Ui::ClientSection *ui;
};

#endif // CLIENTSECTION_H
