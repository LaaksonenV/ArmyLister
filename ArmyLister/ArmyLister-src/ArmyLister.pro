#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T10:03:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArmyLister
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
    detachment.cpp \
    detachmentrole.cpp \
    battleforged.cpp \
    limithandler.cpp \
    modelitem.cpp \
    fileio.cpp \
    listcreator.cpp \
    listcreatordetach.cpp \
    organisation.cpp \
    slotbutton.cpp \
    sortedstringlist.cpp \
    armylistwidget.cpp \
    settings.cpp \
    limitmihandler.cpp \
    textedit.cpp \
    itemfactory.cpp \
    slotsllabel.cpp \
    slotselection.cpp \
    armywidget.cpp

HEADERS += \
        mainwindow.h \
    detachment.h \
    detachmentrole.h \
    battleforged.h \
    limithandler.h \
    modelitem.h \
    fileio.h \
    listcreator.h \
    listcreatordetach.h \
    organisation.h \
    slotbutton.h \
    sortedstringlist.h \
    armylistwidget.h \
    settings.h \
    limitmihandler.h \
    textedit.h \
    itemfactory.h \
    slotslabel.h \
    slotselection.h \
    armywidget.h \
    structs.h

RESOURCES += \
    resource.qrc
