#-------------------------------------------------
#
# Project created by QtCreator 2014-05-10T18:25:27
#
#-------------------------------------------------

QT       += core widgets
QT       -= gui

TARGET = flightIntruments
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    altimeter.cpp \
    artificialhorizon.cpp \
    compass.cpp

HEADERS += \
    altimeter.h \
    artificialhorizon.h \
    compass.h \
    common.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
