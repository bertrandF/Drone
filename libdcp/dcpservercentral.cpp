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

#define PINGDRONES_TIMEOUT      (3000)

DCPServerCentral::DCPServerCentral(QUdpSocket *sock, QSqlDatabase db) :
    DCPServer(sock),
    db(db)
{
    this->myID = DCP_IDCENTRAL;
    this->handler = new DCPPacketHandlerCentralStation(this);
    this->pingDronesTimer.start(PINGDRONES_TIMEOUT);
    connect(&(this->pingDronesTimer), SIGNAL(timeout()),
            this, SLOT(pingDrones()));
}

DCPServerCentral::remote_t*
DCPServerCentral::addNewDrone(QHostAddress addr, quint16 port, QString info)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSTATIONS) +
                  " (type, ip, port, info) VALUES ('drone', ?, ?, ?)");
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
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSTATIONS) +
                  " WHERE type='drone' AND ip=? AND port=? AND info=?");
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
    if(!query.next())
    {
        // TODO: log ?
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
    query.prepare("INSERT INTO " + QString(DCP_DBSTATIONS) +
                  " (type, ip, port, info) VALUES ('command', ?, ?, ?)");
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
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSTATIONS) +
                  " WHERE type='command' AND ip=? AND port=? AND info=?");
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
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }
    remote->id      = query.value(0).toInt();
    remote->date    = query.value(1).toDateTime();

    return remote;
}

bool DCPServerCentral::addNewVideoServers(qint8 id, QString videoServers)
{

    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBVIDEOSERVERS) +
                  " (id, videos) VALUES (?, ?)");
    query.bindValue(0, id);
    query.bindValue(1, videoServers);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }
    qWarning() << "Sucessfully added new Video servers:" << query.lastQuery();
    return true;
}

DCPServerCentral::session_t*
DCPServerCentral::addNewSession(qint8 station1, qint8 station2)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    // Insert new entry
    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBSESSIONS) +
                  " (station1, station2) VALUES (?, ?)");
    query.bindValue(0, station1);
    query.bindValue(1, station2);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }

    qWarning() << "Sucessfully added new Session:" << query.lastQuery();
    session->station1   = station1;
    session->station2   = station2;

    // Get new entry id and date
    query.clear();
    query.prepare("SELECT id, date FROM " + QString(DCP_DBSESSIONS) +
                  " WHERE station1=? AND station2=?");
    query.bindValue(0, station1);
    query.bindValue(1, station2);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }
    session->id      = query.value(0).toInt();
    session->date    = query.value(1).toDateTime();

    return session;
}

bool DCPServerCentral::addNewLog(qint8 id, DCPCommandLog::logLevel level,
                                 QString msg)
{
    QString levelStr;
    switch(level)
    {
    case DCPCommandLog::Info:
        levelStr = QString(DCP_DBLOGINFO);
        break;
    case DCPCommandLog::Warning:
        levelStr = QString(DCP_DBLOGWARNING);
        break;
    case DCPCommandLog::Critical:
        levelStr = QString(DCP_DBLOGCRITICAL);
        break;
    case DCPCommandLog::Fatal:
        levelStr = QString(DCP_DBLOGFATAL);
        break;
    default:
        return false;
    }

    QSqlQuery query(this->db);
    query.prepare("INSERT INTO " + QString(DCP_DBLOGS) +
                  " (id, level, msg) VALUES (?, ?, ?)");
    query.bindValue(0, id);
    query.bindValue(1, levelStr);
    query.bindValue(2, msg);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }
    return true;
}

bool DCPServerCentral::deleteVideoServers(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBVIDEOSERVERS) +
                  " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted video servers :"
               << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteSession(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSESSIONS) +
                  " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Session :"
               << query.lastQuery();
    return true;
}

bool DCPServerCentral::deleteStationById(qint8 id)
{
    QSqlQuery query(this->db);
    query.prepare("DELETE FROM " + QString(DCP_DBSTATIONS) + " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return false;
    }

    qWarning() << "Sucessfully deleted Station:" << query.lastQuery();
    return true;
}

DCPServerCentral::session_t*
DCPServerCentral::getDroneSessionForStation(qint8 id)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT id, station1, station2, date FROM " +
                  QString(DCP_DBSESSIONS) + " WHERE (station1=? OR station2=?)"
                  " AND (station1!=0 AND station2!=0)");
    query.bindValue(0, id);
    query.bindValue(1, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    session->id = query.value(0).toInt();
    session->station1 = query.value(1).toInt();
    session->station2 = query.value(2).toInt();
    session->date = QDateTime::fromString(query.value(3).toString());

    return session;
}

