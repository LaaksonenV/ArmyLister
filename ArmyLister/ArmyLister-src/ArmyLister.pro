#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T10:03:23
#
#-------------------------------------------------

VERSION = 0.1.1

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArmyLister$$VERSION
TEMPLATE = app

CONFIG += c++17

CONFIG(debug, debug|release) {
    DESTDIR += $$OUT_PWD/../ArmyListerdebug
    TARGET.path = $$OUT_PWD/../ArmyListerdebug
} else {
    DESTDIR += $$OUT_PWD/../ArmyLister
    TARGET.path = $$OUT_PWD/../ArmyLister
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    basicrole.cpp \
    listcreatordeforg.cpp \
    listcreatorwidgets.cpp \
        main.cpp \
        mainwindow.cpp \
    detachment.cpp \
    detachmentrole.cpp \
    battleforged.cpp \
    limithandler.cpp \
    listcreator.cpp \
    listcreatordetach.cpp \
    modelitembase.cpp \
    modelitembasic.cpp \
    modelitemcat.cpp \
    modelitemselection.cpp \
    modelitemslot.cpp \
    modelitemunit.cpp \
    modelsatellitelimiter.cpp \
    organisation.cpp \
    organisationrole.cpp \
    armylistwidget.cpp \
    settings.cpp \
    textedit.cpp \
    itemfactory.cpp \
    armywidget.cpp

HEADERS += \
    basicrole.h \
    listcreatordeforg.h \
    listcreatorwidgets.h \
        mainwindow.h \
    detachment.h \
    detachmentrole.h \
    battleforged.h \
    limithandler.h \
    listcreator.h \
    listcreatordetach.h \
    modelitembase.h \
    modelitembasic.h \
    modelitemcat.h \
    modelitemselection.h \
    modelitemslot.h \
    modelitemunit.h \
    modelsatellitelimiter.h \
    organisation.h \
    organisationrole.h \
    armylistwidget.h \
    settings.h \
    textedit.h \
    itemfactory.h \
    armywidget.h

RESOURCES += \
    resource.qrc
