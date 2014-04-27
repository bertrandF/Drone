/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- libdcp_global.h -- bertrand
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

#ifndef LIBDCP_GLOBAL_H
#define LIBDCP_GLOBAL_H

#include <QtCore/qglobal.h>

#include <dcp.h>
#include <dcppacket.h>
#include <dcpcommands.h>
#include <dcppackethandlerinterface.h>
#include <dcpserver.h>
#include <dcpserverbackendcentral.h>
#include <dcpserverbackendremote.h>

#if defined(LIBDCP_LIBRARY)
#  define LIBDCPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBDCPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBDCP_GLOBAL_H
