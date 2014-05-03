CREATE SEQUENCE stations_id_seq
	INCREMENT BY 1
	MINVALUE 0
	MAXVALUE 15
	START WITH 0
	NO CYCLE;

CREATE TYPE station_type AS ENUM ( 
	'central', 
	'command', 
	'drone'
);	

CREATE TABLE stations (
	id		smallint			DEFAULT nextval('stations_id_seq') PRIMARY KEY,
	type	station_type	NOT NULL,
	ip		inet					NOT NULL,
	port	integer				CHECK (port>0) CHECK (port<65536),
	date	timestamp			with time zone NOT NULL default current_timestamp,
	info	varchar(1024)	
);



CREATE TABLE videos (
	id			smallint 			REFERENCES stations (id) PRIMARY KEY,
	videos	varchar(4096)	NOT NULL
);



CREATE SEQUENCE sessions_id_seq 
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 15
	START WITH 1
	NO CYCLE;

CREATE TABLE sessions (
	id				smallint	DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	station1	integer 	REFERENCES stations (id) NOT NULL,
	station2	integer 	REFERENCES stations (id) NOT NULL,
	date			timestamp	with time zone NOT NULL default current_timestamp
);



CREATE TYPE log_level AS ENUM (
	'info',
	'warning',
	'critical',
	'fatal'
);

CREATE TABLE logs (
	id				smallint	REFERENCES stations (id),
	level			log_level	NOT NULL,
	date			timestamp	with time zone NOT NULL default current_timestamp,
	msg				varchar(2048),
	PRIMARY KEY (id, date)
);



GRANT SELECT ON ALL TABLES IN SCHEMA public TO dronedbreader;
