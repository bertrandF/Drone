#-------------------------------------------------
#
# Project created by QtCreator 2014-04-13T18:49:59
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = CentralStation
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS +=


unix:!macx: LIBS += -L$$OUT_PWD/../libdcp/ -ldcp

INCLUDEPATH += $$PWD/../libdcp
DEPENDPATH += $$PWD/../libdcp

OTHER_FILES += \
    LICENSE.txt
