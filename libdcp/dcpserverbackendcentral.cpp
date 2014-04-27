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

#include <QSqlQuery>
#include <QSqlError>


DCPServerBackendCentral::DCPServerBackendCentral(qint8 sessID) :
    DCPServer(),
    sessID(sessID),
    myID(DCP_IDCENTRAL),
    droneSessId(DCP_IDNULL)
{
    this->handler = new DCPPacketHandlerCentralStationHello(this);
}

void DCPServerBackendCentral::registerNewBackendWithServer(
        DCPServerBackendCentral *central)
{
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
    QSqlQuery query("SELECT max(id) FROM sessions", this->db);
    if(query.next())
    {
        qint8 currentMax = (qint8)(query.value(0).toInt() & (int)0x0F);
        return ((currentMax>=0x0F) ?
                    DCP_DBNOAVALIABLEIDS : currentMax+1 );
    }
    return DCP_DBQUERYERROR;
}

bool DCPServerBackendCentral::addNewRemote(
        DCPCommandHelloFromRemote::remoteType type, qint8 id, QHostAddress addr,
        quint8 port, QString description)
{
    QString table;
    switch(type)
    {
    case DCPCommandHelloFromRemote::remoteTypeCommandStation:
        table = DCP_DBCOMMANDSTATIONSTABLE;
        break;
    case DCPCommandHelloFromRemote::remoteTypeDrone:
        table = DCP_DBDRONESTABLE;
        break;
    default:
        qWarning() << "DCPServerBackendCentral: Got request to add remote for"
                      "something not a drone nor a command station!";
        return false;
    }


    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + table +
                  " VALUES (?, ?, ?, ?, ?)");
    query.bindValue(0, id);
    query.bindValue(1, addr.toString());
    query.bindValue(2, port);
    query.bindValue(3, QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(4, description);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully added new remote:" << query.lastQuery();
    return true;
}
