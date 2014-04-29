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

#include "dcppackethandlerinterface.h"
#include "dcpserver.h"
#include "dcpservercommand.h"
#include "dcpservercentral.h"
#include "dcpcommands.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Constructor
 * */
DCPPacketHandlerInterface::DCPPacketHandlerInterface(DCPServer *server) :
    server(server)
{}


/*
 * DEFAULT PACKET Zombie (alive but does nothing)
 * */
//DCPPacketHandlerZombie::DCPPacketHandlerZombie(DCPServerBackend *backend) :
//    DCPPacketHandlerInterface(backend)
//{}

//void DCPPacketHandlerZombie::handleNull(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandAilerons(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandIsAlive(DCPPacket *packet)
//{
//    if(packet->getSessionID() == this->backendSrv->getSessionIdDrone())
//    {
//        DCPCommandAck *response = new DCPCommandAck(
//                    this->backendSrv->getSessionIdDrone());
//        response->setTimestamp(packet->getTimestamp());
//        this->backendSrv->sendPacket(response);
//    }
//}

//void DCPPacketHandlerZombie::handleCommandAck(DCPPacket *packet)
//{
//    DCPPacket* packetAck;
//    if(packet->getSessionID() == this->backendSrv->getSessionIdDrone())
//    {
//        packetAck = this->backendSrv->findInAckQueue(packet->getTimestamp());
//        this->backendSrv->removeFromAckQueue(packetAck);
//    }
//}

//void DCPPacketHandlerZombie::handleCommandThrottle(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandSetSessID(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandUnsetSessID(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandHello(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandBye(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandConnectToDrone(DCPPacket *packet)
//{}

//void DCPPacketHandlerZombie::handleCommandUnconnectFromDrone(DCPPacket *packet)
//{}


/*
 * COMMAND STATION -- Packet Handler for Hello handshake
 * */
