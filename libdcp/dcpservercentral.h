/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpservercentral.h -- bertrand
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

#ifndef DCPSERVERCENTRAL_H
#define DCPSERVERCENTRAL_H

#include <QtGlobal>
#include <QtSql/QSqlDatabase>
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include <QDateTime>

#include <dcp.h>
#include <dcpserver.h>
#include <dcpcommands.h>


class DCPServerCentral : public DCPServer
{
public:
    DCPServerCentral(QUdpSocket *socket, QSqlDatabase db);

    typedef struct remote_s {
        qint8 id;
        QHostAddress addr;
        quint16 port;
        QDateTime date;
        QString info;
    } remote_t;
    typedef struct session_s {
        qint8 id;
        qint8 iddrone;
        qint8 idcommand;
        QDateTime date;
    } session_t;
    typedef struct session_central_s {
        qint8 id;
        qint8 idremote;
        QDateTime date;
    } session_central_t;
    // TODO: make avaliable only to packet handler
    remote_t*   addNewDrone(QHostAddress addr, quint16 port, QString info);
    remote_t*   addNewCommandStation(QHostAddress addr, quint16 port, QString info);
    session_t*  addNewSession(qint8 iddrone, qint8 idcommand);
    session_central_t*   addNewSessionCentralDrone(qint8 iddrone);
    session_central_t*   addNewSessionCentralCommand(qint8 idcommand);
    bool        deleteSessionForCommandId(qint8 id);
    bool        deleteCommandById(qint8 id);
    remote_t*   getCommandFromCentralSessionId(qint8 id);

private:
    QSqlDatabase db;
};

#endif // DCPSERVERCENTRAL_H
