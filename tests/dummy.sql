INSERT INTO stations (type, ip, port, info) VALUES
	('drone', '192.168.0.86', 5866, 'UAV-00237XFT01');
INSERT INTO stations (type, ip, port, info) VALUES
	('command', '192.168.0.75', 5865, 'Command Station XFLDE00056');
INSERT INTO stations (type, ip, port, info) VALUES
	('command', '192.168.0.74', 5865, 'Command Station FLROV00024');
INSERT INTO stations (type, ip, port, info) VALUES
	('drone', '192.168.0.87', 5866, 'UAV-00248LHIYK');
INSERT INTO stations (type, ip, port, info) VALUES
	('drone', '192.168.0.88', 5866, 'UAV-00756QLTMV');


INSERT INTO videos (id, videos) VALUES
	(1, 'rtsp://192.168.0.87/frontVideo');
INSERT INTO videos (id, videos) VALUES
	(4, 'rtsp://192.168.0.87/bottomVideo$rtsp://192.168.0.87/frontVideo');


INSERT INTO sessions (station1, station2) VALUES
	(0, 1);
INSERT INTO sessions (station1, station2) VALUES
	(0, 2);
INSERT INTO sessions (station1, station2) VALUES
	(0, 3);
INSERT INTO sessions (station1, station2) VALUES
	(0, 4);
INSERT INTO sessions (station1, station2) VALUES
	(0, 5);
INSERT INTO sessions (station1, station2) VALUES
	(1, 3);
INSERT INTO sessions (station1, station2) VALUES
	(2, 5);


INSERT INTO logs (id, level, msg) VALUES
	(0, 'info', 'Central station taking control of this NET');
INSERT INTO logs (id, level, msg) VALUES
	(1, 'info', 'Booting up ...');

