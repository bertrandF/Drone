CREATE TABLE command_stations (
	id 		smallserial 	PRIMARY KEY,
	mac		macaddr				NOT NULL,
	ip		inet					NOT NULL,
	port	integer				CHECK (port>0) CHECK (port<65536),
	date	timestamp			with time zone NOT NULL,
	info	varchar(1024)	
);

CREATE TABLE drones (
	id		smallserial	PRIMARY KEY,
	mac		macaddr			NOT NULL,
	ip		inet				NOT NULL,
	port	integer			CHECK (port>0) CHECK (port<65536),
	date	timestamp		with time zone NOT NULL,
	info	varchar(1024)	
);

CREATE SEQUENCE sessions_id_seq 
	INCREMENT BY 1
	MINVALUE 0
	MAXVALUE 15
	START WITH 0
	CYCLE;

CREATE TABLE sessions (
	id				smallint	DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	iddrone		integer 	REFERENCES drones (id) NOT NULL,
	idstation	integer 	REFERENCES command_stations (id) NOT NULL,
	date			timestamp	with time zone NOT NULL
);

ALTER SEQUENCE sessions_id_seq OWNED BY sessions.id;
