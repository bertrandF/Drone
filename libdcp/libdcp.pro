#-------------------------------------------------
#
# Project created by QtCreator 2014-04-16T10:06:57
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = dcp
TEMPLATE = lib

DEFINES += LIBDCP_LIBRARY

SOURCES += \
    dcpcommands.cpp \
    dcppacket.cpp \
    dcppackethandlerinterface.cpp \
    dcpserverbackend.cpp \
    dcpserverbackendcentral.cpp \
    dcpserverbackendremote.cpp

HEADERS += \
    dcp.h \
    dcpcommands.h \
    dcppacket.h \
    dcppackethandlerinterface.h \
    dcpserverbackend.h \
    dcpserverbackendcentral.h \
    dcpserverbackendremote.h \
    libdcp_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    LICENSE.txt
