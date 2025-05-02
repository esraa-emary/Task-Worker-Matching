/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QFrame *frame_3;
    QGridLayout *gridLayout_4;
    QFrame *frame_2;
    QGridLayout *gridLayout_3;
    QFrame *frame_4;
    QGridLayout *gridLayout_5;
    QPushButton *signup;
    QPushButton *login;
    QFrame *frame_5;
    QGridLayout *gridLayout_6;
    QPushButton *exit;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label_3;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1300, 900);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow{\n"
"	color: #000;\n"
"	background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"	stop:0 #778da9, \n"
"	stop:0.2 #355070,\n"
"	stop:0.7 #355070,\n"
"	stop:1 #778da9\n"
"	);\n"
"}\n"
"\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName("frame_3");
        frame_3->setMaximumSize(QSize(16777215, 600));
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_4 = new QGridLayout(frame_3);
        gridLayout_4->setObjectName("gridLayout_4");
        frame_2 = new QFrame(frame_3);
        frame_2->setObjectName("frame_2");
        frame_2->setMaximumSize(QSize(1500, 400));
        frame_2->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"	border: 1px solid #000;\n"
"width: 80%;\n"
"    height: 20px;\n"
"    background-color: #778da9;\n"
"border-radius: 15px;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover{\n"
"    background-color: #0d1b2a;\n"
"transition: all 0.3s ease-in-out;\n"
"	color:#fff;\n"
"}"));
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_3 = new QGridLayout(frame_2);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(-1, 100, -1, 100);
        frame_4 = new QFrame(frame_2);
        frame_4->setObjectName("frame_4");
        frame_4->setFrameShape(QFrame::Shape::StyledPanel);
        frame_4->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_5 = new QGridLayout(frame_4);
        gridLayout_5->setObjectName("gridLayout_5");
        signup = new QPushButton(frame_4);
        signup->setObjectName("signup");
        signup->setMaximumSize(QSize(700, 80));
        QFont font;
        font.setPointSize(17);
        signup->setFont(font);
        signup->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_5->addWidget(signup, 0, 1, 1, 1);

        login = new QPushButton(frame_4);
        login->setObjectName("login");
        login->setMaximumSize(QSize(700, 80));
        login->setFont(font);
        login->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_5->addWidget(login, 0, 0, 1, 1);


        gridLayout_3->addWidget(frame_4, 0, 0, 1, 2);

        frame_5 = new QFrame(frame_2);
        frame_5->setObjectName("frame_5");
        frame_5->setFrameShape(QFrame::Shape::StyledPanel);
        frame_5->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_6 = new QGridLayout(frame_5);
        gridLayout_6->setObjectName("gridLayout_6");
        exit = new QPushButton(frame_5);
        exit->setObjectName("exit");
        exit->setMaximumSize(QSize(700, 80));
        exit->setFont(font);
        exit->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_6->addWidget(exit, 0, 0, 1, 1);


        gridLayout_3->addWidget(frame_5, 1, 0, 1, 2);


        gridLayout_4->addWidget(frame_2, 2, 0, 1, 1);

        frame = new QFrame(frame_3);
        frame->setObjectName("frame");
        frame->setMaximumSize(QSize(16777215, 200));
        frame->setStyleSheet(QString::fromUtf8(""));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(-1, 100, -1, 9);
        label_3 = new QLabel(frame);
        label_3->setObjectName("label_3");
        QFont font1;
        font1.setPointSize(50);
        font1.setBold(true);
        label_3->setFont(font1);
        label_3->setStyleSheet(QString::fromUtf8("color:#fff;"));
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);


        gridLayout_4->addWidget(frame, 1, 0, 1, 1);


        gridLayout->addWidget(frame_3, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        signup->setText(QCoreApplication::translate("MainWindow", "Sign-Up", nullptr));
        login->setText(QCoreApplication::translate("MainWindow", "Log-In", nullptr));
        exit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Task-Worker Matching", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
