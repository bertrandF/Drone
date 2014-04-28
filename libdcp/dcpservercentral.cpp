/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercentral.cpp -- bertrand
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

#include "dcpservercentral.h"
#include "dcp.h"
#include "dcpcommands.h"

#include <QSqlQuery>
#include <QSqlError>


DCPServerCentral::DCPServerCentral(QUdpSocket *sock, QSqlDatabase db) :
    DCPServer(sock),
    db(db)
{
    this->handler = new DCPPacketHandlerCentralStation(this);
}

qint8 DCPServerCentral::nextDroneId()
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

qint8 DCPServerCentral::nextCommandStationId()
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

qint8 DCPServerCentral::nextSessId()
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

bool DCPServerCentral::addNewRemote(
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
        qWarning() << "DCPServerCentral: Got request to add remote for"
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

bool DCPServerCentral::addNewSession(qint8 sessId, qint8 droneId,
                                     qint8 remoteId)
{
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONSTABLE) +
                  "VALUES (?, ?, ?, ?)");
    query.bindValue(0, sessId);
    query.bindValue(1, droneId);
    query.bindValue(2, remoteId);
    query.bindValue(3, QDateTime::currentDateTime().toString(Qt::ISODate));
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully added new session:" << query.lastQuery();
    return true;
}
