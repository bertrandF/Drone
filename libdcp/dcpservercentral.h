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

#include <dcp.h>
#include <dcpserver.h>
#include <dcpcommands.h>


class DCPServerCentral : public DCPServer
{
public:
    DCPServerCentral(qint8 sessID=DCP_SESSIDCENTRAL);

    inline qint8 getSessID()    { return this->sessID;  }
    inline qint8 getMyId()      { return this->myID;    }
    inline void setDroneSessId(qint8 id)
        { this->droneSessId = id; }
    inline qint8 getDroneSessId()
        { return this->droneSessId; }
    inline void setDb(QSqlDatabase db)
        { this->db = db; }
    inline QSqlDatabase getDb()
        { return this->db; }

    void registerNewBackendWithServer(DCPServerCentral *central);

    // TODO: make avaliable only to packet handler
    qint8   nextDroneId();
    qint8   nextCommandStationId();
    qint8   nextSessId();
    bool    addNewRemote(DCPCommandHelloFromRemote::remoteType type, qint8 id,
                         QHostAddress addr, quint8 port, QString description);

private:
    qint8       sessID;
    qint8       myID;
    qint8       droneSessId;

    QSqlDatabase db;
};

#endif // DCPSERVERCENTRAL_H
