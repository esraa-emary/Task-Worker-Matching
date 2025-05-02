/********************************************************************************
** Form generated from reading UI file 'profileclient.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILECLIENT_H
#define UI_PROFILECLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfileClient
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ProfileClient)
    {
        if (ProfileClient->objectName().isEmpty())
            ProfileClient->setObjectName("ProfileClient");
        ProfileClient->resize(800, 600);
        centralwidget = new QWidget(ProfileClient);
        centralwidget->setObjectName("centralwidget");
        ProfileClient->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ProfileClient);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        ProfileClient->setMenuBar(menubar);
        statusbar = new QStatusBar(ProfileClient);
        statusbar->setObjectName("statusbar");
        ProfileClient->setStatusBar(statusbar);

        retranslateUi(ProfileClient);

        QMetaObject::connectSlotsByName(ProfileClient);
    } // setupUi

    void retranslateUi(QMainWindow *ProfileClient)
    {
        ProfileClient->setWindowTitle(QCoreApplication::translate("ProfileClient", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProfileClient: public Ui_ProfileClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILECLIENT_H
