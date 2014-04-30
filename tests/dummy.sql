INSERT INTO command_stations (ip, port, info) VALUES 	('192.168.0.23', 5865, 'First command station');
INSERT INTO command_stations (ip, port, info) VALUES	('192.168.0.24', 5865, 'Second command station');

INSERT INTO drones (ip, port, info)	VALUES 	('192.168.0.25', 5867, 'First drone');
INSERT INTO drones (ip, port, info) VALUES	('192.168.0.26', 5867, 'Second drone');

INSERT INTO sessions_central_drones (iddrone) VALUES	(1);
INSERT INTO sessions_central_drones (iddrone) VALUES	(2);

INSERT INTO sessions_central_commands (idcommand) VALUES (1);
INSERT INTO sessions_central_commands (idcommand) VALUES (2);

INSERT INTO sessions (iddrone, idcommand)	VALUES	(1, 2);
