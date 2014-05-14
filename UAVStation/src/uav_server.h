/*!
 *   \file  uav_server.h
 *   \brief  uav_server.c include file.
 *  
 *  uav_server.c function exports.
 *  
 *  \author  Bertrand.F (), 
 *  
 *  \internal
 *       Created:  12/05/2014
 *      Revision:  none
 *      Compiler:  gcc
 *  Organization:  
 *     Copyright:  Copyright (C), 2014, Bertrand.F
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
 */

#ifndef __UAV_SERVER_H__
#define __UAV_SERVER_H__

#include <sys/socket.h>

#define UAVSRV_ERR_NOERR        (0)     ///< No errors.
#define UAVSRV_ERR_RUNNING      (1)     ///< Server is running.
#define UAVSRV_ERR_MALLOC       (2)     ///< Error in malloc() function call.
#define UAVSRV_ERR_REQCREATED   (3)     ///< State CREATED required.
#define UAVSRV_ERR_NULLPARAMS   (4)     ///< Supplied pointer to struct uavsrv_params_s is null.
#define UAVSRV_ERR_SOCKET       (5)     ///< Error in socket() function call.
#define UAVSRV_ERR_BIND         (6)     ///< Error in bind() function call.
#define UAVSRV_ERR_REQREADY     (7)     ///< State SOCKREADY required.
#define UAVSRV_ERR_TIMEDOUT     (8)     ///< Waiting has timed out.
#define UAVSRV_ERR_SELECT       (9)     ///< Error in select() call.


/*!
 *  \brief  UAV server's user defined parameters.
 *
 *  Structure for the user to set paramters for the server.
 *  This structure must be given to uavsrv_run().
 *  
 */
struct uavsrv_params_s {
    struct sockaddr if_addr;        ///< Socket bind interface addr.
    struct sockaddr central_addr;   ///< Central Station host information.
    unsigned short  central_port;   ///< Central Station port.
    struct timeval  timeout;        ///< Select() timeout.
};


extern int          uavsrv_err;
extern const char*  uavsrv_errstr   ();

extern int          uavsrv_create   ();
extern int          uavsrv_run      (struct uavsrv_params_s *params);
extern void         uavsrv_destroy  ();

#endif
