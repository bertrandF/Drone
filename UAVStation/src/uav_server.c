/*!
 *   \file  uav_server.c
 *   \brief  UAV server, receive commands from the command station 
 *   and send status report to central station.
 *  
 *  Takes care of all the DCP protocol steps for the UAV.
 *  
 *  \author  Bertrand
 *  
 *  \internal
 *       Created:  12/05/2014
 *      Revision:  none
 *      Compiler:  gcc
 *  Organization:  
 *     Copyright:  
 *  
 *  This file is part of the Drone Project
 *  Copyright (C) 12/05/2014 -- uav_server.c -- bertrand
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


