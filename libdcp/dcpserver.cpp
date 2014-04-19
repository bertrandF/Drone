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

#include "dcpserver.h"
#include "dcpcommands.h"
#include "dcpserverbackend.h"

DCPServer::DCPServer(QUdpSocket *sock, QObject *parent) :
    QObject(parent),
    sock(sock),
    backends()
{
    connect(sock, SIGNAL(readyRead()), this, SLOT(receivedDatagram()));
}


void DCPServer::sendPacket(DCPPacket* packet)
{
    int err;
    QByteArray *datagram = packet->packetToData();
    DCPServerBackend *backend = (DCPServerBackend*)QObject::sender();
    if(backend == NULL)
        return;

    err = this->sock->writeDatagram(datagram->data(), packet->getLenght(),
                                packet->getAddrDst(), packet->getPortDst());

    if(err >= 0)
        backend->moveToAckQueue(packet);
    qDebug() << "p leng:" << packet->getLenght();
    qDebug() << this->sock->errorString() << " -- " << this->sock->error();

    return;
}


void DCPServer::receivedDatagram()
{
    QHostAddress addr;
    quint16 port;
    qint64 dataSize = this->sock->pendingDatagramSize();
    if(dataSize <= 0)
        return;

    char* data = new char[dataSize];
    this->sock->readDatagram(data, dataSize, &addr, &port);

    DCPPacket* packet = DCPPacketFactory::commandPacketFromData(data, dataSize);
    packet->setAddrDst(addr);
    packet->setPortDst(port);
    DCPServerBackend *backend =
            this->findBackendFromSessID(packet->getSessionID());
    if(backend != NULL)
        packet->handle(backend->getHandler());
}

void DCPServer::registerBackend(DCPServerBackend *backend, QList<qint8> ids)
{
    qint8 id;

    if(!this->backends.values().contains(backend))
    {
        connect(backend, SIGNAL(send(DCPPacket*)), this,
                        SLOT(sendPacket(DCPPacket*)));
    }

    foreach(id, ids)
    {
        this->backendsMutex.lock();
        this->backends.insert(id, backend);
        this->backendsMutex.unlock();
    }
}

void DCPServer::registerBackend(DCPServerBackend *backend, qint8 id)
{
    if(!this->backends.values().contains(backend))
    {
        connect(backend, SIGNAL(send(DCPPacket*)), this,
                        SLOT(sendPacket(DCPPacket*)));
    }
    this->backends.insert(id, backend);
}

void DCPServer::removeBackend(qint8 id)
{
    this->backendsMutex.lock();
    this->backends.remove(id);
    this->backendsMutex.unlock();
}

DCPServerBackend* DCPServer::findBackendFromSessID(qint8 sessID)
{
    return this->backends.find(sessID).value();

}

