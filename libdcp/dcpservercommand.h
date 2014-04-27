/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercommand.h -- bertrand
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * */

#ifndef DCPSERVERCOMMAND_H
#define DCPSERVERCOMMAND_H

#include <QtGlobal>
#include <QHostAddress>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

#include <dcpserver.h>
#include <dcpcommands.h>



enum DCPServerCommandStatus {
    Init, SayingHello, NotConnected, Connecting, Connected, Stopping, Stopped};

class DCPServerCommand : public DCPServer
{
    Q_OBJECT

public:
    DCPServerCommand(QUdpSocket *sock);

    inline QHostAddress    getAddrDrone()   { return this->addrDrone; }
    inline quint16         getPortDrone()   { return this->portDrone; }
    inline QHostAddress    getAddrCentralStation()
        { return this->addrCentralStation; }
    inline quint16         getPortCentralStation()
        { return this->portCentralStation; }
    inline qint8           getSessionIdDrone()  { return this->sessIdDrone; }
    inline qint8            getSessionIdCentralStation()
        { return this->sessIdCentralStation; }
    inline enum DCPServerCommandStatus getStatus()
        { return this->status; }

    void            setCentralStationHost(QHostAddress addr, quint16 port);
    void            setDroneHost(QHostAddress addr, quint16 port);
    void            setStatus(enum DCPServerCommandStatus status);

    // TODO: Make avaliable only to friends
    void            setSessionIdDrone(qint8 sessIdDrone);
    void            setSessionIdCentralStation(qint8 sessIdCentralStation);

    void            sayHello(QString description,
                             DCPCommandHelloFromRemote::remoteType type);
    void            connectToDrone(qint8 id);

signals:
    void statusChanged(enum DCPServerCommandStatus status);

protected:
    qint8           sessIdDrone;
    qint8           sessIdCentralStation;
    QHostAddress    addrDrone;
    quint16         portDrone;
    QHostAddress    addrCentralStation;
    quint16         portCentralStation;

    enum DCPServerCommandStatus status;
    QMutex                      statusMutex;
};

#endif // DCPSERVERCOMMAND_H
