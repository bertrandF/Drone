/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserverbackend.h -- bertrand
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

#ifndef DCPSERVERBACKEND_H
#define DCPSERVERBACKEND_H

#include <QtGlobal>
#include <QTime>
#include <QLinkedList>
#include <QMutex>

class DCPPacket;
class DCPPacketHandlerInterface;



class DCPServerBackend : public QObject
{
    Q_OBJECT

public:
    DCPServerBackend();

    inline DCPPacketHandlerInterface*  getHandler() { return this->handler; }

    void            moveToAckQueue(DCPPacket* packet);
    void            removeFromAckQueue(DCPPacket* packet);
    DCPPacket*      findInAckQueue(qint32 timestamp);

    // TODO: Make avaliable only to friends
    void            setMyId(qint8 myID);
    void            setHandler(DCPPacketHandlerInterface *handler);

public slots:
    void sendPacket(DCPPacket* packet);

signals:
    void send(DCPPacket* packet);

protected:
    DCPPacketHandlerInterface   *handler;

    QTime           time;
    qint8           myID;

    QMutex                  ackMutex;
    QLinkedList<DCPPacket*> ackQueue;
};



#endif // DCPSERVERBACKEND_H
