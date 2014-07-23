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

/* --- REMOTE TYPES --- */
#define DCP_REMOTETYPECOMMANDSTATION    ((char)'C')
#define DCP_REMOTETYPEDRONE             ((char)'D')
#define DCP_REMOTETYPENOTSET            ((char)'\0')

/* --- LOG LEVELS --- */
#define DCP_LOGLEVELINFO                ((char)'I')
#define DCP_LOGLEVELWARNING             ((char)'W')
#define DCP_LOGLEVELCRITICAL            ((char)'C')
#define DCP_LOGLEVELFATAL               ((char)'F')


/*
 * DCP -- Ailerons command.
 * */
DCPCommandAilerons::DCPCommandAilerons(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDAILERON, sessID, timestamp)
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

    char* data          = this->payload.data();
    this->aileronLeft   = data[0];
    this->aileronRight  = data[1];
    this->rudder        = data[2];
}

QString DCPCommandAilerons::toString()
{
    QString str("--- DCPCommandAilerons ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    text << "Left Aileron: " << this->aileronLeft << endl;
    text << "Right Aileron: " << this->aileronRight << endl;
    text << "Rudder: " << this->rudder << endl;

    return str;
}


/*
 * DCP -- Check if remote drone is alive.
 * */
DCPCommandIsAlive::DCPCommandIsAlive(qint8 sessID, qint32 timestamp) :
  DCPPacket(DCP_CMDISALIVE, sessID, timestamp)
{}

void DCPCommandIsAlive::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandIsAlive(this);
}

QString DCPCommandIsAlive::toString()
{
    QString str("--- DCPCommandIsAlive ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    return str;
}


/*
 * DCP -- Ack of command.
 * */
DCPCommandAck::DCPCommandAck(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDACK, sessID, timestamp)
{}

void DCPCommandAck::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandAck(this);
}

QString DCPCommandAck::toString()
{
    QString str("--- DCPCommandAck ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    return str;
}

/*
 * DCP -- Throttle.
 * */
DCPCommandThrottle::DCPCommandThrottle(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDTHROTTLE, sessID, timestamp)
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

QString DCPCommandThrottle::toString()
{
    QString str("--- DCPCommandThrottle ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    text << "Motor: " << this->motor << endl;
    text << "Throttle: " << this->throttle << endl;

    return str;
}


/*
 * DCP -- Set session ID.
 * */
DCPCommandSetSessID::DCPCommandSetSessID(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDSETSESSID, sessID, timestamp)
{}

void DCPCommandSetSessID::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandSetSessID(this);
}

QByteArray DCPCommandSetSessID::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->droneSessId);
    return this->payload;
}

void DCPCommandSetSessID::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    this->droneSessId   = this->payload.at(0);
}

