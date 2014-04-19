/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcpserverbackendcentral.h -- bertrand
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

#ifndef DCPSERVERBACKENDCENTRAL_H
#define DCPSERVERBACKENDCENTRAL_H

#include <QtGlobal>

#include <dcpserverbackend.h>
class DCPServer;


class DCPServerBackendCentral : public DCPServerBackend
{
public:
    DCPServerBackendCentral();

    inline qint8 getSessID()    { return this->sessID;  }
    inline qint8 getMyId()      { return this->myID;    }

    void    registerWithServer(DCPServer* srv);

private:
    qint8   sessID;
    qint8   myID;
};

#endif // DCPSERVERBACKENDCENTRAL_H
