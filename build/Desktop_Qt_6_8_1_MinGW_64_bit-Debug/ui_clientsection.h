/********************************************************************************
** Form generated from reading UI file 'clientsection.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTSECTION_H
#define UI_CLIENTSECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientSection
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientSection)
    {
        if (ClientSection->objectName().isEmpty())
            ClientSection->setObjectName("ClientSection");
        ClientSection->resize(800, 600);
        centralwidget = new QWidget(ClientSection);
        centralwidget->setObjectName("centralwidget");
        ClientSection->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ClientSection);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        ClientSection->setMenuBar(menubar);
        statusbar = new QStatusBar(ClientSection);
        statusbar->setObjectName("statusbar");
        ClientSection->setStatusBar(statusbar);

        retranslateUi(ClientSection);

        QMetaObject::connectSlotsByName(ClientSection);
    } // setupUi

    void retranslateUi(QMainWindow *ClientSection)
    {
        ClientSection->setWindowTitle(QCoreApplication::translate("ClientSection", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientSection: public Ui_ClientSection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTSECTION_H
