#-------------------------------------------------
#
# Project created by QtCreator 2014-06-15T15:32:51
#
#-------------------------------------------------

QT       += core gui
QT += network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_TcpServer
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/tcpserver.cpp \
    src/tcpserverthread.cpp

HEADERS  += src/mainwindow.h \
    src/tcpserver.h \
    src/utilities.h \
    src/structures.h \
    src/constants.h \
    src/tcpserverthread.h

FORMS    += src/forms/mainwindow.ui
