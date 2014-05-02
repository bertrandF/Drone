#/usr/bin/bash

##########################################################################
#
# This file is part of the Drone project
# Copyright (C) 2014/04/24 -- test.sh -- bertrand
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
##########################################################################
SCRIPTDIR=`dirname $0`
DBDIR="$SCRIPTDIR/../Database/"


usage() 
{
	echo "Test launching script for Drone project."
	echo "Usage: test.sh <testname> [options]"
	echo ""
	echo "Options:"
	echo ""
}

usage_db() 
{
	echo "Manage drone database."
	echo "Usage: test.sh db <action> [options]"
	echo ""
	echo "Actions:"
	echo "  create    Executes create.sql script."
	echo "  drop      Executes drop.sql script."
	echo "  dump      Dumps tables."
	echo "  fill      Executes dummy.sql script."
	echo "  help      Prints help this message."
	echo "  inter     Interactive psql session."
	echo "  rebuild   Executes drop,create,fill."
}

test_db()
{
	DBNAME="drones"
	DBUSER="dronedbmanager"
	DBCREATEFILE="$DBDIR/create.sql"
	DBDROPFILE="$DBDIR/drop.sql"
	DBFILLFILE_1="$SCRIPTDIR//dummy.sql"
	
	if (($# < 2)); then
		echo "Bad number of arguments."
		usage_db
		exit
	fi

	if [ "$2" == "create" ]; then
		psql -d $DBNAME -U $DBUSER -f $DBCREATEFILE
	elif [ "$2" == "drop" ]; then
		psql -d $DBNAME -U $DBUSER -f $DBDROPFILE
	elif [ "$2" == "dump" ]; then
		echo "/**** command_station ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM command_stations'
		echo "/**** drones ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM drones'
		echo "/**** sessions ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM sessions'
		echo "/**** sessions_central_commands ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM sessions_central_commands'
		echo "/**** sessions_central_drones ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM sessions_central_drones'
		echo "/**** logs ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM logs'
	elif [ "$2" == "fill" ]; then
		psql -d $DBNAME -U $DBUSER -f $DBFILLFILE_1
	elif [ "$2" == "help" ]; then
		usage_db
	elif [ "$2" == "inter" ]; then
		psql -d $DBNAME -U $DBUSER
	elif [ "$2" == "rebuild" ]; then
		psql -d $DBNAME -U $DBUSER -f $DBDROPFILE
		psql -d $DBNAME -U $DBUSER -f $DBCREATEFILE
		psql -d $DBNAME -U $DBUSER -f $DBFILLFILE_1
	else
		echo "Unknown action: $2"
		usage_db
	fi
	
	exit
}

# Check that we got at least a testname
if (($# < 1)); then 
	usage
	exit 
fi
TESTNAME=$1

# Call corresponding script
if [ "$TESTNAME" == "db" ]; then
	test_db $@	
fi
