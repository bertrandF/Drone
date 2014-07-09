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

/* Lua */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

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

/* Defines */
#define DEFAULT_BACKUP      "/tmp/drone_config_data.bak"



/*!
 *  \brief  Stores the command line options.
 *
 *  Store the user configuration given from the 
 *  command line.
 *  
 */
struct options_s {
    const char*             if_name;       ///< Interface name.
    int                     sin_family;     ///< User required sock family (IPv4 or IPv6).
    unsigned short          sin_port;       ///< Listening port of the server.
    const char*             central_host;  ///< Central station host.
    unsigned short          central_port;   ///< Central station port.
    unsigned long           timeout;        ///< Select() timeout in milliseconds.
    char*                   videos;         ///< List of video servers.
    const char*             info;           ///< UAV's info string to be stored in table stations of DB.
    const char*             backup;         ///< Path to backup file.
};
static struct options_s options = {
    NULL,
    AF_INET,
    5868,
    NULL,
    5867,
    1000,
    NULL,
    NULL,
    NULL
};


/*!
 *  \brief Avaliable long options.
 *
 * This structure is meant to be given as an argument to
 * the getopt_long() function for reading the command line
 * options.
 */
static struct option long_options[] = {
    {"help",    no_argument,        NULL,   'h'},
    {"test",    no_argument,        NULL,   't'},
    {0,         0,                  NULL,    0 }
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
    printf("Usage: %s [options] <configuration_file>\n", PROGRAM_NAME);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help        Prints this help.\n");
    printf("  -t, --test        Check configuration file.\n");
    printf("\n");
}



/*!
 *  \brief Parse a configuraition file.
 *
 *  Open the given configuration file and fills in the options structure.
 *
 *  \param  filename    Path to the configuration file.
 *  \return -1 on failure, 0 on success.
 */
#define CONF_TAB_UAV        "uav"
#define CONF_TAB_SRV        "server"
#define CONF_TAB_CENTRAL    "central"
#define CONF_TAB_VIDEOS     "videos"

#define CONF_KEY_IPV        "ip_version"
#define CONF_KEY_BACKUP     "backup_file"
#define CONF_KEY_PORT       "port"
#define CONF_KEY_HOST       "host"
#define CONF_KEY_IF         "interface"
#define CONF_KEY_TIMEOUT    "timeout"
#define CONF_KEY_INFO       "info"
#define CONF_KEY_URL        "url"

