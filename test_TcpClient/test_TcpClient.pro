#-------------------------------------------------
#
# Project created by QtCreator 2014-06-15T15:33:11
#
#-------------------------------------------------

QT       += core gui
QT += network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_TcpClient
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/tcpclient.cpp

HEADERS  += src/mainwindow.h \
    src/tcpclient.h \
    src/structures.h

FORMS    += src/forms/mainwindow.ui
