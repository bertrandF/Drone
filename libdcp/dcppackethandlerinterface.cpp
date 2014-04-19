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
#include "dcpserverbackendremote.h"
#include "dcpserverbackendcentral.h"
#include "dcpcommands.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Constructor
 * */
DCPPacketHandlerInterface::DCPPacketHandlerInterface(
        DCPServerBackend *backend) :
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
DCPPacketHandlerHelloFromCS::DCPPacketHandlerHelloFromCS(
        DCPServerBackend *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerHelloFromCS::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandAck(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandHello(DCPPacket *packet)
{
    DCPServerBackendRemote *remote =
            dynamic_cast<DCPServerBackendRemote*> (this->backendSrv);
    if(packet->getSessionID() == DCP_IDNULL)
    {
        DCPPacket* myHello =
                remote->findInAckQueue(packet->getTimestamp());
        if(myHello)
        {
            remote->removeFromAckQueue(myHello);

            DCPCommandHello* hello = dynamic_cast<DCPCommandHello*>(packet);
            QList<QByteArray> list = hello->getPayload(HelloFromCentralStation);
            qint8 sessIDWithCentralStation = 4>>(list.at(0).data()[0] & 0xF0);
            qint8 IDRemoteNode  = list.at(1).data()[0] & 0x0F;

            DCPCommandAck *ack =
                    new DCPCommandAck(packet->getSessionID());
            ack->setTimestamp(packet->getTimestamp());
            remote->sendPacket(ack);

            remote->setMyId(IDRemoteNode);
            remote->setSessionIdCentralStation(
                        sessIDWithCentralStation);
            remote->setStatus(NotConnected);
        }
    }
}

void DCPPacketHandlerHelloFromCS::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerHelloFromCS::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}


/*
 * COMMAND STATION -- Packet Handler Not Connected
 * */
DCPPacketHandlerSelectDrone::DCPPacketHandlerSelectDrone(
        DCPServerBackend *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerSelectDrone::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandIsAlive(DCPPacket *packet)
{
    DCPServerBackendRemote *remote =
            dynamic_cast<DCPServerBackendRemote*> (this->backendSrv);
    if(packet->getSessionID() == remote->getSessionIdDrone())
    {
        DCPCommandAck *response = new DCPCommandAck(
                    remote->getSessionIdDrone());
        response->setTimestamp(packet->getTimestamp());
        remote->sendPacket(response);
    }
}

void DCPPacketHandlerSelectDrone::handleCommandAck(DCPPacket *packet)
{
    DCPServerBackendRemote *remote =
            dynamic_cast<DCPServerBackendRemote*> (this->backendSrv);
    DCPPacket* packetAck;
    if(packet->getSessionID() == remote->getSessionIdDrone())
    {
        packetAck = remote->findInAckQueue(packet->getTimestamp());
        remote->removeFromAckQueue(packetAck);
    }
}

void DCPPacketHandlerSelectDrone::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandSetSessID(DCPPacket *packet)
{

}

void DCPPacketHandlerSelectDrone::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandHello(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerSelectDrone::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}

/*
 * CENTRAL STATION -- Welcome new clients
 * */
DCPPacketHandlerWelcome::DCPPacketHandlerWelcome(DCPServerBackend *backend) :
    DCPPacketHandlerInterface(backend)
{}

void DCPPacketHandlerWelcome::handleNull(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandAilerons(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandIsAlive(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandAck(DCPPacket *packet)
{
    DCPCommandHello* hello = findSentHelloByTimestamp(packet->getTimestamp());
    if(hello != NULL)
    {
        QString description = this->sentHellos[hello];

        // TODO: add client to DB
    }
}

void DCPPacketHandlerWelcome::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandUnsetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandHello(DCPPacket *packet)
{
    DCPServerBackendCentral *central =
            dynamic_cast<DCPServerBackendCentral*>(this->backendSrv);
    if(packet->getSessionID() == central->getSessID())
    {
        DCPCommandHello *hello = dynamic_cast<DCPCommandHello*>(packet);
        QString description(hello->getPayload().at(0).data());

        // TODO: add client to db
        int clientID = 8;
        int clientSessID = 4;
        DCPCommandHello *myHello =
                new DCPCommandHello(central->getSessID(), HelloFromCentralStation);
        myHello->setIDs(clientSessID, clientID);
        myHello->setAddrDst(packet->getAddrDst());
        myHello->setPortDst(packet->getPortDst());

        central->sendPacket(myHello);
        this->sentHellos.insert(myHello,description);
    }
}

void DCPPacketHandlerWelcome::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerWelcome::handleCommandUnconnectFromDrone(
        DCPPacket *packet)
{}

DCPCommandHello* DCPPacketHandlerWelcome::findSentHelloByTimestamp(qint32 timestamp)
{
    DCPCommandHello *hello;
    foreach(hello, this->sentHellos.keys())
    {
        if(hello->getTimestamp() == timestamp)
            return hello;
    }
    return NULL;
}
