#-------------------------------------------------
#
# Project created by QtCreator 2016-06-21T18:03:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Restic
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    route.cpp \
    state.cpp

HEADERS  += mainwindow.h \
    route.h \
    state.h

FORMS    += mainwindow.ui