QString DCPCommandSetSessID::toString()
{
    QString str("--- DCPCommandSetSessID ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "Drone Session Id: " << this->droneSessId << endl;

    return str;
}

/*
 * DCP -- Hello From Remote.
 * */
DCPCommandHelloFromRemote::DCPCommandHelloFromRemote(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDHELLOFROMREMOTE, sessID, timestamp),
    type(DCP_REMOTETYPENOTSET)
{}

void DCPCommandHelloFromRemote::handle(
        DCPPacketHandlerInterface *handler)
{
    handler->handleCommandHelloFromRemote(this);
}

QByteArray DCPCommandHelloFromRemote::buildPayload()
{
    this->payload.clear();
    this->payload.append(this->type);
    this->payload.append(this->description.toUtf8());
    return this->payload;
}

void DCPCommandHelloFromRemote::unbuildPayload()
{
    this->type          = this->payload.at(0);
    this->description   = QString::fromUtf8(this->payload.data()+1);
}

QString DCPCommandHelloFromRemote::toString()
{
    QString str("--- DCPCommandHelloFromRemote ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "remote type: ";
    switch(this->type)
    {
    case DCP_REMOTETYPECOMMANDSTATION:
        text << "remoteTypeCommandStation";
        break;
    case DCP_REMOTETYPEDRONE:
        text << "remoteTypeDrone";
        break;
    case DCP_REMOTETYPENOTSET:
    default:
        text << "remoteTypeNotSet";
        break;
    }
    text << endl;
    text << "description: " << this->description << endl;

    return str;
}

void DCPCommandHelloFromRemote::setRemoteType(enum remoteType type)
{
    switch(type)
    {
    case remoteTypeCommandStation:
        this->type = DCP_REMOTETYPECOMMANDSTATION;
        break;
    case remoteTypeDrone:
        this->type = DCP_REMOTETYPEDRONE;
        break;
    case remoteTypeNotSet:
    default:
        this->type = DCP_REMOTETYPENOTSET;
        break;
    }
}

DCPCommandHelloFromRemote::remoteType DCPCommandHelloFromRemote::getRemoteType()
{
    switch(this->type)
    {
    case DCP_REMOTETYPECOMMANDSTATION:
        return remoteTypeCommandStation;
    case DCP_REMOTETYPEDRONE:
        return remoteTypeDrone;
    case DCP_REMOTETYPENOTSET:
    default:
        return remoteTypeNotSet;
    }
}

/*
 * DCP -- Hello From CommandStation.
 * */
DCPCommandHelloFromCentralStation::DCPCommandHelloFromCentralStation(
        qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDHELLOFROMCENTRAL, sessID, timestamp)
{}

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

void DCPCommandHelloFromCentralStation::unbuildPayload()
{
    this->sessIdCentralStation  = (qint8)((this->payload.at(0)>>4) & 0x0F);
    this->IdRemote              = (qint8)(this->payload.at(0) & 0x0F);
}

QString DCPCommandHelloFromCentralStation::toString()
{
    QString str("--- DCPCommandHelloFromCentralStation ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "Sess Id central station: " << this->sessIdCentralStation << endl;
    text << "Remote Id: " << this->IdRemote << endl;

    return str;
}





/*
 * DCP -- Log
 * */
DCPCommandLog::DCPCommandLog(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDLOG, sessID, timestamp),
    level(DCP_LOGLEVELINFO)
{}

void DCPCommandLog::handle(
        DCPPacketHandlerInterface *handler)
{
    handler->handleCommandLog(this);
}

QByteArray DCPCommandLog::buildPayload()
{
    this->payload.clear();
    this->payload.append(this->level);
    this->payload.append(this->msg.toUtf8());
    return this->payload;
}

void DCPCommandLog::unbuildPayload()
{
    this->level = this->payload.at(0);
    this->msg   = QString::fromUtf8(this->payload.data()+1);
}

QString DCPCommandLog::toString()
{
    QString str("--- DCPCommandLog ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "Log Level: ";
    switch(this->level)
    {
    case DCP_LOGLEVELINFO:
        text << "Info";
        break;
    case DCP_LOGLEVELWARNING:
        text << "Warning";
        break;
    case DCP_LOGLEVELCRITICAL:
        text << "Critical";
        break;
    case DCP_LOGLEVELFATAL:
        text << "Fatal";
        break;
    default:
        text << "Bad log level";
        break;
    }
    text << endl;
    text << "msg: " << this->msg << endl;

    return str;
}

void DCPCommandLog::setLogLevel(enum logLevel level)
{
    switch(level)
    {
    case Info:
        this->level = DCP_LOGLEVELINFO;
        break;
    case Warning:
        this->level = DCP_LOGLEVELWARNING;
        break;
    case Critical:
        this->level = DCP_LOGLEVELCRITICAL;
        break;
    case Fatal:
        this->level = DCP_LOGLEVELFATAL;
        break;
    default:
        break;
    }
}

DCPCommandLog::logLevel DCPCommandLog::getLogLevel()
{
    switch(this->level)
    {
    case DCP_LOGLEVELINFO:
        return Info;
    case DCP_LOGLEVELWARNING:
        return Warning;
    case DCP_LOGLEVELCRITICAL:
        return Critical;
    case DCP_LOGLEVELFATAL:
        return Fatal;
    default:
        return Info;
    }
}


/*
 * DCP -- Bye.
 * */
DCPCommandBye::DCPCommandBye(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDBYE, sessID, timestamp)
{}

void DCPCommandBye::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandBye(this);
}

QString DCPCommandBye::toString()
{
    QString str("--- DCPCommandBye ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    return str;
}

/*
 * DCP -- Connect to Drone.
 * */
DCPCommandConnectToDrone::DCPCommandConnectToDrone(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDCONNECTTODRONE, sessID, timestamp)
{}

void DCPCommandConnectToDrone::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandConnectToDrone(this);
}

QByteArray DCPCommandConnectToDrone::buildPayload()
{
    this->payload.clear();
    this->payload.append((char)this->droneId);
    return this->payload;
}

void DCPCommandConnectToDrone::unbuildPayload()
{
    if(this->payload.length() != 1) return;

    this->droneId = this->payload.at(0);
}

QString DCPCommandConnectToDrone::toString()
{
    QString str("--- DCPCommandConnectToDrone ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "Drone Id: " << this->droneId;

    return str;
}



/*
 * DCP -- Discconnect / Kill session.
 * */
DCPCommandDisconnect::DCPCommandDisconnect(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDDISCONNECT, sessID, timestamp)
{}

void DCPCommandDisconnect::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandDisconnect(this);
}

QString DCPCommandDisconnect::toString()
{
    QString str("--- DCPCommandDisconnect ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();

    return str;
}

/*
 * DCP -- Declare video servers.
 * */
DCPCommandVideoServers::DCPCommandVideoServers(qint8 sessID, qint32 timestamp) :
    DCPPacket(DCP_CMDVIDEOSERVERS, sessID, timestamp)
{}

void DCPCommandVideoServers::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleCommandVideoServers(this);
}

QByteArray DCPCommandVideoServers::buildPayload()
{
    this->payload.clear();
    this->payload.append(
                this->urls.join(QChar(DCP_VIDEOSERVERSSEPARATOR)).toUtf8());
    return this->payload;
}

void DCPCommandVideoServers::unbuildPayload()
{
    this->urls = QString::fromUtf8(this->payload).
            split(QChar(DCP_VIDEOSERVERSSEPARATOR));
}

QString DCPCommandVideoServers::toString()
{
    QString str("--- DCPCommandVideoServers ---");
    QTextStream text(&str);
    text << endl;
    text << DCPPacket::toString();
    text << "URLs: " << this->urls.join(QChar(';')) << endl;

    return str;
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
        packet = new DCPCommandAilerons();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDISALIVE:
        packet = new DCPCommandIsAlive();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDACK:
        packet = new DCPCommandAck();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDTHROTTLE:
        packet = new DCPCommandThrottle();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDSETSESSID:
        packet = new DCPCommandSetSessID();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDHELLOFROMREMOTE:
        packet = new DCPCommandHelloFromRemote();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDHELLOFROMCENTRAL:
        packet = new DCPCommandHelloFromCentralStation();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDLOG:
        packet = new DCPCommandLog();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDBYE:
        packet = new DCPCommandBye();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDCONNECTTODRONE:
        packet = new DCPCommandConnectToDrone();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDDISCONNECT:
        packet = new DCPCommandDisconnect();
        packet->buildFromData(data, len);
        break;
    case DCP_CMDVIDEOSERVERS:
        packet = new DCPCommandVideoServers();
        packet->buildFromData(data, len);
        break;
    default:
        qDebug("Bad Packet cmdID.");
        return NULL;
    }

    return packet;
}
