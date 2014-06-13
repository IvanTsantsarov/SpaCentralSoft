#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T11:03:05
#
#-------------------------------------------------

#DEFINES += DEBUG_MODE_FINETUNING
DEFINES += USE_GRAPHICSRECTITEM_ZONE
#DEFINES += USE_OBSOLETE


QT += core gui
QT += xml

QMAKE_CXXFLAGS += -std=c++11

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
    src/forms/dialogchangetemperature.cpp \
    src/configloader.cpp

HEADERS  += src/mainwindow.h \
    src/temperatureindicator.h \
    src/cgraphicsrectitem.h \
    src/cgraphicsscene.h \
    src/forms/dialogabout.h \
    src/forms/dialogchangetemperature.h \
    src/configloader.h \
    src/custom/constants.h \
    src/custom/types.h \
    src/custom/structures.h

FORMS    += src/forms/mainwindow.ui \
    src/forms/dialogabout.ui \
    src/forms/dialogchangetemperature.ui

OTHER_FILES += \
    assets/map.png \
    assets/dark/zone1.png \
    assets/dark/zone2.png \
    assets/dark/zone3.png \
    assets/dark/zone4.png \
    assets/dark/zone5.png \
    assets/dark/zone6.png \
    assets/dark/zone7.png \
    assets/light/zone1.png \
    assets/light/zone2.png \
    assets/light/zone3.png \
    assets/light/zone4.png \
    assets/light/zone5.png \
    assets/light/zone6.png \
    assets/light/zone7.png \
    assets/config.xml
