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

#include <dcp.h>
#include <dcpservercentral.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QUdpSocket *sock = new QUdpSocket();
    sock->bind(QHostAddress("192.168.0.24"), 5866);
    // TODO: create server

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("drones");
    db.setUserName("dronedbmanager");
    db.setPassword("Ch3v41");

    if(db.open())
    {
//        qDebug() << back->nextCommandStationId();
//        qDebug() << back->nextDroneId();
//        qDebug() << back->nextSessId();
    }

    return a.exec();
}
