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
BUILDDIR="$SCRIPTDIR/../../build-Drone-Desktop-Debug/"

COMMANDSTATION="$BUILDDIR/CommandStation/CommandStation"
CENTRALSTATION="$BUILDDIR/CentralStation/CentralStation"

DBNAME="drones"
DBUSER="dronedbmanager"



usage() 
{
	echo "Test launching script for Drone project."
	echo "Usage: test.sh <testname> [options]"
	echo ""
	echo "Actions:"
	echo "  db        Manage drone database."
	echo "  test1     Run test with setup 1."
}



##############
###   DB   ###
##############
usage_db() 
{
	echo "Manage drone database."
	echo "Usage: test.sh db <action> [options]"
	echo ""
	echo "Actions:"
	echo "  create    Executes create.sql script."
	echo "  drop      Executes drop.sql script."
	echo "  dump      Dumps tables."
	echo "  fill <n>  Executes script the Nth dummy script."
	echo "  help      Prints help this message."
	echo "  inter     Interactive psql session."
	echo "  rebuild   Executes drop,create,fill."
}

test_db()
{
	DBCREATEFILE="$DBDIR/create.sql"
	DBDROPFILE="$DBDIR/drop.sql"
	
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
		echo "/**** stations ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM stations'
		echo "/**** video ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM videos'
		echo "/**** sessions ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM sessions'
		echo "/**** logs ****/"
		psql -d $DBNAME -U $DBUSER -c 'SELECT * FROM logs'
	elif [ "$2" == "fill" ]; then
		if (($# < 3)); then
			echo "FILL: Bad number of arguments."
			usage_db
			exit
		fi
		psql -d $DBNAME -U $DBUSER -f "$SCRIPTDIR/dummy$3.sql"
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
	
}



##############
### TEST 1 ###
##############
usage_test1() 
{
	echo "Run test with setup 1."
	echo "Usage: test.sh test1 <action> [options]"
	echo ""
	echo "Actions:"
	echo "  help      Prints this help."
	echo "  run       Run the test."
	
}

test_test1()
{
	if (($# < 2)); then
		echo "Bad number of arguments."
		usage_test1
		exit
	fi
	
	
	if [ "$2" == "help" ]; then
		usage_test1
	elif [ "$2" == "run" ]; then
		LOCALIP=`ip -4 -o address show wlp3s0 | \
								egrep --color -o '([0-9]+\.){3}[0-9]+' | \
								head -n1`
		
		# CREATE DB
		test_db db drop 
		test_db db create
		
		# START CENTRAL STATION and WAIT FOR START
		xterm -geometry 100x30+0+0 -e $CENTRALSTATION $LOCALIP 5866 &
		PIDCENTRAL=$!
		sleep 2
		
		# START DUMMY RTSP SERVERS
		STREAMFILE1="/home/bertrand/DATA/Movies/Goodfellas[1990][Special.Edition]DvDrip[Eng]-Zeus_Dias.avi"
		RTSP1="rtsp://$LOCALIP:8080/frontVideo.sdp"
		vlc --quiet $STREAMFILE1 --sout "#rtp{sdp=$RTSP1}" &
		PIDVLC1=$!
		
		STREAMFILE2="/home/bertrand/DATA/Movies/28.jours.Plus.tard.avi"
		RTSP2="rtsp://$LOCALIP:8081/frontVideo.sdp"
		vlc --quiet $STREAMFILE1 --sout "#rtp{sdp=$RTSP2}" &
		PIDVLC2=$!
		
		# FILL DB WITH DUMMIES
		psql -d $DBNAME -U $DBUSER -c "INSERT INTO stations (type, ip, port, info) VALUES ('drone', '$LOCALIP', 5867, 'UAV-00237XFT01')" 1> /dev/null
		psql -d $DBNAME -U $DBUSER -c "INSERT INTO videos (id, videos) VALUES (1, '$STREAMFILE1\$$STREAMFILE2')" 1> /dev/null
		psql -d $DBNAME -U $DBUSER -c "INSERT INTO sessions (station1, station2) VALUES (0, 1)" 1> /dev/null
		
		# START DUMMY DRONE
		xterm -geometry 100x30+0+500 -e ncat -lu $LOCALIP 5867 &
		PIDDRONE=$!
		
		# START COMMAND STATION and WAIT EXIT
		$COMMANDSTATION
		
		# CLEAN
		kill $PIDCENTRAL
		kill $PIDDRONE
		kill $PIDVLC1
		kill $PIDVLC2
	fi
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
elif [ "$TESTNAME" == "test1" ]; then
	test_test1 $@
fi
