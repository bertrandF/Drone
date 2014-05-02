CREATE SEQUENCE remotes_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 10
	START WITH 1
	NO CYCLE;

CREATE TABLE command_stations (
	id 		smallint		 	DEFAULT nextval('remotes_id_seq') PRIMARY KEY,
	ip		inet					NOT NULL,
	port	integer				CHECK (port>0) CHECK (port<65536),
	date	timestamp			with time zone NOT NULL default current_timestamp,
	info	varchar(1024)	
);

CREATE TABLE drones (
	id		smallint		DEFAULT nextval('remotes_id_seq') PRIMARY KEY,
	ip		inet				NOT NULL,
	port	integer			CHECK (port>0) CHECK (port<65536),
	date	timestamp		with time zone NOT NULL default current_timestamp,
	info	varchar(1024)	
);



CREATE SEQUENCE sessions_id_seq 
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 15
	START WITH 1
	NO CYCLE;

CREATE TABLE sessions (
	id				smallint	DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	iddrone		integer 	REFERENCES drones (id) NOT NULL,
	idcommand	integer 	REFERENCES command_stations (id) NOT NULL,
	date			timestamp	with time zone NOT NULL default current_timestamp
);

CREATE TABLE sessions_central_drones (
	id			smallint 		DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	iddrone	integer			REFERENCES drones (id) NOT NULL,
	date		timestamp with time zone NOT NULL default current_timestamp
);

CREATE TABLE sessions_central_commands (
	id				smallint 		DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	idcommand	integer			REFERENCES command_stations (id) NOT NULL,
	date			timestamp with time zone NOT NULL default current_timestamp
);



CREATE TYPE logLevel AS ENUM (
	'Info',
	'Warning',
	'Critical',
	'Fatal'
);

CREATE TABLE logs (
	date			timestamp with time zone NOT NULL default current_timestamp,
	level			logLevel NOT NULL,
	msg				varchar(2048)
);


GRANT SELECT ON ALL TABLES IN SCHEMA public TO dronedbreader;
