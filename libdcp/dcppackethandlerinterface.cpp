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

void DCPPacketHandlerCommandStationHello::handleCommandLog(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandDisconnect(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationHello::handleCommandVideoServers(DCPPacket *packet)
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

    if(packetSessId == command->getSessionIdCentralStation())
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

void DCPPacketHandlerCommandStationNotConnected::handleCommandLog(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandDisconnect(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationNotConnected::handleCommandVideoServers(DCPPacket *packet)
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
{
    DCPServerCommand *command =
            dynamic_cast<DCPServerCommand*> (this->server);
    qint8 packetSessId = packet->getSessionID();

    if(packetSessId == command->getSessionIdCentralStation() ||
            packetSessId == command->getSessionIdDrone())
    {
        DCPCommandAck *ack = new DCPCommandAck(packetSessId);
        ack->setTimestamp(packet->getTimestamp());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        command->sendPacket(ack);
    }
}

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
            case DCP_CMDBYE:
                command->setDroneId(DCP_IDNULL);
                command->setSessionIdDrone(DCP_IDNULL);
                command->setSessionIdCentralStation(DCP_IDCENTRAL);
                command->setHandler(
                            new DCPPacketHandlerCommandStationHello(command));
                command->setStatus(Stopped);
                break;
            default:
                break;
            }
            command->removeFromAckQueue(ackedPacket);
        }
    }
    else if(packet->getSessionID() == command->getSessionIdDrone())
    {
        DCPPacket* ackedPacket =
                command->findInAckQueue(packet->getTimestamp());
        if(ackedPacket != NULL)
        {
            switch(ackedPacket->getCommandID())
            {
            case DCP_CMDISALIVE:
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

void DCPPacketHandlerCommandStationConnected::handleCommandLog(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandBye(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandConnectToDrone(DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandDisconnect(
        DCPPacket *packet)
{}

void DCPPacketHandlerCommandStationConnected::handleCommandVideoServers(DCPPacket *packet)
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
    DCPPacket *mypacket = central->findInAckQueue(packet->getTimestamp());

    central->removeFromAckQueue(mypacket);
}

void DCPPacketHandlerCentralStation::handleCommandThrottle(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandSetSessID(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandHelloFromRemote(DCPPacket *packet)
{
    DCPServerCentral::remote_t *remote;
    DCPServerCentral::session_t *session;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);

    // Check is usind default central sessId
    if(packet->getSessionID() == DCP_SESSIDCENTRAL)
    {
        DCPCommandHelloFromRemote *hello =
                dynamic_cast<DCPCommandHelloFromRemote*>(packet);
        if(!hello) return; // Not Hello from remote packet = abort

        // Switch on remote type
        switch(hello->getRemoteType())
        {
        case DCPCommandHelloFromRemote::remoteTypeCommandStation:
            remote = central->addNewCommandStation(hello->getAddrDst(),
                                          hello->getPortDst(),
                                          hello->getDescription());
            break;
        case DCPCommandHelloFromRemote::remoteTypeDrone:
            remote = central->addNewDrone(hello->getAddrDst(),
                                          hello->getPortDst(),
                                          hello->getDescription());
            break;
        default:
            // TODO: Unknwon type
            return; // Abort
        }

        if(!remote)
        {
            // Could not add remote
            return;
        }

        // If session Id is available
        if((session = central->addNewSession(central->getMyId(), remote->id))
                != NULL)
        {
            DCPCommandHelloFromCentralStation *myHello =
               new DCPCommandHelloFromCentralStation(DCP_SESSIDCENTRAL);
            myHello->setTimestamp(packet->getTimestamp());
            myHello->setIdRemote(remote->id);
            myHello->setSessIdCentralStation(session->id);
            myHello->setAddrDst(packet->getAddrDst());
            myHello->setPortDst(packet->getPortDst());
            central->sendPacket(myHello);
        }
    }
}

void DCPPacketHandlerCentralStation::handleCommandHelloFromCentral(DCPPacket *packet)
{}

void DCPPacketHandlerCentralStation::handleCommandLog(DCPPacket *packet)
{
    int remoteId;
    DCPServerCentral::session_t *sessionCentral;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);
    DCPCommandLog *log = dynamic_cast<DCPCommandLog*> (packet);

    // sessId is valid to speak with central station ?
    if((sessionCentral = central->sessionIsCentral(packet->getSessionID()))
            != NULL)
    {
        remoteId = (sessionCentral->station1==0) ? sessionCentral->station2 :
                                                   sessionCentral->station1;

        DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        ack->setTimestamp(packet->getTimestamp());
        central->sendPacket(ack);
    }
}

void DCPPacketHandlerCentralStation::handleCommandBye(DCPPacket *packet)
{
    int station1Id, station2Id;
    DCPServerCentral::remote_t *station2;
    DCPServerCentral::session_t *sessionCentral;
    DCPServerCentral::session_t *sessionDrone;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);
    DCPCommandBye *bye =
            dynamic_cast<DCPCommandBye*> (packet);

    // sessId is valid to speak with central station ?
    if((sessionCentral = central->sessionIsCentral(packet->getSessionID()))
            != NULL)
    {
        station1Id = (sessionCentral->station1==0) ? sessionCentral->station2 :
                                                   sessionCentral->station1;

        // Is remote connected to something ?
        if((sessionDrone=central->getDroneSessionForStation(station1Id)) != NULL)
        {
            station2Id = (sessionDrone->station1==station1Id) ? sessionDrone->station2 :
                                                                sessionDrone->station1;
            station2 = central->getStation(station2Id);
            central->deleteSession(sessionDrone->id);

            // Send info to other end of the connection
            DCPCommandDisconnect *disconn = new DCPCommandDisconnect(
                        central->getCentralSessionForStation(station2Id)->id
                        );
            disconn->setAddrDst(station2->addr);
            disconn->setPortDst(station2->port);
            disconn->setTimestamp(central->msecSinceStart());
            central->sendPacket(disconn);
        }

        // If problem while deleting
        if(!central->deleteSession(sessionCentral->id) ||
                !central->deleteStationById(station1Id))
        {
            // TODO: Handle problem
        }

        // Even if we could not delete we say farewell to the drone/command
        DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
        ack->setAddrDst(packet->getAddrDst());
        ack->setPortDst(packet->getPortDst());
        ack->setTimestamp(packet->getTimestamp());
        central->sendPacket(ack);
    }
}

void DCPPacketHandlerCentralStation::handleCommandConnectToDrone(DCPPacket *packet)
{
    int remoteId;
    DCPServerCentral::remote_t *command;
    DCPServerCentral::remote_t *drone;
    DCPServerCentral::session_t *sessionCentral;
    DCPServerCentral::session_t *sessionDrone;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);
    DCPCommandConnectToDrone *conn =
            dynamic_cast<DCPCommandConnectToDrone*> (packet);

    // sessId is valid to speak with central station ?
    if((sessionCentral = central->sessionIsCentral(packet->getSessionID()))
            != NULL)
    {

        remoteId = (sessionCentral->station1==0) ? sessionCentral->station2 :
                                                   sessionCentral->station1;
        // Only command stations can connect to drones
        if((command=central->stationIsCommand(remoteId)) != NULL)
        {
            // Can only connect to drone
            if((drone=central->stationIsDrone(conn->getDroneId())) != NULL)
            {
                if(central->getDroneSessionForStation(remoteId))
                {
                    // TODO: Command Station already connected
                }
                else if(central->getDroneSessionForStation(drone->id))
                {
                    // TODO: Drone already connected
                }
                else
                {
                    sessionDrone = central->addNewSession(remoteId, drone->id);

                    // Send to Drone
                    DCPCommandSetSessID *setSessDrone =
                            new DCPCommandSetSessID(
                                central->getCentralSessionForStation(drone->id)->id
                                );
                    setSessDrone->setAddrDst(drone->addr);
                    setSessDrone->setPortDst(drone->port);
                    setSessDrone->setTimestamp(central->msecSinceStart());
                    setSessDrone->setDroneSessId(sessionDrone->id);
                    central->sendPacket(setSessDrone);

                    // Send to Command Station
                    DCPCommandSetSessID *setSessCmd =
                            new DCPCommandSetSessID(packet->getSessionID());
                    setSessCmd->setAddrDst(packet->getAddrDst());
                    setSessCmd->setPortDst(packet->getPortDst());
                    setSessCmd->setTimestamp(packet->getTimestamp());
                    setSessCmd->setDroneSessId(sessionDrone->id);
                    central->sendPacket(setSessCmd);
                }
            }
        }
    }
}

void DCPPacketHandlerCentralStation::handleCommandDisconnect(DCPPacket *packet)
{
    int station1Id, station2Id;
    DCPServerCentral::remote_t  *station2;
    DCPServerCentral::session_t *sessionCentral;
    DCPServerCentral::session_t *sessionDrone;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);

    // SessionId exists and is with central station ?
    if((sessionCentral = central->sessionIsCentral(packet->getSessionID()))
            != NULL)
    {
        station1Id = (sessionCentral->station1==0) ? sessionCentral->station2 :
                                                   sessionCentral->station1;

        // Is remote connected to something ?
        if((sessionDrone=central->getDroneSessionForStation(station1Id)) != NULL)
        {
            station2Id = (sessionDrone->station1==station1Id) ? sessionDrone->station2 :
                                                            sessionDrone->station1;
            station2 = central->getStation(station2Id);
            central->deleteSession(sessionDrone->id);

            // Send disconnect to other end
            DCPCommandDisconnect *disconn = new DCPCommandDisconnect(
                        central->getCentralSessionForStation(station2Id)->id
                        );
            disconn->setAddrDst(station2->addr);
            disconn->setPortDst(station2->port);
            disconn->setTimestamp(central->msecSinceStart());
            central->sendPacket(disconn);

            // Disconnect OK
            DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
            ack->setAddrDst(packet->getAddrDst());
            ack->setPortDst(packet->getPortDst());
            ack->setTimestamp(packet->getTimestamp());
            central->sendPacket(ack);
        }
    }
}

void DCPPacketHandlerCentralStation::handleCommandVideoServers(DCPPacket *packet)
{
    int remoteId;
    DCPServerCentral::session_t *sessionCentral;
    DCPServerCentral::remote_t  *drone;
    DCPServerCentral *central =
            dynamic_cast<DCPServerCentral*>(this->server);
    DCPCommandVideoServers *video =
            dynamic_cast<DCPCommandVideoServers*> (packet);

    // SessionId exists and is with central station ?
    if((sessionCentral = central->sessionIsCentral(packet->getSessionID()))
            != NULL)
    {
        remoteId = (sessionCentral->station1==0) ? sessionCentral->station2 :
                                                   sessionCentral->station1;

        // Only drones can register video servers
        if((drone=central->stationIsDrone(remoteId)) != NULL)
        {
            // Delete previously registered video servers if any
            central->deleteVideoServers(remoteId);
            // Register new servers
            if(central->addNewVideoServers(remoteId, video->getRawUrlList()))
            {
                DCPCommandAck *ack = new DCPCommandAck(packet->getSessionID());
                ack->setAddrDst(packet->getAddrDst());
                ack->setPortDst(packet->getPortDst());
                ack->setTimestamp(packet->getTimestamp());
                central->sendPacket(ack);
            }
        }
    }
}
