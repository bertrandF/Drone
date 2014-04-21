/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpcommands.cpp -- bertrand
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

#include "dcpcommands.h"


/*
 * DCP -- Ailerons command.
 * */
DCPCommandAilerons::DCPCommandAilerons(qint8 sessID) :
    DCPPacket(DCP_CMDAILERON, sessID)
{}

DCPCommandAilerons::DCPCommandAilerons(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandAilerons::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandAilerons(this);
}

QByteArray DCPCommandAilerons::buildPayload()
{
    this->payload.clear();
    this->payload.append((char*)&(this->aileronLeft), 1);
    this->payload.append((char*)&(this->aileronRight), 1);
    this->payload.append((char*)&(this->rudder), 1);
    return this->payload;
}

void DCPCommandAilerons::unbuildPayload()
{
    if(this->payload.length() != 3) return;

    char* data = this->payload.data();
    this->aileronLeft   = data[0];
    this->aileronRight  = data[1];
    this->rudder    = data[2];
}


/*
 * DCP -- Check if remote drone is alive.
 * */
DCPCommandIsAlive::DCPCommandIsAlive(qint8 sessID) :
  DCPPacket(DCP_CMDISALIVE, sessID)
{}

DCPCommandIsAlive::DCPCommandIsAlive(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandIsAlive::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandIsAlive(this);
}


/*
 * DCP -- Ack of command.
 * */
DCPCommandAck::DCPCommandAck(qint8 sessID) :
    DCPPacket(DCP_CMDACK, sessID)
{}

DCPCommandAck::DCPCommandAck(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandAck::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandAck(this);
}


/*
 * DCP -- Throttle.
 * */
DCPCommandThrottle::DCPCommandThrottle(qint8 sessID) :
    DCPPacket(DCP_CMDTHROTTLE, sessID)
{}

DCPCommandThrottle::DCPCommandThrottle(char *data, int len) :
    DCPPacket(data,len)
{}

void DCPCommandThrottle::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandThrottle(this);
}

QByteArray DCPCommandThrottle::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->motor);
    this->payload.append((char)this->throttle);
    return this->payload;
}

void DCPCommandThrottle::unbuildPayload()
{
    if(this->payload.length() != 2) return;

    char* data      = this->payload.data();
    this->motor     = data[0];
    this->throttle  = data[1];
}


/*
 * DCP -- Set session ID.
 * */
DCPCommandSetSessID::DCPCommandSetSessID(qint8 sessID) :
    DCPPacket(DCP_CMDSETSESSID, sessID)
{}

DCPCommandSetSessID::DCPCommandSetSessID(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandSetSessID::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandSetSessID(this);
}

QByteArray DCPCommandSetSessID::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->sessID);
    return this->payload;
}

void DCPCommandSetSessID::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    char* data = this->payload.data();
    this->sessID = data[0];
}


/*
 * DCP -- Unset SessID.
 * */
DCPCommandUnsetSessID::DCPCommandUnsetSessID(qint8 sessID) :
    DCPPacket(DCP_CMDUNSETSESSID, sessID)
{}

DCPCommandUnsetSessID::DCPCommandUnsetSessID(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandUnsetSessID::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandUnsetSessID(this);
}

QByteArray DCPCommandUnsetSessID::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->sessID);
    return this->payload;
}

void DCPCommandUnsetSessID::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    char* data = this->payload.data();
    this->sessID = data[0];
}

/*
 * DCP -- Hello From Remote.
 * */
DCPCommandHelloFromRemote::DCPCommandHelloFromRemote(
        qint8 sessID) :
    DCPPacket(DCP_CMDHELLOFROMREMOTE, sessID)
{}

DCPCommandHelloFromRemote::DCPCommandHelloFromRemote(
        char* data, int len) :
    DCPPacket(data, len)
{
    this->description = this->description.fromUtf8(this->payload);
}

void DCPCommandHelloFromRemote::handle(
        DCPPacketHandlerInterface *handler)
{
    handler->handleCommandHelloFromRemote(this);
}

QByteArray DCPCommandHelloFromRemote::buildPayload()
{
    this->payload.clear();
    this->payload = this->description.toUtf8();
    return this->payload;
}

/*
 * DCP -- Hello From CommandStation.
 * */
