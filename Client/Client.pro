#-------------------------------------------------
#
# Project created by QtCreator 2017-08-10T13:53:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        login.cpp \
    chatdialog.cpp \
    sndfileserver.cpp \
    rcvfileclient.cpp

HEADERS  += login.h \
    chatdialog.h \
    msgtype.h \
    sndfileserver.h \
    rcvfileclient.h

FORMS    += login.ui \
    chatdialog.ui \
    sndfileserver.ui \
    rcvfileclient.ui

RESOURCES += \
    image.qrc
