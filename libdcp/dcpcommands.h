/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpcommands.h -- bertrand
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

#ifndef DCPCOMMANDS_H
#define DCPCOMMANDS_H

#include <QByteArray>
#include <QList>
#include <QString>

#include <dcppacket.h>

class DCPPacket;
class DCPPacketHandlerInterface;


/*
 * DCP -- Ailerons command.
 * */
class DCPCommandAilerons : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandAilerons(qint8 sessID);
    DCPCommandAilerons(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8   aileronRight, aileronLeft, rudder;
};


/*
 * DCP -- Check if remote drone is alive.
 * */
class DCPCommandIsAlive : public DCPPacket
{
  friend class DCPPacketFactory;

public:
    DCPCommandIsAlive(qint8 sessID);
    DCPCommandIsAlive(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);
};

/*
 * DCP -- Ack of command.
 * */
class DCPCommandAck : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandAck(qint8 sessID);
    DCPCommandAck(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);
};

/*
 * DCP -- Throttle.
 * */
class DCPCommandThrottle : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandThrottle(qint8 sessID);
    DCPCommandThrottle(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8 motor, throttle;
};

/*
 * DCP -- Set session ID.
 * */
class DCPCommandSetSessID : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandSetSessID(qint8 sessID);
    DCPCommandSetSessID(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8   sessID;
};

/*
 * DCP -- Unset SessID.
 * */
class DCPCommandUnsetSessID : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandUnsetSessID(qint8 sessID);
    DCPCommandUnsetSessID(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8 sessID;
};

/*
 * DCP -- Hello.
 * */
enum DCPCommandHelloType {HelloFromCentralStation, HelloFromRemoteNode};

class DCPCommandHello : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandHello(qint8 sessID, DCPCommandHelloType type);
    DCPCommandHello(char* data, int len, DCPCommandHelloType type);
    void handle(DCPPacketHandlerInterface *handler);
    QList<QByteArray> getPayload();
    QList<QByteArray> getPayload(enum DCPCommandHelloType type);

    void setNodeDescription(QString description);
    void setIDs(qint8 sessIDWithCentralStation, qint8 IDRemoteNode);

protected:
    QByteArray buildPayload();
    int getPayloadLength();

private:
    enum DCPCommandHelloType type;
    QString     nodeDescription;
    qint8       sessIDWithCentralStation;
    qint8       IDRemoteNode;
};

/*
 * DCP -- Bye.
 * */
class DCPCommandBye : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandBye(qint8 sessID);
    DCPCommandBye(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);
};

/*
 * DCP -- Connect to Drone.
 * */
class DCPCommandConnectToDrone : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandConnectToDrone(qint8 sessID);
    DCPCommandConnectToDrone(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8 droneID;
};

/*
 * DCP -- Unconnect from Drone.
 * */
class DCPCommandUnconnectFromDrone : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandUnconnectFromDrone(qint8 sessID);
    DCPCommandUnconnectFromDrone(char* data, int len);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray buildPayload();
    void unbuildPayload();

private:
    qint8 droneID;
};

/*
 * DCP -- Packet Factory.
 * */
class DCPPacketFactory
{
public:
    static DCPPacket* commandPacketFromData(char *data, qint64 len);

};


#endif // DCPCOMMANDS_H
