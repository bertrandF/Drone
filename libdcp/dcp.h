/*
 *  This file is part of the libDCP Project
 *  Copyright (C) 15/04/2014 -- dcp.h -- bertrand
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

#ifndef DCP_H
#define DCP_H


/* --- DCP Header length --- */
#define DCP_HEADERSIZE      (4)

/* --- Commands --- */
#define DCP_CMDACK                  ((char)0x00)
#define DCP_CMDISALIVE              ((char)0x01)
#define DCP_CMDAILERON              ((char)0x02)
#define DCP_CMDTHROTTLE             ((char)0x03)
#define DCP_CMDSETSESSID            ((char)0x04)
#define DCP_CMDLOG                  ((char)0x05)
#define DCP_CMDHELLOFROMCENTRAL     ((char)0x06)
#define DCP_CMDHELLOFROMREMOTE      ((char)0x07)
#define DCP_CMDBYE                  ((char)0x08)
#define DCP_CMDCONNECTTODRONE       ((char)0x09)
#define DCP_CMDDISCONNECT           ((char)0x0A)
#define DCP_CMDVIDEOSERVERS         ((char)0x0B)


/* --- VIDEO SERVERS --- */
#define DCP_VIDEOSERVERSSEPARATOR   ((char)'$')

/* --- DEFAULTS IDS --- */
#define DCP_IDNULL                  ((char)0x00)
#define DCP_IDCENTRAL               ((char)0x00)
#define DCP_SESSIDCENTRAL           ((char)0x00)

/* --- Motors Defaults IDs --- */
/* Those Motors IDs are reserved */
#define DCP_MOTORSALL               ((char)0x00)
#define DCP_MOTORSLEFT              ((char)0x01)
#define DCP_MOTORSRIGHT             ((char)0x02)

/* --- DataBase query --- */
#define DCP_DBQUERYERROR                ((char)-1)
#define DCP_DBNOAVALIABLEIDS            ((char)-2)

#define DCP_DBSTATIONS                  "stations"
#define DCP_DBVIDEOSERVERS              "videos"
#define DCP_DBSESSIONS                  "sessions"
#define DCP_DBLOGS                      "logs"

#define DCP_DBLOGINFO                   "info"
#define DCP_DBLOGWARNING                "warning"
#define DCP_DBLOGCRITICAL               "critical"
#define DCP_DBLOGFATAL                  "fatal"



#endif // DCP_H
