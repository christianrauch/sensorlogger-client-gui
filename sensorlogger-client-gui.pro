#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T21:03:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sensorlogger-client-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    config.h \
    connection.h

FORMS    += mainwindow.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += glib-2.0

unix: CONFIG += c++11

unix:!macx: LIBS += -lthrift

unix:!macx: LIBS += -lsensor_logger_communication
