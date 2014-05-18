/*!
 *   \file  main.c
 *   \brief Main file for UAV station.
 *
 *  Parses the command line, create the socket, starts the server.
 *  
 *  \author  Bertrand.F (), 
 *  
 *  \internal
 *       Created:  11/05/2014
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

/* Get interfaces addrs includes */
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>

/* Other includes */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/wait.h>

/* Local includes */
#include "config.h"
#include "uav_server.h"
#include "dcp.h"


/*!
 *  \brief  Available long options
 *  
 *  This structure is meant to be given as an argument to
 *  the getopt_long() function for reading the command line
 *  options.
 *
 */
static struct option long_options[] = {
    {"central-host",required_argument,  NULL,   'C'},
    {"help",        no_argument,        NULL,   'h'},
    {"info",        required_argument,  NULL,   'I'},
    {"interface",   required_argument,  NULL,   'i'},
    {"ipv4",        no_argument,        NULL,   '4'},
    {"ipv6",        no_argument,        NULL,   '6'},
    {"central-port",required_argument,  NULL,   'P'},
    {"port",        required_argument,  NULL,   'p'},
    {"timeout",     required_argument,  NULL,   't'},
    {"videos",      required_argument,  NULL,   'v'},
    {0,             0,                  NULL,    0 }
};



/*!
 *  \brief  Stores the command line options.
 *
 *  Store the user configuration given from the 
 *  command line.
 *  
 */
struct options_s {
    char                    *if_name;       ///< Interface name.
    int                     sin_family;     ///< User required sock family (IPv4 or IPv6).
    unsigned short          sin_port;       ///< Listening port of the server.
    char                    *central_host;  ///< Central station host.
    unsigned short          central_port;   ///< Central station port.
    unsigned long           timeout;        ///< Select() timeout in milliseconds.
    char*                   videos;         ///< List of video servers.
    char*                   info;           ///< UAV's info string to be stored in table stations of DB.
};
static struct options_s options = {
    NULL,
    AF_INET,
    5868,
    NULL,
    5867,
    1000,
    NULL,
    NULL
};


/*!
 *  \brief  Prints program's help.
 *
 *  Displays the version of the program and 
 *  the available options.
 *  
 *  \return Void. 
 *
 */
void usage()
{
    printf("%s version %i.%i.\n", PROGRAM_NAME, PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR);
    printf("UAV's server to receive commands and send reports.\n");
    printf("Usage: %s [options]\n", PROGRAM_NAME);
    printf("\n");
    printf("Options:\n");
    printf("  -4, --ipv4         Use IPv4 only.\n");
    printf("  -6, --ipv6         Use IPv6 only.\n");
    printf("  -C, --central-host Central Station host.\n");
    printf("  -h, --help         Prints this help.\n");
    printf("  -I, --info         UAV's info string to be stored in table stations of DB.\n");
    printf("  -i, --interface    Interface to bind server on.\n");
    printf("  -P, --central-port Central Station port.\n");
    printf("  -p, --port         Port to bind the server on.\n");
    printf("  -t, --timeout      Timeout to receive a packet.\n");
    printf("  -v, --videos       List of RTSP cam servers (separator='%c').", DCP_VIDEOSERVERSSEPARATOR);
    printf("\n");
}



/*!
 *  \brief Get central station's sockaddr. 
 *  
 *  This function calls getaddrinfo to retreive central station's sockaddr's.
 *  The sockaddr will be copied to the memory pointed by the sockaddr parameter;
 *  the length of this structure is copied in len.
 *
 *  \param  saddr   Pointer to central station's sockaddr.
 *  \param  slen    Pointer central station sockaddr's length.
 *  \return -1 on failure and prints an error message. 0 on sucess.
 */
int config_central(struct sockaddr_storage *saddr, socklen_t *slen) 
{
    int err;
    char portstr[8];
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family     = options.sin_family;   /* IPv4 / IPv6 */
    hints.ai_socktype   = SOCK_DGRAM;           /* UDP */
    hints.ai_flags      = 0;                    /*  */
    hints.ai_protocol   = 0;                    /* Any protocol */
    
    sprintf(portstr, "%d", options.central_port);
    
    err = getaddrinfo(options.central_host, portstr, &hints, &res);
    if(err!=0) { 
        syslog(LOG_ERR, "getaddrinfo(): %s", gai_strerror(err));
        return -1;
    }
    else if(!res) {
        syslog(LOG_ERR, "No sockaddr for given central station");
        return -1;
    }

    *slen = res->ai_addrlen;
    memcpy(saddr, res->ai_addr, *slen);
    freeaddrinfo(res);

    return 0;
}



/*!
 *  \brief  Get interface configuration.
 *
 *  This function calls getifaddrs to retreive interface's sockaddr's.
 *  The sockaddr will be copied to the memory pointed by the sockaddr parameter;
 *  the length of this structure is copied in len.
 *  
 *  \param  saddr   Pointer to interface's sockaddr.
 *  \param  slen    Pointer to interface's sockaddr length.
 *  \return -1 on failure and prints an error message. 0 on sucess.
 */
