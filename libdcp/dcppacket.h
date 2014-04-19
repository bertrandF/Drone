/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcppacket.h -- bertrand
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

#ifndef DCPPACKETINTERFACE_H
#define DCPPACKETINTERFACE_H

#include <QtGlobal>
#include <QByteArray>
#include <QHostAddress>

#include <dcp.h>
#include <dcppackethandlerinterface.h>




class DCPPacket
{
    friend class DCPPacketFactory;

public:
    DCPPacket(qint8 cmdID=DCP_CMDACK, qint8 sessID=DCP_SESSIDCENTRAL,
              qint32 timestamp=0);
    DCPPacket(char* data, int len);

    inline qint8        getCommandID()  { return this->cmdID;       }
    inline qint8        getSessionID()  { return this->sessID;      }
    qint32              getTimestamp()  { return this->timestamp;   }
    inline QHostAddress getAddrDst()    { return this->addrDst;     }
    inline quint16      getPortDst()    { return this->portDst;     }
    int                 getLenght();

    bool            setTimestamp(qint32 timestamp);
    inline void     setAddrDst(QHostAddress addr)   { this->addrDst = addr; }
    inline void     setPortDst(quint16 port)        { this->portDst = port; }


    virtual void    handle(DCPPacketHandlerInterface *handler);
    DCPPacket*      dataToPacket(char* data, int len);
    QByteArray*     packetToData();

protected:
    QByteArray  payload;

    virtual QByteArray  buildPayload();
    virtual void        unbuildPayload();
    virtual int         getPayloadLength();

private:
    qint8       cmdID;
    qint8       sessID;
    qint32      timestamp;

    QHostAddress    addrDst;
    quint16         portDst;

    QByteArray  header;
    QByteArray  buildHeader();
};

#endif // DCPPACKETINTERFACE_H
