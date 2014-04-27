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
#include "dcpserverbackendremote.h"
#include "dcpservercentral.h"
#include "dcpcommands.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Constructor
 * */
DCPPacketHandlerInterface::DCPPacketHandlerInterface(
        DCPServer *backend) :
    backendSrv(backend)
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

void DCPPacketHandlerCommandStationHello::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandHelloFromRemote(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandHelloFromCentral(
        DCPPacket *packet)
{
    DCPServerCommand *remote =
            dynamic_cast<DCPServerCommand*> (this->backendSrv);
    if(packet->getSessionID() == DCP_IDNULL)
    {
        DCPPacket* myHello =
                remote->findInAckQueue(packet->getTimestamp());
        if(myHello)
        {
            remote->removeFromAckQueue(myHello);

            DCPCommandHelloFromCentralStation* hello =
                    dynamic_cast<DCPCommandHelloFromCentralStation*>(packet);
            qint8 sessIdCentralStation = hello->getSessIdCentralStation();
            qint8 IdRemote  = hello->getIdRemote();

            DCPCommandAck *ack =
                    new DCPCommandAck(packet->getSessionID());
            ack->setTimestamp(packet->getTimestamp());
            ack->setAddrDst(packet->getAddrDst());
            ack->setPortDst(packet->getPortDst());
            remote->sendPacket(ack);

            remote->setMyId(IdRemote);
            remote->setSessionIdCentralStation(
                        sessIdCentralStation);

            remote->setStatus(NotConnected);
            remote->setHandler(new DCPPacketHandlerCommandStationNotConnected(remote));
        }
    }
}

void DCPPacketHandlerCommandStationHello::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandUnconnectFromDrone(
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
    DCPServerCommand *remote =
            dynamic_cast<DCPServerCommand*> (this->backendSrv);
    if(packet->getSessionID() == remote->getSessionIdDrone() ||
            packet->getSessionID() == remote->getSessionIdCentralStation())
    {
        DCPCommandAck *response = new DCPCommandAck(
                    packet->getSessionID());
        response->setTimestamp(packet->getTimestamp());
        response->setAddrDst(packet->getAddrDst());
        response->setPortDst(packet->getPortDst());
        remote->sendPacket(response);
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandAck(DCPPacket *packet)
{
    DCPServerCommand *remote =
            dynamic_cast<DCPServerCommand*> (this->backendSrv);
    DCPPacket* packetAck;
    if(packet->getSessionID() == remote->getSessionIdDrone())
    {
        packetAck = remote->findInAckQueue(packet->getTimestamp());
        remote->removeFromAckQueue(packetAck);
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandSetSessID(DCPPacket *packet)
{
    DCPServerCommand *remote =
            dynamic_cast<DCPServerCommand*> (this->backendSrv);
    DCPCommandSetSessID *sess =
            dynamic_cast<DCPCommandSetSessID*> (packet);
    DCPCommandConnectToDrone *conn;
    if(packet->getSessionID() == remote->getSessionIdCentralStation())
    {
        conn = dynamic_cast<DCPCommandConnectToDrone*>
                (remote->findInAckQueue(packet->getTimestamp()));
        if(conn)
        {
            remote->setSessionIdDrone(sess->getDroneSessId());
            remote->removeFromAckQueue(conn);
            DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
            ack->setAddrDst(packet->getAddrDst());
            ack->setPortDst(packet->getPortDst());
            ack->setTimestamp(packet->getTimestamp());
            remote->sendPacket(ack);

            remote->setStatus(Connected);
        }
    }
}

void DCPPacketHandlerCommandStationNotConnected::handleCommandUnsetSessID(DCPPacket *packet)
{}

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

void DCPPacketHandlerCommandStationNotConnected::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}

/*
 * CENTRAL STATION -- Welcome new clients
 * */
DCPPacketHandlerCentralStationHello::DCPPacketHandlerCentralStationHello(DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCentralStationHello::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandAck(DCPPacket *packet)
{
    struct newRemote* remote = findNewRemoteByTimestamp(packet->getTimestamp());
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->backendSrv);
    if(remote != NULL)
    {
        central->addNewRemote(
                    (DCPCommandHelloFromRemote::remoteType)remote->type,
                    remote->id, remote->myHello->getAddrDst(),
                    remote->myHello->getPortDst(), remote->description);

        DCPServerCentral* newBackend =
                new DCPServerCentral(remote->sessIdCentralStation);
        newBackend->setHandler(
                    new DCPPacketHandlerCentralStationWaitConnectRequest(newBackend));
        central->registerNewBackendWithServer(newBackend);
    }
}

void DCPPacketHandlerCentralStationHello::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandHelloFromRemote(DCPPacket *packet)
{
    int clientID;
    int clientSessID ;

    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->backendSrv);
    if(packet->getSessionID() == central->getSessID())
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
                new DCPCommandHelloFromCentralStation(central->getSessID());
        myHello->setIdRemote(clientID);
        myHello->setSessIdCentralStation(clientSessID);
        myHello->setAddrDst(packet->getAddrDst());
        myHello->setPortDst(packet->getPortDst());

        central->sendPacket(myHello);

        struct newRemote* remote = new newRemote();
        remote->description = description;
        remote->id = clientID;
        remote->sessIdCentralStation = clientSessID;
        remote->type = hello->getRemoteType();
        remote->myHello = myHello;
        this->pendingRemote.append(remote);
    }
}

void DCPPacketHandlerCentralStationHello::handleCommandHelloFromCentral(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationHello::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}

struct newRemote *DCPPacketHandlerCentralStationHello::findNewRemoteByTimestamp(
        qint32 timestamp)
{
    struct newRemote *remote;
    foreach(remote, this->pendingRemote)
    {
        if(remote->myHello->getTimestamp() == timestamp)
            return remote;
    }
    return NULL;
}


/*
 * CENTRAL STATION -- Central station Wait connect request from command station
 * */
DCPPacketHandlerCentralStationWaitConnectRequest::
    DCPPacketHandlerCentralStationWaitConnectRequest(
        DCPServer *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandAck(DCPPacket *packet)
{
    DCPServerCentral *central = dynamic_cast<DCPServerCentral*>
            (this->backendSrv);
    DCPCommandSetSessID *sess = dynamic_cast<DCPCommandSetSessID*>
            (central->findInAckQueue(packet->getTimestamp()));
    if(sess)
    {
        central->setDroneSessId(sess->getDroneSessId());
        central->removeFromAckQueue(sess);
    }
}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandHelloFromRemote(
        DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandHelloFromCentral(
        DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandConnectToDrone(
        DCPPacket *packet)
{
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*> (this->backendSrv);
    DCPCommandConnectToDrone *conn =
            dynamic_cast<DCPCommandConnectToDrone*> (packet);
    if(packet->getSessionID() == central->getSessID())
    {
        // TODO: check with DB & get next sessID

        qint8 droneSessId = 9;
        DCPCommandSetSessID *sess = new DCPCommandSetSessID(
                    central->getSessID());
        sess->setAddrDst(packet->getAddrDst());
        sess->setPortDst(packet->getPortDst());
        sess->setDroneSessId(droneSessId);
        sess->setTimestamp(packet->getTimestamp());
        central->sendPacket(sess);
    }
}

void DCPPacketHandlerCentralStationWaitConnectRequest::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}
