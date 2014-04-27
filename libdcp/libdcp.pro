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
    dcpserver.cpp \
    dcpservercentral.cpp \
    dcpservercommand.cpp

HEADERS += \
    dcp.h \
    dcpcommands.h \
    dcppacket.h \
    dcppackethandlerinterface.h \
    dcpserver.h \
    dcpservercentral.h \
    dcpservercommand.h \
    libdcp_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    LICENSE.txt
