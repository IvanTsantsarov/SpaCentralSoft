#-------------------------------------------------
#
# Project created by QtCreator 2014-06-15T15:32:51
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_TcpServer
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/tcpserver.cpp

HEADERS  += src/mainwindow.h \
    src/tcpserver.h

FORMS    += src/forms/mainwindow.ui
