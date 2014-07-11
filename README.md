Drone
=====

This project aims at building a whole system in which several drones and command stations can dynamicaly join the net, connect to each other, quit the net. The system as three main nodes: 

  1- CENTRAL STATION : one per net, manage the connection between drones and command stations, its Id is always 0 (therefore it must be the first node to register on the DB). When a command station wants to connect to a drone it must first ask to this node.  Every command station and drone must connect to this node 
  before being allowed to speak on the net.
  
  2- COMMAND STATION : send commands and monitor the drones one at a time : you must disconnect from your current drone to send command to an other, or you can launch a second instance of that program. 
  
  3- UAV STATION : the drone itself, register itself on the DB so that it is known and wait for command stations to connect and send commands.


The project is organized as follows :

- CentralStation (C++, Qt) : 
Central point for handling connections between Drones/CommandStationCommandStations. It also receives data from drones and push it into a PostgresDB.

- CommandStation (C++, Qt) : 
GUI for controling drones. Connects to a central station and DB, then chose a drone to connect to and monitors it + send commands.

- Database (SQL) : 
Scripts to create / delete the tables/sequences in the database. A PostgreSQL db is used. The DB contains all the info on the drones/commands station (id, address, port, time of connection, description string, urls of streaming servers, logs).

- UAVStation (pure C + CMake + Doxygen doc!) : 
This software is meant to be run on the UAV/Drone. It register itself on the DB throught the central station and wait for a command station to connect to it and send orders.

- flightInstruments (C++, Qt) : 
A library used by command station. Contains all the widgets representing flight instrument (altimeter, artificial horizon, compass).

- libDCP (C++, Qt) : 
Drone Communication Protocol, defines packets and classes for communication. Used only by central station and command station as the UAV uses it own lighter implementation of the protocol.

- logo : 
Most important part of the project: the logo !!!

- test (bash, sql): 
Scripts to quickly create/drop/dump/fill with dummy data the db OR deploy the whole system in one command.

- SDL-rtmp-player ( C ):
This is the player used to read RTMP streams from the drones. It need rtmpdump which it starts as a child process and pipe its output to stdin from where it gets the video frames. The frames are processed with libav and displayed with SDL.

How to Run:
=====
  1- Copy the SDLplayer executable to the same directory as CommandStation.
  
  2- Start your postgresql deamon.
  
  3- run ./tests/test.sh db drop to clean the drone DB.
  
  4- run ./tests/test.sh db create to create the drone DB.
  
  5- Start your CentralStation ( 1st arg is IP of interface to listen to, 2nd arg is port). It will register itself on the DB.

  6- Start your RTMP server and ffmpeg to send the video to it. You can use the UAVStation/run.py script to do that.
  
  7- Start your UAVStation.  There is my sample configuration file conf.lua, you might want to change the IP for the RTMP stream and some other things.
  
  8- Run the CommandStation. Select the drone from the DB.
  
