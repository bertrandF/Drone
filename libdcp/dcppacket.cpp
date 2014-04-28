/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcppacket.cpp -- bertrand
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

#include "dcppacket.h"

DCPPacket::DCPPacket(qint8 cmdID, qint8 sessID, qint32 timestamp) :
    cmdID(cmdID),
    sessID(sessID),
    timestamp(timestamp)
{
}

void DCPPacket::buildFromData(char *data, int len)
{
    this->cmdID = (data[0]>>4) & (qint8)0x0F;
    this->sessID = (data[0] & (qint8)0x0F);
    this->timestamp = ((qint32)((qint32)data[1]<<16) & (qint32)0x00FF0000) |
            ((qint32)((qint32)data[1]<<8) & (qint32)0x0000FF00) |
            ((qint32)((qint32)data[1]) & (qint32)0x000000FF);
    this->payload = QByteArray::fromRawData(
                data+DCP_HEADERSIZE, len-DCP_HEADERSIZE);
    this->unbuildPayload();
}

QByteArray DCPPacket::buildHeader()
{
    char *h = new char[DCP_HEADERSIZE];
    this->header.clear();

    h[0] = (this->cmdID & 0x0F)<<4 | (this->sessID & 0x0F);
    h[1] = (quint8)((this->timestamp>>16) & (quint32)0x000000FF);
    h[2] = (quint8)((this->timestamp>>8) & (quint32)0x000000FF);
    h[3] = (quint8)(this->timestamp & (quint32)0x000000FF);
    this->header = QByteArray::fromRawData(h, DCP_HEADERSIZE);

    return this->header;
}

QByteArray DCPPacket::buildPayload()
{
    this->payload.clear();
    return this->payload;
}

QByteArray* DCPPacket::packetToData()
{
    QByteArray *data = new QByteArray();

    this->buildHeader();
    data->append(this->header.data(), DCP_HEADERSIZE);
    this->buildPayload();
    data->append(this->payload.data(), this->getPayloadLength());

    return data;
}

void DCPPacket::unbuildPayload()
{
    return;
}


void DCPPacket::handle(DCPPacketHandlerInterface *handler)
{
    handler->handleNull(this);
}

bool DCPPacket::setTimestamp(qint32 timestamp)
{
    if(timestamp > (qint32)0x00FFFFFF)
        return false;
    this->timestamp = timestamp;
    return true;
}

int DCPPacket::getLenght()
{
    return DCP_HEADERSIZE + this->getPayloadLength();
}

int DCPPacket::getPayloadLength()
{
    return this->payload.length();
}

QString DCPPacket::toString()
{
    QString str;
    QTextStream text(&str);
    text << "Addr Dst: " << this->addrDst.toString() << endl;
    text << "Port Dst: " << this->portDst << endl;
    text << "Cmd Id: " << this->cmdID << endl;
    text << "Sess Id: " << this->sessID << endl;
    text << "Timestamp: " << this->timestamp << endl;

    return str;
}
