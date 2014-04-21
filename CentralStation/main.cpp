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

#include <dcp.h>
#include <dcpserver.h>
#include <dcpserverbackendcentral.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QUdpSocket *sock = new QUdpSocket();
    sock->bind(QHostAddress("192.168.0.29"), 5866);
    DCPServer * srv = new DCPServer(sock);
    DCPServerBackendCentral *back = new DCPServerBackendCentral();
    back->registerWithServer(srv);

    return a.exec();
}
