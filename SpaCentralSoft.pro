#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T11:03:05
#
#-------------------------------------------------

#DEFINES += DEBUG_MODE_FINETUNING
DEFINES += USE_GRAPHICSRECTITEM_ZONE


QT += core gui

QMAKE_CXXFLAGS += -std=c++11

#QMAKE_CXXFLAGS += "-fsanitize=address -fno-omit-frame-pointer"
#QMAKE_CFLAGS += "-fsanitize=address -fno-omit-frame-pointer"
#QMAKE_LFLAGS += "-fsanitize=address"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpaCentralSoft
TEMPLATE = app

DESTDIR = ../SpaCentralSoftBin

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/temperatureindicator.cpp \
    src/cgraphicsrectitem.cpp \
    src/cgraphicsscene.cpp \
    src/forms/dialogabout.cpp \
    src/forms/dialogchangetemperature.cpp

HEADERS  += src/mainwindow.h \
    src/temperatureindicator.h \
    src/constants.h \
    src/cgraphicsrectitem.h \
    src/cgraphicsscene.h \
    src/forms/dialogabout.h \
    src/forms/dialogchangetemperature.h

FORMS    += src/forms/mainwindow.ui \
    src/forms/dialogabout.ui \
    src/forms/dialogchangetemperature.ui

OTHER_FILES += \
    assets/map.png \
    assets/zone1.png \
    assets/zone2.png \
    assets/zone3.png \
    assets/zone4.png \
    assets/zone5.png \
    assets/zone6.png \
    assets/zone7.png

deployment.path = assets/
INSTALLS += deployment
