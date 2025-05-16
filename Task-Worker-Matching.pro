QT       += core gui
QT += svgwidgets
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Sources/home.cpp \
    Sources/login.cpp \
    hoverbutton.cpp \
    main.cpp \
    Sources/mainwindow.cpp \
    Sources/requesttask.cpp \
    Sources/signup.cpp

HEADERS += \
    Headers/home.h \
    Headers/login.h \
    Headers/mainwindow.h \
    Headers/requesttask.h \
    Headers/signup.h \
    hoverbutton.h

FORMS += \
    Forms/home.ui \
    Forms/login.ui \
    Forms/mainwindow.ui \
    Forms/requesttask.ui \
    Forms/signup.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Sources/svg.qrc
