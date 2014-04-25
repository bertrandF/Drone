/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserverbackendremote.h -- bertrand
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

#include "dcpserverbackendremote.h"
#include "dcpcommands.h"
#include "dcpserver.h"

DCPServerBackendRemote::DCPServerBackendRemote() :
    DCPServerBackend(),
    sessIdDrone(DCP_IDNULL),
    sessIdCentralStation(DCP_SESSIDCENTRAL),
    status(Init)
{
    this->handler = new DCPPacketHandlerCommandStationHello(this);
}

void DCPServerBackendRemote::setCentralStationHost(
        QHostAddress addr,
        quint16 port)
{
    this->addrCentralStation = addr;
    this->portCentralStation = port;
}

void DCPServerBackendRemote::setDroneHost(QHostAddress addr, quint16 port)
{
    this->addrDrone = addr;
    this->portDrone = port;
}

void DCPServerBackendRemote::setSessionIdDrone(qint8 sessID)
{
    this->sessIdDrone = sessID;
}

void DCPServerBackendRemote::setSessionIdCentralStation(
        qint8 sessIdCentralStation)
{
    this->sessIdCentralStation = sessIdCentralStation;
}

void DCPServerBackendRemote::setStatus(DCPServerBackendRemoteStatus status)
{
    this->statusMutex.lock();
    this->status = status;
    this->statusMutex.unlock();
    this->statusChanged.wakeAll();
}

void DCPServerBackendRemote::registerWithServer(DCPServer *srv)
{
    QList<qint8>  ids;
    ids.append(this->sessIdCentralStation);
    ids.append(this->sessIdDrone);
    srv->registerBackend(this, ids);
}

void DCPServerBackendRemote::sayHello(QString description,
                                     DCPCommandHelloFromRemote::remoteType type)
{
    if(this->getStatus() != Init) return;
    this->setStatus(SayingHello);

    DCPCommandHelloFromRemote *hello =
            new DCPCommandHelloFromRemote(this->sessIdCentralStation);
    hello->setDescription(description);
    hello->setAddrDst(this->addrCentralStation);
    hello->setPortDst(this->portCentralStation);
    hello->setRemoteType(type);
    this->time.start();
    hello->setTimestamp(this->time.elapsed());
    this->sendPacket(hello);
}

enum DCPServerBackendRemoteStatus DCPServerBackendRemote::waitStatusChanged()
{
    enum DCPServerBackendRemoteStatus status;
    this->statusMutex.lock();
    this->statusChanged.wait(&(this->statusMutex));
    status = this->getStatus();
    this->statusMutex.unlock();

    return status;
}

void DCPServerBackendRemote::connectToDrone(qint8 id)
{
    if(this->getStatus() != NotConnected) return;

    this->setStatus(Connecting);
    DCPCommandConnectToDrone *conn = new DCPCommandConnectToDrone(
                this->sessIdCentralStation);
    conn->setAddrDst(this->addrCentralStation);
    conn->setPortDst(this->portCentralStation);
    conn->setDroneId(id);
    conn->setTimestamp(this->time.elapsed());
    this->sendPacket(conn);
}
