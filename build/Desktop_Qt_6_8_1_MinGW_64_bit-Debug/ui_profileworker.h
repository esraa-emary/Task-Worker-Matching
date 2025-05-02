/********************************************************************************
** Form generated from reading UI file 'profileworker.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEWORKER_H
#define UI_PROFILEWORKER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfileWorker
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ProfileWorker)
    {
        if (ProfileWorker->objectName().isEmpty())
            ProfileWorker->setObjectName("ProfileWorker");
        ProfileWorker->resize(800, 600);
        centralwidget = new QWidget(ProfileWorker);
        centralwidget->setObjectName("centralwidget");
        ProfileWorker->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ProfileWorker);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        ProfileWorker->setMenuBar(menubar);
        statusbar = new QStatusBar(ProfileWorker);
        statusbar->setObjectName("statusbar");
        ProfileWorker->setStatusBar(statusbar);

        retranslateUi(ProfileWorker);

        QMetaObject::connectSlotsByName(ProfileWorker);
    } // setupUi

    void retranslateUi(QMainWindow *ProfileWorker)
    {
        ProfileWorker->setWindowTitle(QCoreApplication::translate("ProfileWorker", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProfileWorker: public Ui_ProfileWorker {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEWORKER_H
