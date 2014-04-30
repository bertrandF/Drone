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

DCPServerCentral::remote_t*
DCPServerCentral::addNewDrone(QHostAddress addr, quint16 port, QString info)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    // Insert new entry
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
        return NULL;
    }

    qWarning() << "Sucessfully added new Drone:" << query.lastQuery();
    remote->addr = addr;
    remote->port = port;
    remote->info = info;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBDRONESTABLE) +
                  " WHERE ip=? AND port=? AND info=?");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    remote->id      = query.value(0).toInt();
    remote->date    = query.value(1).toDateTime();

    return remote;
}

DCPServerCentral::remote_t*
DCPServerCentral::addNewCommandStation(QHostAddress addr, quint16 port,
                                       QString info)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    // Insert new entry
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
        return NULL;
    }

    qWarning() << "Sucessfully added new Command Station:" << query.lastQuery();
    remote->addr = addr;
    remote->port = port;
    remote->info = info;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " +
                  QString(DCP_DBCOMMANDSTATIONSTABLE) +
                  " WHERE ip=? AND port=? AND info=?");
    query.bindValue(0, addr.toString());
    query.bindValue(1, port);
    query.bindValue(2, info);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    remote->id      = query.value(0).toInt();
    remote->date    = query.value(1).toDateTime();

    return remote;
}

DCPServerCentral::session_t*
DCPServerCentral::addNewSession(qint8 iddrone, qint8 idcommand)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    // Insert new entry
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
        return NULL;
    }

    qWarning() << "Sucessfully added new Session:" << query.lastQuery();
    session->iddrone    = iddrone;
    session->idcommand  = idcommand;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSESSIONSTABLE) +
                  " WHERE iddrone=? AND idcommand=?");
    query.bindValue(0, iddrone);
    query.bindValue(1, idcommand);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    session->id      = query.value(0).toInt();
    session->date    = query.value(1).toDateTime();

    return session;
}

DCPServerCentral::session_central_t*
DCPServerCentral::addNewSessionCentralDrone(qint8 iddrone)
{
    DCPServerCentral::session_central_t* session =
            new DCPServerCentral::session_central_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONSCENTRALDRONES) + " " +
                  QString(DCP_DBSESSIONSDRONESCOLUMNS) + " VALUES (?)");
    query.bindValue(0, iddrone);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Session central/drone:"
               << query.lastQuery();
    session->idremote = iddrone;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " +
                  QString(DCP_DBSESSIONSCENTRALDRONES) +
                  " WHERE iddrone=?");
    query.bindValue(0, iddrone);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    session->id      = query.value(0).toInt();
    session->date    = query.value(1).toDateTime();

    return session;
}

DCPServerCentral::session_central_t*
DCPServerCentral::addNewSessionCentralCommand(qint8 idcommand)
{
    DCPServerCentral::session_central_t* session =
            new DCPServerCentral::session_central_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONSCENTRALCOMMANDS) + " " +
                  QString(DCP_DBSESSIONSCOMMANDSCOLUMNS) + " VALUES (?)");
    query.bindValue(0, idcommand);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Session central/command:"
               << query.lastQuery();
    session->idremote = idcommand;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " +
                  QString(DCP_DBSESSIONSCENTRALCOMMANDS) +
                  " WHERE idcommand=?");
    query.bindValue(0, idcommand);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    session->id      = query.value(0).toInt();
    session->date    = query.value(1).toDateTime();

    return session;
}

bool DCPServerCentral::deleteSessionForCommandId(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONSTABLE) +
                  " WHERE idcommand = ?");
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

bool DCPServerCentral::deleteSessionCentralForCommandId(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONSCENTRALCOMMANDS) +
                  " WHERE idcommand = ?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Session central/command:"
               << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteCommandById(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBCOMMANDSTATIONSTABLE) +
                  " WHERE id = ?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Command Station:" << query.lastQuery();
    return true;
}

DCPServerCentral::remote_t*
DCPServerCentral::getCommandFromCentralSessionId(qint8 id)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT idcommand, ip, port, command_stations.date, info "
                  "FROM command_stations "
                  "INNER JOIN sessions_central_commands "
                  "ON command_stations.id=sessions_central_commands.idcommand "
                  "WHERE sessions_central_commands.id=?");
    query.bindValue(0, id);
    if(!query.exec() || !query.next())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    remote->id      = query.value(0).toInt();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toInt();
    remote->date    = query.value(3).toDateTime();
    remote->info    = query.value(4).toString();

    return remote;
}