DCPCommandHelloFromCentralStation::DCPCommandHelloFromCentralStation(
        qint8 sessID) :
    DCPPacket(DCP_CMDHELLOFROMCENTRAL, sessID)
{}

DCPCommandHelloFromCentralStation::DCPCommandHelloFromCentralStation(
        char* data, int len) :
    DCPPacket(data, len)
{
    this->sessIdCentralStation = (qint8)((this->payload.at(0)>>4) & 0x0F);
    this->IdRemote = (qint8)(this->payload.at(0) & 0x0F);
}

void DCPCommandHelloFromCentralStation::handle(
        DCPPacketHandlerInterface *handler)
{
    handler->handleCommandHelloFromCentral(this);
}

QByteArray DCPCommandHelloFromCentralStation::buildPayload()
{
    this->payload.clear();
    quint8 data = (quint8)((this->sessIdCentralStation & 0x0F)<<4 |
                 (this->IdRemote & 0x0F));
    this->payload.append((char)data);

    return this->payload;
}


/*
 * DCP -- Bye.
 * */
DCPCommandBye::DCPCommandBye(qint8 sessID) :
    DCPPacket(DCP_CMDBYE, sessID)
{}

DCPCommandBye::DCPCommandBye(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandBye::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandBye(this);
}

/*
 * DCP -- Connect to Drone.
 * */
DCPCommandConnectToDrone::DCPCommandConnectToDrone(qint8 sessID) :
    DCPPacket(DCP_CMDCONNECTTODRONE, sessID)
{}

DCPCommandConnectToDrone::DCPCommandConnectToDrone(char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandConnectToDrone::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandConnectToDrone(this);
}

QByteArray DCPCommandConnectToDrone::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->droneID);
    return this->payload;
}

void DCPCommandConnectToDrone::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    char* data = this->payload.data();
    this->droneID = data[0];
}


/*
 * DCP -- Unconnect from Drone.
 * */
DCPCommandUnconnectFromDrone::DCPCommandUnconnectFromDrone(qint8 sessID) :
    DCPPacket(DCP_CMDUNCONNECTFROMDRONE, sessID)
{}

DCPCommandUnconnectFromDrone::DCPCommandUnconnectFromDrone(
        char* data, int len) :
    DCPPacket(data, len)
{}

void DCPCommandUnconnectFromDrone::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandUnconnectFromDrone(this);
}

QByteArray DCPCommandUnconnectFromDrone::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->droneID);
    return this->payload;
}

void DCPCommandUnconnectFromDrone::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    char* data = this->payload.data();
    this->droneID = data[0];
}


/*
 * DCP -- Packet Factory.
 * */
DCPPacket* DCPPacketFactory::commandPacketFromData(char *data, qint64 len)
{
    qint8 cmdID     = (data[0]>>4) & (qint8)0x0F;

    DCPPacket* packet;
    switch(cmdID)
    {
    case DCP_CMDAILERON:
        packet = new DCPCommandAilerons(data, len);
        break;
    case DCP_CMDISALIVE:
        packet = new DCPCommandIsAlive(data, len);
        break;
    case DCP_CMDACK:
        packet = new DCPCommandAck(data, len);
        break;
    case DCP_CMDTHROTTLE:
        packet = new DCPCommandThrottle(data, len);
        break;
    case DCP_CMDSETSESSID:
        packet = new DCPCommandSetSessID(data, len);
        break;
    case DCP_CMDUNSETSESSID:
        packet = new DCPCommandUnsetSessID(data, len);
        break;
    case DCP_CMDHELLOFROMREMOTE:
        packet = new DCPCommandHelloFromRemote(data, len);
        break;
    case DCP_CMDHELLOFROMCENTRAL:
        packet = new DCPCommandHelloFromCentralStation(data, len);
        break;
    case DCP_CMDBYE:
        packet = new DCPCommandBye(data, len);
        break;
    case DCP_CMDCONNECTTODRONE:
        packet = new DCPCommandConnectToDrone(data, len);
        break;
    case DCP_CMDUNCONNECTFROMDRONE:
        packet = new DCPCommandUnconnectFromDrone(data, len);
        break;
    default:
        qDebug("Bad Packet cmdID.");
        return NULL;
        break;
    }

    return packet;
}
