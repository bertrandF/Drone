#-------------------------------------------------
#
# Project created by QtCreator 2014-03-26T17:28:53
#
#-------------------------------------------------

QT       += core gui multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CommandStation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configurationpanel.cpp \
    commandpanel.cpp \
    mplayerwidget.cpp

HEADERS  += mainwindow.h \
    configurationpanel.h \
    commandpanel.h \
    mplayerwidget.h \
    commandstationparameters.h \
    constants.h

FORMS    += mainwindow.ui \
    configurationpanel.ui \
    commandpanel.ui \
    configurationwindow.ui

RESOURCES += \
    Resources.qrc

OTHER_FILES += \
    LICENSE.txt

# LibDCP
unix:!macx: LIBS += -L$$OUT_PWD/../libdcp/ -ldcp

INCLUDEPATH += $$PWD/../libdcp
DEPENDPATH += $$PWD/../libdcp

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../libdcp/libdcp.a

# Lib flightInstruments
unix:!macx: LIBS += -L$$OUT_PWD/../flightIntruments/ -lflightIntruments

INCLUDEPATH += $$PWD/../flightIntruments
DEPENDPATH += $$PWD/../flightIntruments

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../flightIntruments/libflightIntruments.a
