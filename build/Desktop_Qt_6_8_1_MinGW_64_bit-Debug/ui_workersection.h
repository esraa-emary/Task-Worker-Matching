/********************************************************************************
** Form generated from reading UI file 'workersection.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORKERSECTION_H
#define UI_WORKERSECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WorkerSection
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *WorkerSection)
    {
        if (WorkerSection->objectName().isEmpty())
            WorkerSection->setObjectName("WorkerSection");
        WorkerSection->resize(800, 600);
        centralwidget = new QWidget(WorkerSection);
        centralwidget->setObjectName("centralwidget");
        WorkerSection->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WorkerSection);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        WorkerSection->setMenuBar(menubar);
        statusbar = new QStatusBar(WorkerSection);
        statusbar->setObjectName("statusbar");
        WorkerSection->setStatusBar(statusbar);

        retranslateUi(WorkerSection);

        QMetaObject::connectSlotsByName(WorkerSection);
    } // setupUi

    void retranslateUi(QMainWindow *WorkerSection)
    {
        WorkerSection->setWindowTitle(QCoreApplication::translate("WorkerSection", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WorkerSection: public Ui_WorkerSection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORKERSECTION_H
