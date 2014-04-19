/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserver.h -- bertrand
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

#ifndef DCPSERVER_H
#define DCPSERVER_H

#include <QtGlobal>
#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QMap>


class DCPPacket;
class DCPServerBackend;



class DCPServer : public QObject
{
    Q_OBJECT
public:
    explicit    DCPServer       (QUdpSocket *sock, QObject *parent = 0);
    void        removeBackend   (qint8 id);

    // TODO: make avaliable only to DCPServerBackend
    void        registerBackend (DCPServerBackend* backend, QList<qint8> ids);
    void        registerBackend(DCPServerBackend *backend, qint8 id);

public slots:
    void    sendPacket(DCPPacket *packet);

private slots:
    void    receivedDatagram();

private:
    QUdpSocket                      *sock;
    QMutex                          backendsMutex;
    QMap<qint8, DCPServerBackend*>  backends;

    DCPServerBackend*   findBackendFromSessID(qint8 sessID);

};

#endif // DCPSERVER_H
