#-------------------------------------------------
#
# Project created by QtCreator 2014-06-12T14:34:51
#
#-------------------------------------------------

QT       += core gui
QT += xml

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../../SpaCentralSoftBin

TARGET = ConfigLoadingTest
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/configloader.cpp

HEADERS  += src/mainwindow.h \
    src/configloader.h \
    src/structures.h

FORMS    += src/forms/mainwindow.ui
