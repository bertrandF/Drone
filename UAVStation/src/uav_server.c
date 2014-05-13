/*!
 *   \file  uav_server.c
 *   \brief  UAV server: receive commands from the command station
 *   and send status report to central station.
 *  
 *  Takes care of all the DCP protocol steps for the UAV.
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


/* Linux socket includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

/* Other includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Local includes */
#include "uav_server.h"
#include "dcp.h"


/*!
 *  \brief  UAV server state enumeration.
 *  
 */
enum uavsrv_state_e {
    NONE,       ///< No servers, stopped or not yet created.
    CREATED,    ///< Server created, after call to uavsrv_create().
    SOCKREADY,  ///< Socket created and bound to interface.
};

/*!
 *  \brief  UAV server description structure.
 *  
 *  Keeps track of the configuration and state of the UAV server.
 *
 */
struct uavsrv_s {
    enum uavsrv_state_e     state;  ///< UAV Server state.
    struct uavsrv_params_s  params; ///< User given configuration structure.
    int                     sock;   ///< Server socket.
};



/*!
 *  \brief  Static structure UAV server, internal use only.
 *  
 *  Static structure for internal use in uav_server.c.
 */
static struct uavsrv_s  uavsrv = { NONE };


/*!
 *  \brief  Last error code.
 *  
 *  Last error code. You can call uavsrv_errstr() to get
 *  the description of the error.
 */
int uavsrv_err=UAVSRV_ERR_NOERR;


/*!
 *  \brief  Table of error strings. Internal use only.
 *
 *  Table containing the error strings. Used by uavsrv_errstr() to
 *  get the error string associated with a error code. The string
 *  for the error code X is at errstrs[X].
 *  
 */
const char* errstrs [] = {
    "UAV SRV: no errors",
    "UAV SRV: srv running",
    "UAV SRV: cannot alloc mem",
    "UAV SRV: requires state CREATED",
    "UAV SRV: params=NULL",
    "UAV SRV: cannot create socket",
    "UAV SRV: cannot bind socket",
    "UAV SRV: requires state SOCKREADY"
};


/*!
 *  \brief  Returns a string decribing the last error message.
 *  
 *  Returns the string describing the last error which number is
 *  uavsrv_err. The returns value points to a staticaly allocated
 *  buffer.
 *
 *  \return Pointer to the string describing the last error.
 */
const char* uavsrv_errstr() 
{
    return errstrs[uavsrv_err];
}


/*!
 *  \brief  Creates an UAV server and returns a pointer to the
 *          parameters structure.
 *
 *  This function allocates space for the uavsrv_s structure.
 *  The uavsrv_destroy() must be called if this function was 
 *  previously called. This fore re-initialization of all
 *  parameters.
 *  
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int uavsrv_create() 
{
    /* Check the server is not already running. */
    if( uavsrv.state > NONE ) 
    {
        uavsrv_err = UAVSRV_ERR_RUNNING;
        return -1;
    }
    uavsrv.state = CREATED;
    return 0;
}

/*!
 *  \brief  Go thought the UAV init sequence.
 *
 *  Says Hello to central station and wait for response.
 *  
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 *
 */
int uavsrv_start()
{
    /* Check required state */
    if( uavsrv.state != SOCKREADY ) {
        uavsrv_err = UAVSRV_ERR_REQREADY;
        return -1;
    }
    
    /* Say hello */


    return 0;
}



/*!
 *  \brief  Server main loop.
 *
 *  Process the init sequence ( reports itself to the central
 *  station). If the init sequence failed, it returns; otherwise
 *  it starts the reporting thread and call select, waiting for 
 *  a command station to send commands.
 *  
 *  \param  params  Pointer to the UAV server's parameters structure.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 *
 */
int uavsrv_run(struct uavsrv_params_s *params) 
{
    /* Check configuration */
    if( uavsrv.state != CREATED ) 
    {
        uavsrv_err = UAVSRV_ERR_REQCREATED;
        return -1;
    }
    if( params == NULL )
    {
        uavsrv_err = UAVSRV_ERR_NULLPARAMS;
        return -1;
    }
    else 
    {
        memcpy(&(uavsrv.params), params, sizeof(struct uavsrv_params_s));
    }
        

    /* Create socket+bind */
    if((uavsrv.sock=socket(uavsrv.params.if_addr.sa_family, SOCK_DGRAM, 0)) < 0)
    {
        uavsrv_err = UAVSRV_ERR_SOCKET;
        return -1;
    }
    if(bind(uavsrv.sock, &(uavsrv.params.if_addr), sizeof(struct sockaddr)) < 0) 
    {
        uavsrv_err = UAVSRV_ERR_BIND;
        return -1;
    }
    uavsrv.sock = SOCKREADY;

    /* Say hello to central station */
    if( uavsrv_start() < 0 )
        return -1;


    close(uavsrv.sock);
    return 0;
}


/*!
 *  \brief  Destroy the UAV server.
 * 
 *  Clean everything up.
 *
 *  \return Void.
 * 
 */
void uavsrv_destroy() 
{
    close(uavsrv.sock);
    uavsrv.state = NONE;
}
