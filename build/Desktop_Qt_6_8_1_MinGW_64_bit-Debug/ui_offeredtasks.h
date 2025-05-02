/********************************************************************************
** Form generated from reading UI file 'offeredtasks.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OFFEREDTASKS_H
#define UI_OFFEREDTASKS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OfferedTasks
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *OfferedTasks)
    {
        if (OfferedTasks->objectName().isEmpty())
            OfferedTasks->setObjectName("OfferedTasks");
        OfferedTasks->resize(800, 600);
        centralwidget = new QWidget(OfferedTasks);
        centralwidget->setObjectName("centralwidget");
        OfferedTasks->setCentralWidget(centralwidget);
        menubar = new QMenuBar(OfferedTasks);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        OfferedTasks->setMenuBar(menubar);
        statusbar = new QStatusBar(OfferedTasks);
        statusbar->setObjectName("statusbar");
        OfferedTasks->setStatusBar(statusbar);

        retranslateUi(OfferedTasks);

        QMetaObject::connectSlotsByName(OfferedTasks);
    } // setupUi

    void retranslateUi(QMainWindow *OfferedTasks)
    {
        OfferedTasks->setWindowTitle(QCoreApplication::translate("OfferedTasks", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OfferedTasks: public Ui_OfferedTasks {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OFFEREDTASKS_H
