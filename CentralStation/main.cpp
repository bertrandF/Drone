/*
 *  This file is part of the CentralStation Project
 *  Copyright (C) 15/04/2014 -- main.cpp -- bertrand
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

#include <QCoreApplication>
#include <QHostAddress>
#include <QUdpSocket>
#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <dcp.h>
#include <dcpservercentral.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString strAddr = a.arguments().at(1);
    QString strPort = a.arguments().at(2);

    QUdpSocket *sock = new QUdpSocket();
    sock->bind(QHostAddress(strAddr), strPort.toInt());

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("drones");
    db.setUserName("dronedbmanager");
    db.setPassword("Ch3v41");

    if(db.open())
    {
        DCPServerCentral *central = new DCPServerCentral(sock, db);

        QSqlQuery query(db);
        query.prepare("INSERT INTO " DCP_DBSTATIONS " (type, ip, port, info)"
                      " VALUES (?, ?, ?, ?)");
        query.bindValue(0, "central");
        query.bindValue(1, strAddr);
        query.bindValue(2, strPort.toInt());
        query.bindValue(3, "Central station in charge of this NET");
        if(!query.exec())
        {
            qWarning() << query.lastError().driverText() << endl;
            qWarning() << query.lastError().databaseText() << endl;
            qWarning() << query.lastQuery();
            qFatal("Central station: BYE cruel world !");
        }
    }

    return a.exec();
}