DCPPacketHandlerCommandStationHello::DCPPacketHandlerCommandStationHello(
        DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCommandStationHello::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandAck(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandHelloFromRemote(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandHelloFromCentral(
        DCPPacket *packet)
{
    qint8 sessIdCentral;
    qint8 IdCommand;
    DCPServerCommand *command =
            dynamic_cast<DCPServerCommand*> (this->server);
    if(packet->getSessionID() == DCP_IDNULL)
    {
        DCPPacket* myHello =
                command->findInAckQueue(packet->getTimestamp());
        if(myHello)
        {
            command->removeFromAckQueue(myHello);

            DCPCommandHelloFromCentralStation* hello =
                    dynamic_cast<DCPCommandHelloFromCentralStation*>(packet);
            sessIdCentral   = hello->getSessIdCentralStation();
            IdCommand       = hello->getIdRemote();

            DCPCommandAck *ack =
                    new DCPCommandAck(packet->getSessionID());
            ack->setTimestamp(packet->getTimestamp());
            ack->setAddrDst(packet->getAddrDst());
            ack->setPortDst(packet->getPortDst());
            command->sendPacket(ack);

            command->setMyId(IdCommand);
            command->setSessionIdCentralStation(sessIdCentral);
            command->setHandler(
                        new DCPPacketHandlerCommandStationNotConnected(command));
            command->setStatus(NotConnected);
        }
    }
}

void DCPPacketHandlerCommandStationHello::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandDisconnect(
        DCPPacket *packet)
{}


/*
 * COMMAND STATION -- Packet Handler Not Connected
 * */
DCPPacketHandlerCommandStationNotConnected::DCPPacketHandlerCommandStationNotConnected(
        DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCommandStationNotConnected::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandIsAlive(DCPPacket *packet)
{
    DCPServerCommand *command =
            dynamic_cast<DCPServerCommand*> (this->server);
    qint8 packetSessId = packet->getSessionID();

    if(packetSessId == command->getSessionIdDrone() ||
            packetSessId == command->getSessionIdCentralStation())
    {
        DCPCommandAck *ack = new DCPCommandAck(packetSessId);
        ack->setTimestamp(packet->getTimestamp());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        command->sendPacket(ack);
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandAck(DCPPacket *packet)
{
    DCPServerCommand *command =
            dynamic_cast<DCPServerCommand*> (this->server);
    qint8 packetSessId = packet->getSessionID();

    if(packetSessId == command->getSessionIdDrone() ||
            packetSessId == command->getSessionIdCentralStation())
    {
        command->removeFromAckQueue(
                    command->findInAckQueue(packet->getTimestamp()) );
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandSetSessID(DCPPacket *packet)
{
    DCPServerCommand *command =
            dynamic_cast<DCPServerCommand*> (this->server);
    DCPCommandSetSessID *sess =
            dynamic_cast<DCPCommandSetSessID*> (packet);
    qint8 packetSessId = packet->getSessionID();
    DCPCommandConnectToDrone *conn;

    if(packetSessId == command->getSessionIdCentralStation())
    {
        conn = dynamic_cast<DCPCommandConnectToDrone*>
                (command->findInAckQueue(packet->getTimestamp()));
        if(conn)
        {
            command->setSessionIdDrone(sess->getDroneSessId());
            command->setDroneId(conn->getDroneId());
            command->removeFromAckQueue(conn);

            DCPCommandAck *ack = new DCPCommandAck(packetSessId);
            ack->setAddrDst(packet->getAddrDst());
            ack->setPortDst(packet->getPortDst());
            ack->setTimestamp(packet->getTimestamp());
            command->sendPacket(ack);

            command->setHandler(
                        new DCPPacketHandlerCommandStationConnected(command));
            command->setStatus(Connected);
        }
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandHelloFromRemote(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandHelloFromCentral(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandDisconnect(
        DCPPacket *packet)
{}

/*
 * COMMAND STATION -- Packet Handler Connected
 * */
DCPPacketHandlerCommandStationConnected::DCPPacketHandlerCommandStationConnected(
        DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCommandStationConnected::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandAck(DCPPacket *packet)
{
    DCPServerCommand *command = dynamic_cast<DCPServerCommand*> (this->server);

    if(packet->getSessionID() == command->getSessionIdCentralStation())
    {
        DCPPacket* ackedPacket =
                command->findInAckQueue(packet->getTimestamp());
        if(ackedPacket != NULL)
        {
            switch(ackedPacket->getCommandID())
            {
            case DCP_CMDDISCONNECT:
                command->setDroneId(DCP_IDNULL);
                command->setSessionIdDrone(DCP_IDNULL);
                command->setHandler(
                            new DCPPacketHandlerCommandStationConnected(command));
                command->setStatus(Disconnected);
                break;
            default:
                break;
            }
            command->removeFromAckQueue(ackedPacket);
        }
    }
}

void DCPPacketHandlerCommandStationConnected::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandHelloFromRemote(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandHelloFromCentral(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandDisconnect(
        DCPPacket *packet)
{}


/*
 * CENTRAL STATION -- Packet Handler for central station normal operations
 * */
DCPPacketHandlerCentralStation::DCPPacketHandlerCentralStation(DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCentralStation::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandAck(DCPPacket *packet)
{
    DCPServerCentral *central = dynamic_cast<DCPServerCentral*> (this->server);
    struct newRemote* remote;

    // Hello Ack
    if(packet->getSessionID() == DCP_SESSIDCENTRAL)
    {
        DCPPacket* myHello;
        if((remote=findNewRemoteByPacket(packet))!=NULL &&
                (myHello=central->findInAckQueue(packet->getTimestamp()))!=NULL)
        {
            central->addNewRemote(
                        (DCPCommandHelloFromRemote::remoteType)remote->type,
                        remote->id, remote->addr, remote->port,
                        remote->description);
            this->pendingRemotes.removeOne(remote);
            this->registeredRemotes.append(remote);
            central->removeFromAckQueue(myHello);
        }
    }
    // Other commands Acks
    else
    {
        DCPPacket* ackedPacket;
        if((ackedPacket=central->findInAckQueue(packet->getTimestamp())) != NULL
                && (remote=this->findRegisteredRemoteBySessId(
                        packet->getSessionID())) != NULL)
        {
            switch(ackedPacket->getCommandID())
            {
            case DCP_CMDSETSESSID:
                central->addNewSession(remote->sessIdDrone, remote->droneId,
                                       remote->id);
                central->removeFromAckQueue(ackedPacket);
                break;
            default:
                break;
            }
            central->removeFromAckQueue(ackedPacket);
        }
    }
}

void DCPPacketHandlerCentralStation::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandHelloFromRemote(DCPPacket *packet)
{
    int clientID;
    int clientSessID ;

    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);
    if(packet->getSessionID() == DCP_SESSIDCENTRAL)
    {
        DCPCommandHelloFromRemote *hello =
                dynamic_cast<DCPCommandHelloFromRemote*>(packet);
        QString description(hello->getDescription());

        if(hello->getRemoteType() ==
                DCPCommandHelloFromRemote::remoteTypeCommandStation)
            clientID = central->nextCommandStationId();
        else if(hello->getRemoteType() ==
                DCPCommandHelloFromRemote::remoteTypeDrone)
            clientID = central->nextDroneId();
        clientSessID = central->nextSessId();
        // TODO: Handle no more ID avaliables

        DCPCommandHelloFromCentralStation *myHello =
                new DCPCommandHelloFromCentralStation(DCP_SESSIDCENTRAL);
        myHello->setIdRemote(clientID);
        myHello->setSessIdCentralStation(clientSessID);
        myHello->setAddrDst(packet->getAddrDst());
        myHello->setPortDst(packet->getPortDst());
        central->sendPacket(myHello);

        struct newRemote* remote = new newRemote();
        remote->description             = description;
        remote->id                      = clientID;
        remote->sessIdCentralStation    = clientSessID;
        remote->droneId                 = DCP_IDNULL;
        remote->sessIdDrone             = DCP_IDNULL;
        remote->type                    = hello->getRemoteType();
        remote->addr                    = hello->getAddrDst();
        remote->port                    = hello->getPortDst();
        remote->myHello                 = myHello;
        this->pendingRemotes.append(remote);
    }
}

void DCPPacketHandlerCentralStation::handleCommandHelloFromCentral(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandBye(DCPPacket *packet)
{
    struct newRemote *remote;
    DCPServerCentral *central = dynamic_cast<DCPServerCentral*> (this->server);
    if((remote = this->findRegisteredRemoteBySessId(packet->getSessionID()))
            != NULL)
    {
        DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        ack->setTimestamp(packet->getTimestamp());
        central->sendPacket(ack);

        if(remote->sessIdDrone != DCP_IDNULL)
            central->deleteSession(remote->sessIdDrone);
        this->registeredRemotes.removeOne(remote);
    }
}

void DCPPacketHandlerCentralStation::handleCommandConnectToDrone(DCPPacket *packet)
{
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*> (this->server);
    DCPCommandConnectToDrone *conn =
            dynamic_cast<DCPCommandConnectToDrone*> (packet);
    struct newRemote* remote;
    if((remote = this->findRegisteredRemoteBySessId(packet->getSessionID()))
            != NULL)
    {
        qint8 droneSessId = central->nextSessId();
        DCPCommandSetSessID *sess = new DCPCommandSetSessID(
                    packet->getSessionID());
        sess->setAddrDst(packet->getAddrDst());
        sess->setPortDst(packet->getPortDst());
        sess->setDroneSessId(droneSessId);
        sess->setTimestamp(packet->getTimestamp());
        central->sendPacket(sess);

        remote->sessIdDrone = droneSessId;
        remote->droneId     = conn->getDroneId();
    }
}

void DCPPacketHandlerCentralStation::handleCommandDisconnect(DCPPacket *packet)
{
    struct newRemote *remote;
    DCPServerCentral *central = dynamic_cast<DCPServerCentral*> (this->server);
    if((remote = this->findRegisteredRemoteBySessId(packet->getSessionID()))
            != NULL)
    {
        DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        ack->setTimestamp(packet->getTimestamp());
        central->sendPacket(ack);

        central->deleteSession(remote->sessIdDrone);
        remote->sessIdDrone = DCP_IDNULL;
        remote->droneId     = DCP_IDNULL;
    }
}

struct newRemote* DCPPacketHandlerCentralStation::findNewRemoteByPacket(
        DCPPacket *packet)
{
    struct newRemote *remote;
    foreach (remote, this->pendingRemotes) {
        if(remote->addr == packet->getAddrDst() &&
                remote->port == packet->getPortDst() &&
                remote->myHello->getTimestamp() == packet->getTimestamp())
            return remote;
    }
    return NULL;
}

struct newRemote* DCPPacketHandlerCentralStation::findRegisteredRemoteBySessId(
        qint8 sessId)
{
    struct newRemote *remote;
    foreach (remote, this->registeredRemotes) {
        if(remote->sessIdCentralStation == sessId)
            return remote;
    }
    return NULL;
}
