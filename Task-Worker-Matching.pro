QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientsection.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    offeredtasks.cpp \
    profileclient.cpp \
    profileworker.cpp \
    requestpage.cpp \
    signup.cpp \
    workersection.cpp

HEADERS += \
    clientsection.h \
    login.h \
    mainwindow.h \
    offeredtasks.h \
    profileclient.h \
    profileworker.h \
    requestpage.h \
    signup.h \
    workersection.h

FORMS += \
    clientsection.ui \
    login.ui \
    mainwindow.ui \
    offeredtasks.ui \
    profileclient.ui \
    profileworker.ui \
    requestpage.ui \
    signup.ui \
    workersection.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
