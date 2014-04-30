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

bool DCPServerCentral::addNewDrone(QHostAddress addr, quint8 port, QString info)
{
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBDRONESTABLE) + " " +
                  QString(DCP_DBDRONESCOLUMNS) + " VALUES (?, ?, ?)");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully added new Drone:" << query.lastQuery();
    return true;
}

bool DCPServerCentral::addNewCommandStation(QHostAddress addr, quint8 port,
                                            QString info)
{
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBCOMMANDSTATIONSTABLE) + " " +
                  QString(DCP_DBCOMMANDSCOLUMNS) + " VALUES (?, ?, ?)");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully added new Command Station:" << query.lastQuery();
    return true;
}

bool DCPServerCentral::addNewSession(qint8 iddrone, qint8 idcommand)
{
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONSTABLE) + " " +
                  QString(DCP_DBSESSIONSCOLUMNS) + " VALUES (?, ?)");
    query.bindValue(0, iddrone);
    query.bindValue(1, idcommand);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully added new Session:" << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteSession(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONSTABLE) +
                  " WHERE id = ?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Session:" << query.lastQuery();
    return true;
}
