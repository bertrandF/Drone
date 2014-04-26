/*
 *  This file is part of the CommandStation Project
 *  Copyright (C) 15/04/2014 -- commandstationparameters.h -- bertrand
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

#ifndef COMMANDSTATIONPARAMETERS_H
#define COMMANDSTATIONPARAMETERS_H

#include <QUrl>
#include <QHostAddress>

class CommandStationParameters
{
public:
    CommandStationParameters() {};

    QUrl            frontVideoFeed;

    QHostAddress    dcpServerHost;
    quint16         dcpServerPort;

    QHostAddress    dbServerHost;
    quint16         dbServerPort;
    QString         dbName;
    QString         dbUserName;
    QString         dbUserPassword;
    qint8           droneId;

    QHostAddress    centralStationHost;
    quint16         centralStationPort;
};

#endif // COMMANDSTATIONPARAMETERS_H
