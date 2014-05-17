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
#include <QTime>
#include <QLinkedList>
#include <QMutex>
#include <QUdpSocket>

class DCPPacket;
class DCPPacketHandlerInterface;



class DCPServer : public QObject
{
    Q_OBJECT

public:
    DCPServer(QUdpSocket *sock);

    inline DCPPacketHandlerInterface*  getHandler() { return this->handler; }

    void            moveToAckQueue(DCPPacket* packet);
    void            removeFromAckQueue(DCPPacket* packet);
    DCPPacket*      findInAckQueue(qint32 timestamp);

    // TODO: Make avaliable only to friends
    void            setMyId(qint8 myID);
    inline qint8    getMyId()
        { return this->myID; }
    void            setHandler(DCPPacketHandlerInterface *handler);
    inline int      msecSinceStart()
        { return this->time.elapsed(); }

public slots:
    void sendPacket(DCPPacket* packet);
    void receiveDatagram();

protected:
    QUdpSocket      *sock;
    DCPPacketHandlerInterface   *handler;

    QTime           time;
    qint8           myID;

    QMutex                  ackMutex;
    QLinkedList<DCPPacket*> ackQueue;
};



#endif // DCPSERVER_H
