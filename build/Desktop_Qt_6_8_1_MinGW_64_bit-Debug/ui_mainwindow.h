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
    QPushButton *pushButton_2;
    QPushButton *exit;
    QPushButton *pushButton_5;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1300, 900);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow{\n"
"	color: #000;\n"
"	background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"	stop:0 #a3b18a, \n"
"	stop:0.2 #588157,\n"
"	stop:0.7 #588157,\n"
"	stop:1 #a3b18a\n"
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
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_4 = new QGridLayout(frame_3);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(9, -1, -1, 9);
        frame_2 = new QFrame(frame_3);
        frame_2->setObjectName("frame_2");
        frame_2->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"	border: 1px solid #000;\n"
"width: 80%;\n"
"    height: 20px;\n"
"    background-color: #a3b18a;\n"
"border-radius: 15px;\n"
"\n"
"}\n"
"\n"
"QPushButton:hover{\n"
"    background-color: #3a5a40;\n"
"transition: all 0.3s ease-in-out;\n"
"	color:#fff;\n"
"}"));
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_3 = new QGridLayout(frame_2);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(-1, 200, -1, 200);
        pushButton_2 = new QPushButton(frame_2);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMaximumSize(QSize(800, 80));
        QFont font;
        font.setPointSize(17);
        pushButton_2->setFont(font);
        pushButton_2->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_3->addWidget(pushButton_2, 3, 0, 1, 1);

        exit = new QPushButton(frame_2);
        exit->setObjectName("exit");
        exit->setMaximumSize(QSize(800, 80));
        exit->setFont(font);
        exit->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_3->addWidget(exit, 5, 0, 1, 1);

        pushButton_5 = new QPushButton(frame_2);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setMaximumSize(QSize(800, 80));
        pushButton_5->setFont(font);
        pushButton_5->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_3->addWidget(pushButton_5, 2, 0, 1, 1);

        pushButton_3 = new QPushButton(frame_2);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMaximumSize(QSize(800, 80));
        pushButton_3->setFont(font);
        pushButton_3->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        gridLayout_3->addWidget(pushButton_3, 4, 0, 1, 1);

        pushButton = new QPushButton(frame_2);
        pushButton->setObjectName("pushButton");
        pushButton->setEnabled(true);
        pushButton->setMinimumSize(QSize(0, 20));
        pushButton->setMaximumSize(QSize(800, 80));
        pushButton->setFont(font);
        pushButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton->setStyleSheet(QString::fromUtf8(""));

        gridLayout_3->addWidget(pushButton, 1, 0, 1, 1);


        gridLayout_4->addWidget(frame_2, 1, 1, 1, 1);

        frame = new QFrame(frame_3);
        frame->setObjectName("frame");
        frame->setMaximumSize(QSize(16777215, 16777215));
        frame->setStyleSheet(QString::fromUtf8(""));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(-1, 350, -1, 356);
        label_3 = new QLabel(frame);
        label_3->setObjectName("label_3");
        QFont font1;
        font1.setPointSize(40);
        font1.setBold(true);
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        label = new QLabel(frame);
        label->setObjectName("label");
        label->setFont(font1);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_2->addWidget(label, 1, 0, 1, 1);


        gridLayout_4->addWidget(frame, 1, 0, 1, 1);


        gridLayout->addWidget(frame_3, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        exit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Task-Worker Matching", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
