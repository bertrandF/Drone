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
    DCPCommandAilerons(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

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
    DCPCommandIsAlive(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);
};

/*
 * DCP -- Ack of command.
 * */
class DCPCommandAck : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandAck(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

    QString toString();
};

/*
 * DCP -- Throttle.
 * */
class DCPCommandThrottle : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandThrottle(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

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
    DCPCommandSetSessID(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

    inline void setDroneSessId(qint8 id)
        { this->droneSessId = id; }
    inline qint8 getDroneSessId()
        { return this->droneSessId; }

    QString toString();

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

private:
    qint8   droneSessId;
};


/*
 * DCP -- Hello From Remote Node
 * */

class DCPCommandHelloFromRemote : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    enum remoteType {
        remoteTypeDrone, remoteTypeCommandStation, remoteTypeNotSet
    };

    DCPCommandHelloFromRemote(qint8 sessID=DCP_SESSIDCENTRAL,
                              qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

    inline void setDescription(QString description)
        { this->description = description; }
    inline QString getDescription()
        { return this->description; }
    void setRemoteType(enum remoteType type);
    enum remoteType getRemoteType();

    QString toString();

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

private:
    char        type;
    QString     description;
};

/*
 * DCP -- Hello From Central Station
 * */

class DCPCommandHelloFromCentralStation : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandHelloFromCentralStation(qint8 sessID=DCP_SESSIDCENTRAL,
                                      qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

    inline void setSessIdCentralStation(qint8 sessId)
        { this->sessIdCentralStation = sessId; }
    inline void setIdRemote(qint8 id)
        { this->IdRemote = id; }
    inline qint8 getSessIdCentralStation()
        { return this->sessIdCentralStation; }
    inline qint8 getIdRemote()
        { return this->IdRemote; }

    QString toString();

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

private:
    qint8       sessIdCentralStation;
    qint8       IdRemote;
};





/*
 * DCP -- Bye.
 * */
class DCPCommandBye : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandBye(qint8 sessID=DCP_SESSIDCENTRAL, qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);
};

/*
 * DCP -- Connect to Drone.
 * */
class DCPCommandConnectToDrone : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandConnectToDrone(qint8 sessID=DCP_SESSIDCENTRAL,
                             qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);

    inline void setDroneId(qint8 id)
        { this->droneId = id; }
    inline qint8 getDroneId()
        { return this->droneId; }

    QString toString();

protected:
    QByteArray  buildPayload();
    void        unbuildPayload();

private:
    qint8 droneId;
};

/*
 * DCP -- Discconnect / Kill session.
 * */
class DCPCommandDisconnect : public DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPCommandDisconnect(qint8 sessID=DCP_SESSIDCENTRAL,
                                 qint32 timestamp=0);
    void handle(DCPPacketHandlerInterface *handler);
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