DCPServerCentral::session_t*
DCPServerCentral::getCentralSessionForStation(qint8 id)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT id, station1, station2, date FROM " +
                  QString(DCP_DBSESSIONS) + " WHERE (station1=? OR station2=?)"
                  " AND (station1=0 OR station2=0)");
    query.bindValue(0, id);
    query.bindValue(1, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    session->id = query.value(0).toInt();
    session->station1 = query.value(1).toInt();
    session->station2 = query.value(2).toInt();
    session->date = QDateTime::fromString(query.value(3).toString());

    return session;
}

DCPServerCentral::remote_t*
DCPServerCentral::getStation(qint8 id)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT type, ip, port, date, info FROM " +
                  QString(DCP_DBSTATIONS) + " WHERE id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    remote->id      = id;
    remote->type    = query.value(0).toString();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toInt();
    remote->date    = QDateTime::fromString(query.value(3).toString());
    remote->info    = query.value(4).toString();

    return remote;
}

DCPServerCentral::remote_t*
DCPServerCentral::stationIsDrone(qint8 id)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT type, ip, port, date, info FROM " +
                  QString(DCP_DBSTATIONS) + " WHERE type='drone' AND id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    remote->id      = id;
    remote->type    = query.value(0).toString();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toInt();
    remote->date    = QDateTime::fromString(query.value(3).toString());
    remote->info    = query.value(4).toString();

    return remote;
}

DCPServerCentral::remote_t*
DCPServerCentral::stationIsCommand(qint8 id)
{
    DCPServerCentral::remote_t* remote = new DCPServerCentral::remote_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT type, ip, port, date, info FROM " +
                  QString(DCP_DBSTATIONS) + " WHERE type='command' AND id=?");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    remote->id      = id;
    remote->type    = query.value(0).toString();
    remote->addr    = QHostAddress(query.value(1).toString());
    remote->port    = query.value(2).toInt();
    remote->date    = QDateTime::fromString(query.value(3).toString());
    remote->info    = query.value(4).toString();

    return remote;
}

DCPServerCentral::session_t*
DCPServerCentral::sessionIsCentral(qint8 id)
{
    DCPServerCentral::session_t* session = new DCPServerCentral::session_t;

    QSqlQuery query(this->db);
    query.prepare("SELECT station1, station2, date FROM " +
                  QString(DCP_DBSESSIONS) +
                  " WHERE id=? AND (station1=0 OR station2=0)");
    query.bindValue(0, id);
    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        return NULL;
    }
    if(!query.next())
    {
        // TODO: log ?
        return NULL;
    }

    session->id         = id;
    session->station1   = query.value(0).toInt();
    session->station2   = query.value(1).toInt();
    session->date       = QDateTime::fromString(query.value(2).toString());

    return session;
}

void DCPServerCentral::pingDrones()
{
    QHostAddress addr;
    qint16 port, sessId;

    QSqlQuery query(this->db);
    query.prepare(
    "SELECT DISTINCT tmp1.ip, tmp1.port, tmp2.id"
    "FROM (SELECT * FROM " + QString(DCP_DBSTATIONS) +
                " WHERE type!='central') AS tmp1 " +
    "LEFT OUTER JOIN (SELECT * FROM " + QString(DCP_DBSESSIONS) +
                    "WHERE station1=0 OR station2=0) AS tmp "
    "ON tmp.station1=tmp1.id OR tmp.station2=tmp1.id" );

    if(!query.exec())
    {
        qWarning() << query.lastError().driverText() << endl;
        qWarning() << query.lastError().databaseText() << endl;
        qWarning() << query.lastQuery();
        qWarning() << "Error while trying to ping drones" << endl;
        return;
    }

    while(query.next()) {
            addr = QHostAddress(query.value(0).toString());
            port = query.value(1).toInt();
            sessId = query.value(2).toInt();


            DCPCommandIsAlive *isalive =
                    new DCPCommandIsAlive(sessId, this->msecSinceStart());
            isalive->setAddrDst(addr);
            isalive->setPortDst(port);
            this->sendPacket(isalive);
    }
}
