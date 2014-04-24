/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserverbackendcentral.h -- bertrand
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

#include "dcpserverbackendcentral.h"
#include "dcp.h"
#include "dcpcommands.h"
#include "dcpserver.h"

#include <QSqlQuery>

DCPServerBackendCentral::DCPServerBackendCentral(qint8 sessID) :
    DCPServerBackend(),
    sessID(sessID),
    myID(DCP_IDCENTRAL),
    droneSessId(DCP_IDNULL)
{
    this->handler = new DCPPacketHandlerCentralStationHello(this);
}

void DCPServerBackendCentral::registerWithServer(DCPServer *srv)
{
    this->srv = srv;
    srv->registerBackend(this, this->sessID);
}

void DCPServerBackendCentral::registerNewBackendWithServer(
        DCPServerBackendCentral *central)
{
    central->registerWithServer(this->srv);
}

qint8 DCPServerBackendCentral::nextDroneId()
{
    QSqlQuery query("SELECT max(id) FROM drones", this->db);
    if(query.next())
    {
        qint8 currentMax = (qint8)(query.value(0).toInt() & (int)0x0F);
        return ((currentMax>=0x0F) ?
                    DCP_DBNOAVALIABLEIDS : currentMax+1 );
    }
    return DCP_DBQUERYERROR;
}

qint8 DCPServerBackendCentral::nextCommandStationId()
{
    QSqlQuery query("SELECT max(id) FROM command_stations", this->db);
    if(query.next())
    {
        qint8 currentMax = (qint8)(query.value(0).toInt() & (int)0x0F);
        return ((currentMax>=0x0F) ?
                    DCP_DBNOAVALIABLEIDS : currentMax+1 );
    }
    return DCP_DBQUERYERROR;
}

qint8 DCPServerBackendCentral::nextSessId()
{
    QSqlQuery query("SELECT max(id) FROM drones", this->db);
    if(query.next())
    {
        qint8 currentMax = (qint8)(query.value(0).toInt() & (int)0x0F);
        return ((currentMax>=0x0F) ?
                    DCP_DBNOAVALIABLEIDS : currentMax+1 );
    }
    return DCP_DBQUERYERROR;
}
