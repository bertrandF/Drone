/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserver.cpp -- bertrand
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

#include "dcpserver.h"
#include "dcpcommands.h"

DCPServer::DCPServer(QUdpSocket *sock) :
    QObject(),
    sock(sock),
    myID(0)
{
    this->handler = new DCPPacketHandlerCommandStationHello(this);
}

void DCPServer::sendPacket(DCPPacket *packet)
{
    QByteArray *datagram = packet->packetToData();
    int err = this->sock->writeDatagram(datagram->data(), packet->getLenght(),
                                packet->getAddrDst(), packet->getPortDst());

    if(err >= 0)
    {
        qDebug() << "Send success for:";
        qDebug() << packet->toString();
        this->moveToAckQueue(packet);
    }
    else
    {
        qDebug() << "Send failure for:";
        qDebug() << packet->toString();
    }

    return;
}

void DCPServer::setMyId(qint8 myID)
{
    this->myID = myID;
}

void DCPServer::moveToAckQueue(DCPPacket *packet)
{
    this->ackMutex.lock();
    this->ackQueue.append(packet);
    this->ackMutex.unlock();
}

void DCPServer::removeFromAckQueue(DCPPacket *packet)
{
    this->ackMutex.lock();
    this->ackQueue.removeOne(packet);
    this->ackMutex.unlock();
}

DCPPacket* DCPServer::findInAckQueue(qint32 timestamp)
{
    DCPPacket* packet=NULL;
    this->ackMutex.lock();
    foreach (packet, this->ackQueue) {
        if(packet->getTimestamp() == timestamp)
        {
            this->ackMutex.unlock();
            return packet;
        }
    }

    this->ackMutex.unlock();
    return NULL;
}

void DCPServer::setHandler(DCPPacketHandlerInterface *handler)
{
    this->handler = handler;
}
