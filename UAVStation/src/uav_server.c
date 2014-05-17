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
#include <time.h>
#include <syslog.h>

/* Local includes */
#include "uav_server.h"
#include "dcp.h"

/* Defines */
#define PDATAMAX    (256)   ///< Max packet data len



//-----------------------------------------------------------------------------
//  DCP DATATYPES
//-----------------------------------------------------------------------------
/*!
 *  \brief  DCP packet structure.
 *
 *  Used to store DCP packet information on header and payload.
 *  
 */
struct dcp_packet_s {
    struct sockaddr_storage dstaddr;        ///< Host from which the packet has been received.
    socklen_t               dstaddrlen;     ///< Length of the dstaddr field.
    uint8_t                 cmd;            ///< DCP command ID.
    uint8_t                 sessid;         ///< DCP packet session ID.
    uint32_t                timestamp;      ///< DCP packet timestmap.
    char                    data[PDATAMAX]; ///< DCP packet payload.
    int                     datalen;        ///< DCP packet payload length.
    struct dcp_packet_s*    next;           ///< For ackqueue, next packet in the queue.
};



/*!
 *  \brief  DCP packet handler callback functions type.
 *  
 *  This type of function will handle DCP packets.
 *
 */
typedef int(*dcp_handler_f)(struct dcp_packet_s* packet);


//-----------------------------------------------------------------------------
//  UAV DATATYPES
//-----------------------------------------------------------------------------

/*!
 *  \brief  UAV server state enumeration.
 *  
 */
enum uavsrv_state_e {
    NONE,       ///< No servers, stopped or not yet created.
    CREATED,    ///< Server created, after call to uavsrv_create().
    SOCKREADY,  ///< Socket created and bound to interface.
    REGISTERED  ///< Said hello to central station and Hello accepted.
};

/*!
 *  \brief  UAV server description structure.
 *  
 *  Keeps track of the configuration and state of the UAV server.
 *
 */
struct uavsrv_s {
    enum uavsrv_state_e     state;          ///< UAV Server state.
    time_t                  start_time;     ///< UAV start time (used for timestamps).
    struct uavsrv_params_s  params;         ///< User given configuration structure.
    int                     sock;           ///< Server socket.
    dcp_handler_f           handlers[16];   ///< DCP packets handlers.
    struct dcp_packet_s*    ackqueue;       ///< List of sent packet, waiting for ack.
    struct dcp_packet_s**   ackqueue_tail;  ///< Pointer to next packet pointer; used to insert new packet at the tail of the queue.
    int                     myid;           ///< UAV ID.
    int                     central_sessid; ///< SessID to speak with central station.
};



/*!
 *  \brief  Static structure UAV server, internal use only.
 *  
 *  Static structure for internal use in uav_server.c.
 */
static struct uavsrv_s  uavsrv = { 
    NONE, 
    0,
    {},
    -1,
    {},
    NULL,
    NULL,
    DCP_IDNULL,
    DCP_IDNULL
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
    "UAV SRV: error in select()",
    "UAV SRV: no such packet in ackqueue"
};



//-----------------------------------------------------------------------------
//  FUNCTIONS DECLARATION
//-----------------------------------------------------------------------------
int                     ackqueue_add                (struct dcp_packet_s*);
int                     ackqueue_delete             (struct dcp_packet_s*);
struct dcp_packet_s*    ackqueue_findbytimestamp    (uint32_t);

int handler_null                (struct dcp_packet_s*);
int handler_ack                 (struct dcp_packet_s*);
int handler_hellofromcentral    (struct dcp_packet_s*);

struct dcp_packet_s*    dcp_packetnew   ();
void                    dcp_packetfree  (struct dcp_packet_s*);
int                     dcp_send        (struct dcp_packet_s*); 
int                     dcp_hello       (struct sockaddr_storage*, char*, int);
int                     dcp_videos      (char*);
int                     dcp_packetack   (struct dcp_packet_s*);

const char*             uavsrv_errstr();
struct dcp_packet_s*    uavsrv_dcp_waitone();
int                     uavsrv_create();
int                     uavsrv_start();
int                     uavsrv_run(struct uavsrv_params_s*);
void                    uavsrv_destroy();


//-----------------------------------------------------------------------------
//  UAV ACK QUEUE HANDLING FUNCTIONS
//-----------------------------------------------------------------------------
/*!
 *  \brief  Add new packet to ackqueue.
 *  
 *  \param  packet  Packet to add to ackqueue.
 *  \return -1 is returned in case of failure. On Success 0 is
 *          returned.
 */
int ackqueue_add(struct dcp_packet_s* packet)
{
    packet->next=NULL;
    *(uavsrv.ackqueue_tail) = packet; 
    uavsrv.ackqueue_tail = &(packet->next);
    return 0;
}



/*!
 *  \brief  Delete packet from ack queue.
 *  
 *  \param  packet  Packet to delete from ackqueue.
 *  \return -1 is returned in case of failure. On Success 0 is
 *          returned.
 */
