/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcppackethandleinterface.h -- bertrand
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

#ifndef DCPPACKETHANDLERINTERFACE_H
#define DCPPACKETHANDLERINTERFACE_H

#include <QList>
#include <QString>
#include <QHostAddress>

#include "dcpserver.h"

class DCPServer;
class DCPPacket;
class DCPCommandHelloFromCentralStation;



class DCPPacketHandlerInterface
{
public:
    DCPPacketHandlerInterface(DCPServer* server);

    virtual void handleNull                         (DCPPacket *packet) = 0;
    virtual void handleCommandAilerons              (DCPPacket *packet) = 0;
    virtual void handleCommandIsAlive               (DCPPacket *packet) = 0;
    virtual void handleCommandAck                   (DCPPacket *packet) = 0;
    virtual void handleCommandThrottle              (DCPPacket *packet) = 0;
    virtual void handleCommandSetSessID             (DCPPacket *packet) = 0;
    virtual void handleCommandUnsetSessID           (DCPPacket *packet) = 0;
    virtual void handleCommandHelloFromCentral      (DCPPacket *packet) = 0;
    virtual void handleCommandHelloFromRemote       (DCPPacket *packet) = 0;
    virtual void handleCommandBye                   (DCPPacket *packet) = 0;
    virtual void handleCommandConnectToDrone        (DCPPacket *packet) = 0;
    virtual void handleCommandUnconnectFromDrone    (DCPPacket *packet) = 0;

protected:
    DCPServer* server;
};

/*
 * DEFAULT PACKET Zombie (alive but does nothing)
 * */
//class DCPPacketHandlerZombie : public DCPPacketHandlerInterface
//{
//public:
//    DCPPacketHandlerZombie(DCPServerBackend *backend);

//    virtual void handleNull                         (DCPPacket* packet);
//    virtual void handleCommandAilerons              (DCPPacket* packet);
//    virtual void handleCommandIsAlive               (DCPPacket* packet);
//    virtual void handleCommandAck                   (DCPPacket* packet);
//    virtual void handleCommandThrottle              (DCPPacket* packet);
//    virtual void handleCommandSetSessID             (DCPPacket* packet);
//    virtual void handleCommandUnsetSessID           (DCPPacket* packet);
//    virtual void handleCommandHello                 (DCPPacket* packet);
//    virtual void handleCommandBye                   (DCPPacket* packet);
//    virtual void handleCommandConnectToDrone        (DCPPacket* packet);
//    virtual void handleCommandUnconnectFromDrone    (DCPPacket* packet);
//};

/*
 * COMMAND STATION -- Packet Handler for Hello handshake
 * */
class DCPPacketHandlerCommandStationHello : public DCPPacketHandlerInterface
{
public:
    DCPPacketHandlerCommandStationHello(DCPServer *backend);

    virtual void handleNull                         (DCPPacket* packet);
    virtual void handleCommandAilerons              (DCPPacket* packet);
    virtual void handleCommandIsAlive               (DCPPacket* packet);
    virtual void handleCommandAck                   (DCPPacket* packet);
    virtual void handleCommandThrottle              (DCPPacket* packet);
    virtual void handleCommandSetSessID             (DCPPacket* packet);
    virtual void handleCommandUnsetSessID           (DCPPacket* packet);
    virtual void handleCommandHelloFromCentral      (DCPPacket *packet);
    virtual void handleCommandHelloFromRemote       (DCPPacket *packet);
    virtual void handleCommandBye                   (DCPPacket* packet);
    virtual void handleCommandConnectToDrone        (DCPPacket* packet);
    virtual void handleCommandUnconnectFromDrone    (DCPPacket* packet);
};

/*
 * COMMAND STATION -- Packet Handler Not Connected
 * */
class DCPPacketHandlerCommandStationNotConnected : public DCPPacketHandlerInterface
{
public:
    DCPPacketHandlerCommandStationNotConnected(DCPServer *backend);

    virtual void handleNull                         (DCPPacket* packet);
    virtual void handleCommandAilerons              (DCPPacket* packet);
    virtual void handleCommandIsAlive               (DCPPacket* packet);
    virtual void handleCommandAck                   (DCPPacket* packet);
    virtual void handleCommandThrottle              (DCPPacket* packet);
    virtual void handleCommandSetSessID             (DCPPacket* packet);
    virtual void handleCommandUnsetSessID           (DCPPacket* packet);
    virtual void handleCommandHelloFromCentral      (DCPPacket *packet);
    virtual void handleCommandHelloFromRemote       (DCPPacket *packet);
    virtual void handleCommandBye                   (DCPPacket* packet);
    virtual void handleCommandConnectToDrone        (DCPPacket* packet);
    virtual void handleCommandUnconnectFromDrone    (DCPPacket* packet);
};

/*
 * CENTRAL STATION -- Packet Handler for central station normal operations
 * */
struct newRemote {
    QString         description;
    qint8           id;
    qint8           sessIdCentralStation;
    qint8           droneId;
    qint8           sessIdDrone;
    int             type;
    QHostAddress    addr;
    quint16         port;
    DCPCommandHelloFromCentralStation* myHello;
};

class DCPPacketHandlerCentralStation : public DCPPacketHandlerInterface
{
public:
    DCPPacketHandlerCentralStation(DCPServer *backend);

    virtual void handleNull                         (DCPPacket* packet);
    virtual void handleCommandAilerons              (DCPPacket* packet);
    virtual void handleCommandIsAlive               (DCPPacket* packet);
    virtual void handleCommandAck                   (DCPPacket* packet);
    virtual void handleCommandThrottle              (DCPPacket* packet);
    virtual void handleCommandSetSessID             (DCPPacket* packet);
    virtual void handleCommandUnsetSessID           (DCPPacket* packet);
    virtual void handleCommandHelloFromCentral      (DCPPacket *packet);
    virtual void handleCommandHelloFromRemote       (DCPPacket *packet);
    virtual void handleCommandBye                   (DCPPacket* packet);
    virtual void handleCommandConnectToDrone        (DCPPacket* packet);
    virtual void handleCommandUnconnectFromDrone    (DCPPacket* packet);

private:
    QList<struct newRemote*>    pendingRemotes;
    QList<struct newRemote*>    registeredRemotes;
    struct newRemote*           findNewRemoteByPacket(DCPPacket* packet);
    struct newRemote*           findRegisteredRemoteBySessId(qint8 sessId);
};

#endif // DCPPACKETHANDLERINTERFACE_H
