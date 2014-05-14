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
#include <sys/time.h>
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

/* Defines */
#define UAVDESC     "UAV-XFL02648"
#define UAVDESCLEN  (12)



/*!
 *  \brief  DCP packet structure.
 *
 *  Used to store DCP packet information on header and payload.
 *  
 */
struct dcp_packet_s {
    struct sockaddr dstaddr;    ///< Host from which the packet has been received.
    int             dstaddrlen; ///< Length of the dstaddr field.
    uint8_t         cmd;        ///< DCP command ID.
    uint8_t         sessid;     ///< DCP packet session ID.
    uint32_t        timestamp;  ///< DCP packet timestmap.
    char            data[16];   ///< DCP packet payload.
    int             datalen;    ///< DCP packet payload length.
};



//-----------------------------------------------------------------------------
//  CALLBACKS
//-----------------------------------------------------------------------------

/*!
 *  \brief  DCP packet handler callback functions type.
 *  
 *  This type of function will handle DCP packets.
 *
 */
typedef int(*dcp_handler_f)(struct dcp_packet_s* packet);



/*!
 *  \brief Null DCP packet handler. 
 *  
 *  This function does nothing.
 *
 *  \param  packet  The DCP packet structure.
 *  \return Always return 0 (Sucess).
 */
int handler_null(struct dcp_packet_s* packet) 
{
    return 0;
}




//-----------------------------------------------------------------------------
//  SEND DCP PACKETS
//-----------------------------------------------------------------------------

/*!
 *  \brief  Destroy a previously allocated packet structure.
 *  
 *  Free the memory allocated for a packet; for exmample after a call to 
 *  uavsrv_dcp_waitone().
 *
 *  \param  packet  pointer to the packet to destroy.
 *  \return Void.
 */
void dcp_packetfree(struct dcp_packet_s* packet)
{
     free(packet);   
}

/*!
 *  \brief  Send Hello to central server.
 *  
 *  Sen hello to the central station. This function builds the DCP packet to send
 *  and call the sendto() function.
 *
 *  \param  dst Central station sockaddr structure.
 *  \param  str UAV description message to register to DB.
 *  \param  len Length of the desciption message in str.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int dcp_hello(struct sockaddr* dst, char *str, int len) 
{
    struct dcp_packet_s packet;
    // TODO: send packet
    
    return 0;
}




/*!
 *  \brief  Send Ack for given packet.
 *  
 *  Send and Ack for the given packet. The packet already contains the struct sockaddr 
 *  of the sender. For performances purposes, it reusses the same struct to build the Ack.
 *
 *  \param  packet  The packet to send an Ack for.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int dcp_packetack(struct dcp_packet_s* packet)
{
    return 0;
}

//-----------------------------------------------------------------------------
//  UAV STRUCTURES + FUNCTIONS
//-----------------------------------------------------------------------------

/*!
 *  \brief  UAV server state enumeration.
 *  
 */
enum uavsrv_state_e {
    NONE,       ///< No servers, stopped or not yet created.
    CREATED,    ///< Server created, after call to uavsrv_create().
    SOCKREADY   ///< Socket created and bound to interface.
};

/*!
 *  \brief  UAV server description structure.
 *  
 *  Keeps track of the configuration and state of the UAV server.
 *
 */
struct uavsrv_s {
    enum uavsrv_state_e     state;          ///< UAV Server state.
    struct uavsrv_params_s  params;         ///< User given configuration structure.
    int                     sock;           ///< Server socket.
    dcp_handler_f           handlers[16];   ///< DCP packets handlers.
};



/*!
 *  \brief  Static structure UAV server, internal use only.
 *  
 *  Static structure for internal use in uav_server.c.
 */
static struct uavsrv_s  uavsrv = { 
    NONE, 
    {},
    -1,
    {}
};


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
    "UAV SRV: requires state SOCKREADY",
    "UAV SRV: timed out",
    "UAV SRV: error in select()"
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
 *  \brief  Wait to receive one DCP packet.
 *
 *  Wait to receive one DCP packet an return a pointer to the corresponding struct
 *  dcp_packet_s. The timeout value is specified by the struct timeval pointer
 *  in the uavsrv structure.
 *  
 *  \return A pointer to the new DCP packet or NULL if error. If select() has
 *          been timed out, uavsrv_err is set to UAVSRV_ERR_TIMED_OUT.
 */
struct dcp_packet_s* uavsrv_dcp_waitone()
{
    fd_set readset;
    struct dcp_packet_s *packet = NULL;
    int fdnb, bufflen=64, bread;
    char buff[64];

    FD_ZERO(&readset);
    FD_SET(uavsrv.sock, &readset);

    fdnb = select(uavsrv.sock+1, &readset, NULL, NULL, &(uavsrv.params.timeout));
    switch(fdnb) {
        case 0:
            uavsrv_err = UAVSRV_ERR_TIMEDOUT;
            break;
        case -1:
            uavsrv_err = UAVSRV_ERR_SELECT;
            break;
        default:
            packet = malloc(sizeof(struct dcp_packet_s));
            if(!packet) {
                uavsrv_err = UAVSRV_ERR_MALLOC;
                break;
            }
            bread = recvfrom(uavsrv.sock, buff, bufflen, 0, &(packet->dstaddr), &(packet->dstaddrlen));
            packet->cmd     = (buff[0]>>4) & (char)0x0F;
            packet->sessid  = buff[0] & (char)0x0F;
            packet->timestamp = (uint32_t)((uint32_t)(buff[1]<<16) & (uint32_t)0xFF0000) |
                                (uint32_t)((uint32_t)(buff[2]<< 8) & (uint32_t)0x00FF00) |
                                (uint32_t)((uint32_t)(buff[3]    ) & (uint32_t)0x0000FF) ;
            packet->datalen = bread-4;
            memcpy(&(packet->data), buff+4, bread-4);
            break;
    }

    return packet;
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
 */
int uavsrv_start()
{
    struct dcp_packet_s *packet;

    /* Check required state */
    if( uavsrv.state != SOCKREADY ) {
        uavsrv_err = UAVSRV_ERR_REQREADY;
        return -1;
    }
    
    dcp_hello(&(uavsrv.params.central_addr), UAVDESC, UAVDESCLEN);
    do {
        packet = uavsrv_dcp_waitone();
    } while(!packet || packet->cmd!=DCP_CMDHELLOFROMCENTRAL);
    dcp_packetack(packet);

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
 */
void uavsrv_destroy() 
{
    close(uavsrv.sock);
    uavsrv.state = NONE;
}
