/********************************************************************************
** Form generated from reading UI file 'requestpage.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REQUESTPAGE_H
#define UI_REQUESTPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RequestPage
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *RequestPage)
    {
        if (RequestPage->objectName().isEmpty())
            RequestPage->setObjectName("RequestPage");
        RequestPage->resize(800, 600);
        centralwidget = new QWidget(RequestPage);
        centralwidget->setObjectName("centralwidget");
        RequestPage->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RequestPage);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        RequestPage->setMenuBar(menubar);
        statusbar = new QStatusBar(RequestPage);
        statusbar->setObjectName("statusbar");
        RequestPage->setStatusBar(statusbar);

        retranslateUi(RequestPage);

        QMetaObject::connectSlotsByName(RequestPage);
    } // setupUi

    void retranslateUi(QMainWindow *RequestPage)
    {
        RequestPage->setWindowTitle(QCoreApplication::translate("RequestPage", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RequestPage: public Ui_RequestPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REQUESTPAGE_H