int ackqueue_delete(struct dcp_packet_s* packet)
{
    struct dcp_packet_s *p, **prev=&(uavsrv.ackqueue);
    for(p=uavsrv.ackqueue ; p!=NULL ; prev=&(p->next), p=p->next) {
        if(p == packet) {
            *prev = p->next;
            if(p->next==NULL) {
                uavsrv.ackqueue_tail = prev;
            }
            break;
        }
    } 
    
    return ((!p) ? -1 : 0);
}


/*!
 *  \brief  Find packet in ackqueue by timestamp value.
 *  
 *  \param  timestamp The value of the timestamp to find.
 *  \return A pointer to the dcp_packet_s structure or NULL on failure.
 */
struct dcp_packet_s* ackqueue_findbytimestamp(uint32_t timestamp)
{
    struct dcp_packet_s *p;
    for(p=uavsrv.ackqueue ; p!=NULL ; p=p->next) {
        if(p->timestamp == timestamp)
            return p;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
//  CALLBACKS
//-----------------------------------------------------------------------------
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



/*!
 *  \brief  Handle Acks.
 *
 *  This function find the Acked packet in the ack queue and removes
 *  it from it.
 *  
 *  \param  packet  Ack packet.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int handler_ack(struct dcp_packet_s* packet)
{
    struct dcp_packet_s* p;
    p = ackqueue_findbytimestamp(packet->timestamp);
    if(p==NULL) {
        syslog(LOG_NOTICE, "Got ack but no corresponding packet");
        return 0;
    }
    ackqueue_delete(p);
    dcp_packetfree(p);
    return 0;
}

/*!
 *  \brief  Handle DCP packet hello from central.
 *  
 *  This function handles the HelloFromCentral packet.
 *
 *  \param  packet  Hello from central packet.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int handler_hellofromcentral(struct dcp_packet_s* packet)
{
    struct dcp_packet_s* ack_p = ackqueue_findbytimestamp(packet->timestamp);
    if(!ack_p) {
       uavsrv_err = UAVSRV_ERR_NOACKPACKET;
       return -1;
    }
    ackqueue_delete(ack_p);
    dcp_packetfree(ack_p);
    
    uavsrv.myid             = (char)((packet->data[0]   ) & 0x0F);
    uavsrv.central_sessid   = (char)((packet->data[0]>>4) & 0x0F);
    syslog(LOG_INFO, "Registered: myid=%d -- central_sessid=%d", uavsrv.myid, uavsrv.central_sessid);

    uavsrv.handlers[DCP_CMDHELLOFROMCENTRAL] = handler_null;
    uavsrv.handlers[DCP_CMDACK] = handler_ack;
    uavsrv.state = REGISTERED;

    dcp_packetack(packet);

    return 0;
}




//-----------------------------------------------------------------------------
//  SEND DCP PACKETS
//-----------------------------------------------------------------------------
/*!
 *  \brief  Alloc space for new DCP packet.
 *
 *  Allocate space on heap for new DCP packet. You must call dcp_packetfree() once
 *  you don't need the packet anymore, in order to free the memory space.
 *  
 *  \return Pointer to the struct dcp_packet_s on success, NULL on failure.
 */
struct dcp_packet_s* dcp_packetnew() 
{
    struct dcp_packet_s* packet;
    packet = malloc(sizeof(struct dcp_packet_s));

    if(!packet)
        return NULL;

    memset(&(packet->dstaddrlen), 0, sizeof(struct sockaddr_storage));
    packet->dstaddrlen = sizeof(struct sockaddr_storage);
    packet->datalen     = 0;
    packet->cmd         = 0;
    packet->next        = NULL;

    return packet;
}

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
 *  \brief  Send given DCP packet.
 *  
 *  Translate the given packet into a buffer and send it.
 *
 *  \param  packet  Packet to send
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int dcp_send(struct dcp_packet_s* packet) 
{
    char buff[PDATAMAX+4];
    int bsent;

    buff[0] = (packet->cmd & 0x0F)<<4 | (packet->sessid & 0x0F);
    buff[1] = (char)((packet->timestamp>>16) & (uint32_t)0x000000FF);
    buff[2] = (char)((packet->timestamp>> 8) & (uint32_t)0x000000FF);
    buff[3] = (char)((packet->timestamp    ) & (uint32_t)0x000000FF);
    memcpy(buff+4, packet->data, packet->datalen);

    bsent=sendto(uavsrv.sock, buff, 4+packet->datalen, 0, (struct sockaddr*)&(packet->dstaddr), packet->dstaddrlen);

    return ((bsent>0) ? 0 : -1);
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
int dcp_hello(struct sockaddr_storage* dst, char *str, int len) 
{
    struct dcp_packet_s* packet = dcp_packetnew();

    packet->dstaddr     = uavsrv.params.central_addr;
    packet->dstaddrlen  = uavsrv.params.central_addrlen;
    packet->cmd         = DCP_CMDHELLOFROMREMOTE;
    packet->sessid      = DCP_SESSIDCENTRAL;
    packet->timestamp   = difftime(time(NULL), uavsrv.start_time);
    memcpy(&(packet->data), str, len);
    packet->datalen     = len;

    if(dcp_send(packet) < 0) {
        return -1;
    }
    ackqueue_add(packet);
    return 0;
}



/*!
 *  \brief  Register video servers to central database.
 *
 *  This function send the DCP Video Servers command.
 *  
 *  \param  urls    Null terminated string of one or more video
 *                  servers URLs. See DCP_VIDEOSERVERSSEPARATOR
 *                  for separator char.
 *  \return -1 is returned in case of failure and uavsrv_err is set
 *          with the corresponding error code. On Success 0 is
 *          returned.
 */
int dcp_videos(char* urls)
{
    struct dcp_packet_s* packet = dcp_packetnew();
    int len = strnlen(urls, PDATAMAX);

    packet->dstaddr     = uavsrv.params.central_addr;
    packet->dstaddrlen  = uavsrv.params.central_addrlen;
    packet->cmd         = DCP_CMDVIDEOSERVERS;
    packet->sessid      = uavsrv.central_sessid;
    packet->timestamp   = difftime(time(NULL), uavsrv.start_time);
    memcpy(&(packet->data), urls, len);
    packet->datalen     = len;


    if(dcp_send(packet) < 0) {
        return -1;
    }
    ackqueue_add(packet);
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
    packet->cmd = DCP_CMDACK;
    packet->datalen = 0;
    dcp_send(packet);

    return 0;
}



//-----------------------------------------------------------------------------
//  UAV FUNCTIONS
//-----------------------------------------------------------------------------
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
    struct timeval timeout;
    struct dcp_packet_s *packet = NULL;
    int fdnb, bufflen=64, bread;
    char buff[64];

    FD_ZERO(&readset);
    FD_SET(uavsrv.sock, &readset);
    memcpy(&timeout, &(uavsrv.params.timeout), sizeof(struct timeval));

    fdnb = select(uavsrv.sock+1, &readset, NULL, NULL, &timeout);
    switch(fdnb) {
        case 0:
            uavsrv_err = UAVSRV_ERR_TIMEDOUT;
            break;
        case -1:
            uavsrv_err = UAVSRV_ERR_SELECT;
            break;
        default:
            packet = dcp_packetnew();
            if(!packet) {
                uavsrv_err = UAVSRV_ERR_MALLOC;
                break;
            }
            bread = recvfrom(uavsrv.sock, buff, bufflen, 0, (struct sockaddr*)&(packet->dstaddr), &(packet->dstaddrlen));
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
    int i;

    /* Check the server is not already running. */
    if( uavsrv.state > NONE ) 
    {
        uavsrv_err = UAVSRV_ERR_RUNNING;
        return -1;
    }

    /* Init DCP packet handlers*/
    for(i=0 ; i<16 ; ++i) {
        uavsrv.handlers[i] = handler_null;
    }
    uavsrv.handlers[DCP_CMDHELLOFROMCENTRAL] = handler_hellofromcentral;

    /* Ack queue */
    uavsrv.ackqueue_tail = &(uavsrv.ackqueue);

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
   
    /* Say hello */
    dcp_hello(&(uavsrv.params.central_addr), uavsrv.params.info, strnlen(uavsrv.params.info, PDATAMAX));
    do {
        packet = uavsrv_dcp_waitone();
    } while(packet!=NULL && packet->cmd!=DCP_CMDHELLOFROMCENTRAL);
    if(!packet) {
        return -1;
    }
    uavsrv.handlers[packet->cmd](packet);
    dcp_packetfree(packet);

    /* Register videos servers to DB. */
    dcp_videos(uavsrv.params.videos);

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
    struct dcp_packet_s* packet;

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
    if((uavsrv.sock=socket(uavsrv.params.if_addr.ss_family, SOCK_DGRAM, 0)) < 0)
    {
        uavsrv_err = UAVSRV_ERR_SOCKET;
        return -1;
    }
    if(bind(uavsrv.sock, (struct sockaddr*)&(uavsrv.params.if_addr), uavsrv.params.if_addrlen) < 0) 
    {
        uavsrv_err = UAVSRV_ERR_BIND;
        return -1;
    }
    uavsrv.state = SOCKREADY;
    uavsrv.start_time = time(NULL);

    /* Say hello to central station */
    if( uavsrv_start() < 0 )
        return -1;


    /* -- MAIN LOOP -- */
    while(1) {
        packet = uavsrv_dcp_waitone();
        if(!packet) {
            switch(uavsrv_err) {
                case UAVSRV_ERR_TIMEDOUT:
                    syslog(LOG_NOTICE, uavsrv_errstr());
                    break;
                case UAVSRV_ERR_SELECT:
                    syslog(LOG_ERR, uavsrv_errstr());
                    break;
                default:
                    syslog(LOG_NOTICE, "Unkwnon error code from uavsrv_waitone(): %d", uavsrv_err);
                    break;
            }
            continue;
        }
        uavsrv.handlers[packet->cmd](packet);
        dcp_packetfree(packet);
    }


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