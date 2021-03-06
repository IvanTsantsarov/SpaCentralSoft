#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T11:03:05
#
#-------------------------------------------------

#DEFINES += DEBUG_MODE_FINETUNING
DEFINES += USE_GRAPHICSRECTITEM_ZONE
DEFINES += USE_HIGHLIGHTING
#DEFINES += USE_OBSOLETE


QT += core gui
QT += xml
QT += network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUIController
TEMPLATE = app

DESTDIR = ../../SpaCentralSoftBin

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/temperatureindicator.cpp \
    src/cgraphicsrectitem.cpp \
    src/cgraphicsscene.cpp \
    src/forms/dialogabout.cpp \
    src/configloader.cpp \
    src/forms/dialogtemperaturetarget.cpp

HEADERS  += src/mainwindow.h \
    src/temperatureindicator.h \
    src/cgraphicsrectitem.h \
    src/cgraphicsscene.h \
    src/forms/dialogabout.h \
    src/configloader.h \
    src/custom/constants.h \
    src/custom/types.h \
    src/custom/structures.h \
    src/forms/dialogtemperaturetarget.h

FORMS    += src/forms/mainwindow.ui \
    src/forms/dialogabout.ui \
    src/forms/dialogtemperaturetarget.ui

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

###################################################################################################
# Copy assets to DESTDIR
###################################################################################################

## If using Shadow build, get the output folder
#CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
#CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug
#
## If using normal build (non-Shadow) that would have worked as well.
#CONFIG(release, debug|release): DESTDIR = release
#CONFIG(debug, debug|release): DESTDIR = debug
#
# Finally add "make install" as a post-build step

CONFIG += copy_dir_files # Enables the install rule to also copy directories, not just files
CONFIG += no_check_exist

assets.path = $$DESTDIR/assets
assets.files +=$$files(assets/*.*)
INSTALLS += assets

assetsImagesDark.path = $$DESTDIR/assets/dark
assetsImagesDark.files +=$$files(assets/dark/*.*)
INSTALLS += assetsImagesDark

assetsImagesLight.path = $$DESTDIR/assets/light
assetsImagesLight.files +=$$files(assets/light/*.*)
INSTALLS += assetsImagesLight

#copydata.commands = $(COPY_DIR) $$PWD/assets $$DESTDIR
#first.depends = $(first) copydata
#export(first.depends)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first copydata