int parse_conf_file(char* filename) 
{
    int ret=-1;
    lua_State *L;
    int len=0, total_len=0, videos_size=0;
    const char* video;

    /* Init lua */
    L = luaL_newstate();
    luaL_openlibs(L);

    /* Open configuration file */
    if( luaL_dofile(L, filename)!=0 ) {
        fprintf(stderr, "Cannot open configuration file: '%s'\n", filename);
        goto end;
    }

    /* --------------------- MAIN TABLE ------------------- */
    /* Get main table */
    lua_getglobal(L, CONF_TAB_UAV);
    if( !lua_istable(L, -1) ) {
        fprintf(stderr, "Configuration file does not have a valid '%s' table.\n", CONF_TAB_UAV);
        goto end;
    }

    /* Get ip version */
    lua_pushstring(L, CONF_KEY_IPV);
    lua_gettable(L, -2);
    if( !lua_isnumber(L, -1) ) {
        fprintf(stderr, "Bad value for '%s'\n", CONF_KEY_IPV);
        goto end;
    }
    options.sin_family = (lua_tonumber(L, -1)==4) ? AF_INET : AF_INET6; 
    lua_pop(L, 1);

    /* Get backup file */
    lua_pushstring(L, CONF_KEY_BACKUP);
    lua_gettable(L, -2);
    if( !lua_isstring(L, -1) ) {
        fprintf(stdout, "No '%s' found. Defaulting to '%s'\n", CONF_KEY_BACKUP, DEFAULT_BACKUP);
        options.backup = malloc(sizeof(DEFAULT_BACKUP)+1);
        if( !options.backup ) {
            fprintf(stderr, "Cannot allocate space for default backup filename\n");
            goto end;
        }
    } else {
        options.backup = lua_tostring(L, -1);
    }
    lua_pop(L, 1);

    /* --------------------- SERVER TABLE ----------------- */
    /* Get server table */
    lua_pushstring(L, CONF_TAB_SRV);
    lua_gettable(L, -2);
    if( !lua_istable(L, -1) ) {
        fprintf(stderr, "Table '%s' does not have a '%s' table.", CONF_TAB_UAV, CONF_TAB_SRV);
        goto end;
    }
    
    /* Get server port */
    lua_pushstring(L, CONF_KEY_PORT);
    lua_gettable(L, -2);
    if( !lua_isnumber(L, -1) ) {
        fprintf(stdout, "Table '%s' does not have a valid '%s' key. Defaulting to %u\n", 
                CONF_TAB_SRV, CONF_KEY_PORT, options.sin_port);
    } else {
        options.sin_port = (unsigned short)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    /* Get server timeout */
    lua_pushstring(L, CONF_KEY_TIMEOUT);
    lua_gettable(L, -2);
    if( !lua_isnumber(L, -1) ) { 
        fprintf(stdout, "Table '%s' does not have a valid '%s' key. Defaulting to %lu\n", 
                CONF_TAB_SRV, CONF_KEY_TIMEOUT, options.timeout);
    } else {
        options.timeout = (unsigned long)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    /* Get server info */
    lua_pushstring(L, CONF_KEY_INFO);
    lua_gettable(L, -2);
    if( !lua_isstring(L, -1) ) {
        fprintf(stderr, "Bad value for '%s' in '%s' table.\n", CONF_KEY_INFO, CONF_TAB_SRV);
        goto end;
    }
    options.info = lua_tostring(L, -1);
    lua_pop(L, 1);

    /* Get server listening interface */
    lua_pushstring(L, CONF_KEY_IF);
    lua_gettable(L, -2);
    if( !lua_isstring(L, -1) ) {
        fprintf(stderr, "Bad value for '%s' in '%s' table.\n", CONF_KEY_IF, CONF_TAB_SRV);
        goto end;
    }
    options.if_name = lua_tostring(L, -1);
    lua_pop(L, 1);

    /* --------------------- CENTRAL TABLE ---------------- */
    lua_pop(L, 1);
    /* Get central table */
    lua_pushstring(L, CONF_TAB_CENTRAL);
    lua_gettable(L, -2);
    if( !lua_istable(L, -1) ) {
        fprintf(stderr, "Table '%s' doea not have a table '%s'.\n", 
                CONF_TAB_UAV, CONF_TAB_CENTRAL);
        goto end;
    }

    /* Get central host */
    lua_pushstring(L, CONF_KEY_HOST);
    lua_gettable(L, -2);
    if( !lua_isstring(L, -1) ) {
        fprintf(stderr, "Table '%s' does not have a '%s' key.\n", 
                CONF_TAB_CENTRAL, CONF_KEY_HOST);
        goto end;
    }
    options.central_host = lua_tostring(L, -1);
    lua_pop(L, 1);

    /* Get central port */
    lua_pushstring(L, CONF_KEY_PORT);
    lua_gettable(L, -2);
    if( !lua_isnumber(L, -1) ) {
        fprintf(stdout, "Table '%s' does not have a valid '%s' key. Defaulting to %u\n", 
                CONF_TAB_CENTRAL, CONF_KEY_PORT, options.central_port);
    } else {
        options.central_port = (unsigned short)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    /* --------------------- VIDEOS TABLE ----------------- */
    lua_pop(L, 1);
    /* Get video table */
    lua_pushstring(L, CONF_TAB_VIDEOS);
    lua_gettable(L, -2);
    if( !lua_istable(L, -1) ) {
        fprintf(stderr, "Table '%s' does not have a table '%s'.\n", 
                CONF_TAB_UAV, CONF_TAB_VIDEOS);
        goto end;
    }

    /* For each video in the table */
    lua_pushnil(L);
    options.videos = (char*)malloc(2048);
    videos_size=2048;
    if( !options.videos ) {
        fprintf(stderr, "Cannot allocate space for videos string.\n");
        goto end;
    }
    while( lua_next(L, -2)!=0 ) {
        if( lua_istable(L, -1) ) {
            lua_pushstring(L, CONF_KEY_URL);
            lua_gettable(L, -2);
            if( !lua_isstring(L, -1) ) {
                fprintf(stderr, "Bad value for key '%s' in '%s' table.\n",
                        CONF_KEY_URL, CONF_TAB_VIDEOS);
                goto end;
            }
            video = lua_tostring(L, -1);
            len = strlen(video)+1;
            while(total_len+len > videos_size) {
                videos_size += 2048;
                options.videos = (char*)malloc(videos_size);
                if( !options.videos ) {
                    fprintf(stderr, "Cannot increase videos buffer size.\n");
                    goto end;
                }
            }
            strcat(options.videos, "$");
            strcat(options.videos, video);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    ret=0;
end:
    lua_close(L);
    return ret;
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
    int pid, status, itsjustatest=0;
    struct uavsrv_params_s uavparams;
    char* conffile;
    int ret=EXIT_FAILURE;

    /* Open Logs */
    openlog(PROGRAM_NAME, LOG_CONS | LOG_PERROR | LOG_PID, LOG_USER);
    
    if(argc < 2) {
        usage();
        return EXIT_FAILURE;
    }
    while( (opt=getopt_long(argc, argv, "ht", long_options, NULL))>0 ) {
        switch(opt) {
            case 'h':
                usage();
                ret=EXIT_SUCCESS;
                goto end;
            case 't':
                itsjustatest=1;
                break;
            case '?':
                usage();
                goto end;
            default:
                break;
        }
    }
    if( optind>=argc || optind<0 ) {
        fprintf(stderr, "Please specify path to configuration file.\n");
        usage();
        goto end;
    }
    conffile = argv[optind];

    if(parse_conf_file(conffile) < 0)
        goto end;
    else if( itsjustatest ) {
        ret=EXIT_SUCCESS;
        goto end;
    }

    /* GET central station sockaddr*/
    if(config_central(&(uavparams.central_addr), &(uavparams.central_addrlen)) < 0)
        goto end;
    /* GET interface sockaddr */
    if(config_interface(&(uavparams.if_addr), &(uavparams.if_addrlen)) < 0)
        goto end;
    /* GET timeout */ 
    uavparams.timeout.tv_sec    = options.timeout/1000;
    uavparams.timeout.tv_usec   = (options.timeout%1000)*1000;
    /* GET video servers */
    uavparams.videos    = options.videos;
    /* GET infostr */
    uavparams.info  = options.info;
    /* GET backup file */
    uavparams.backup        = options.backup;
    uavparams.backup_mode   = 0;


    /* Fork child */
    while(1) {
        pid = fork();
        if(pid<0) {
            syslog(LOG_ERR, "Could not start child process\n\terrno: %m");
            goto end;
        }

        if(pid>0) { // Parent
            uavparams.backup_mode = 1;
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
                goto end;
            }
            if(uavsrv_run(&uavparams) < 0) {
                syslog(LOG_ERR, "uavsrv_run(): %s\n\terrno: %m", uavsrv_errstr());
                uavsrv_destroy();
                goto end;
            }
            uavsrv_destroy();
            break;
        }
    }
    ret=EXIT_SUCCESS;

end:
    /* Close Logs */
    closelog();
    return ret;
}