int config_interface(struct sockaddr_storage* saddr, socklen_t *slen) 
{
    struct ifaddrs *if_addr, *if_addrs;

    *slen = (options.sin_family==AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

    if(getifaddrs(&if_addrs) < 0) {
        syslog(LOG_ERR, "Could not get interfaces configuration\n\terrno: %m");
        return -1;
    }

    for(if_addr=if_addrs ; if_addr!=NULL ; if_addr=if_addr->ifa_next) {
        if(if_addr->ifa_addr && 
                if_addr->ifa_addr->sa_family == options.sin_family && 
                strcmp(options.if_name, if_addr->ifa_name) == 0) {
            if(options.sin_family == AF_INET) {
                memcpy(saddr, if_addr->ifa_addr, *slen);
                ((struct sockaddr_in*)&saddr)->sin_port = htons(options.sin_port);
                break;
            }
            else if(options.sin_family == AF_INET6 &&
                    IN6_IS_ADDR_LINKLOCAL(((struct sockaddr_in6*)if_addr->ifa_addr)->sin6_addr.s6_addr)) {
                memcpy(saddr, if_addr->ifa_addr, *slen);
                ((struct sockaddr_in6*)saddr)->sin6_port = htons(options.sin_port);
                break;
            }
        }
    }
    if(!if_addr) {
        freeifaddrs(if_addrs);
        syslog(LOG_ERR, "Could not find suitable address for local server");
        return -1;
    }
    freeifaddrs(if_addrs);

    return 0;
}

/*!
 *  \brief  Main function.
 *
 *  Parses arguments of the command line and stores
 *  configuration in struct options_s. Then creates and
 *  binds the server's UDP socket.
 *  
 *  \param  argc    Number of arguments.
 *  \param  argv    List or arguments.
 *  \return         Exit status.
 *
 */
int main(int argc, char** argv)
{
    char opt; 
    int pid, status;
    struct uavsrv_params_s uavparams;

    /* Open Logs */
    openlog(PROGRAM_NAME, LOG_CONS | LOG_PERROR | LOG_PID, LOG_USER);

    /* Parse command line arguments */
    while( (opt=getopt_long(argc, argv, "46C:hI:i:P:p:t:v:", long_options, NULL)) > 0) {
        switch(opt) {
            case '4':
                options.sin_family = AF_INET;
                break;
            case '6':
                options.sin_family = AF_INET6;
                break;
            case 'C':
                options.central_host = optarg;
                break;
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'I':
                options.info = optarg;
                break;
            case 'i':
                options.if_name = optarg;
                break;
            case 'P':
                options.central_port = atoi(optarg);
                break;
            case 'p':
                options.sin_port = atoi(optarg);
                break;
            case 't':
                options.timeout = atol(optarg);
                break;
            case 'v':
                options.videos = optarg;
                break;
            case '?':
                break;
            default:
                break;
        }
    }  
    if( !options.if_name ) {
        syslog(LOG_ERR, "Please specify interface through the --interface option.");
        usage();
        return EXIT_FAILURE;
    }
    if( !options.central_host ) {
        syslog(LOG_ERR, "Please specify central hostname/IP through the --central-host option.");
        usage();
        return EXIT_FAILURE;
    }
    if( !options.info ) {
        syslog(LOG_ERR, "Please specify information string on UAV through the --info option.");
        usage();
        return EXIT_FAILURE;
    }


    /* GET central station sockaddr*/
    if(config_central(&(uavparams.central_addr), &(uavparams.central_addrlen)) < 0)
        return EXIT_FAILURE;
    /* GET interface sockaddr */
    if(config_interface(&(uavparams.if_addr), &(uavparams.if_addrlen)) < 0)
        return EXIT_FAILURE;
    /* GET timeout */ 
    uavparams.timeout.tv_sec    = options.timeout/1000;
    uavparams.timeout.tv_usec   = (options.timeout%1000)*1000;
    /* GET video servers */
    uavparams.videos    = options.videos;
    /* GET infostr */
    uavparams.info  = options.info;


    /* Fork child */
    while(1) {
        pid = fork();
        if(pid<0) {
            syslog(LOG_ERR, "Could not start child process\n\terrno: %m");
            return EXIT_FAILURE;
        }

        if(pid>0) { // Parent
            syslog(LOG_INFO, "Starting child process: PID=%d", pid);
            waitpid(pid, &status, 0);
            if(!WIFEXITED(status)) {
                syslog(LOG_CRIT, "CHILD CRASHED: status=%x", WEXITSTATUS(status));
            }
            if(WIFSIGNALED(status)) {
                syslog(LOG_CRIT, "CHILD TERMINATED: signal=%d", WTERMSIG(status));
            }
        }
        else if(!pid) { // Child
            /* UAV params + UAV run */
            if(uavsrv_create() < 0) {
                syslog(LOG_ERR, "uavsrv_create(): %s\n\terrno: %m", uavsrv_errstr());
                return EXIT_FAILURE;
            }
            if(uavsrv_run(&uavparams) < 0) {
                syslog(LOG_ERR, "uavsrv_run(): %s\n\terrno: %m", uavsrv_errstr());
                uavsrv_destroy();
                return EXIT_FAILURE;
            }
            uavsrv_destroy();
            break;
        }
    }

    /* Close Logs */
    closelog();

    return EXIT_SUCCESS;
}

