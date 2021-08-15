--
--   File: Railways.sql
--
--  About:
--
--    This script illustrates how to use the Period_DT and Period types
--  in anger. It uses as an example a Railway scheduling system.
--
--    Note that this requires the Random() bladelet, because it generates
--  a bunch of schedule data.
--
--
-- DROP TABLE Delete_TS;
-- DROP FUNCTION BIGGER ( INTEGER, INTEGER );
-- DROP TABLE Track_Schedule;
-- DROP TABLE Trains;
-- DROP TABLE Tracks;
-- DROP TYPE Path RESTRICT;
--
------------------------------- SCHEMA ---------------------------------------
--
CREATE DISTINCT TYPE Path AS VARCHAR(48);
--
CREATE TABLE Tracks (
	Id		SERIAL		PRIMARY KEY,
	Where		Path		NOT NULL
);
--
CREATE TABLE Trains (
	Id		SERIAL		PRIMARY KEY,
	Name 		VARCHAR(32)	NOT NULL
);
--
CREATE TABLE Track_Schedule  (
	Id	 SERIAL		PRIMARY KEY,
        Train    INTEGER NOT NULL,
        Track    INTEGER NOT NULL,
        Starts   DATETIME YEAR TO SECOND NOT NULL,
        Ends     DATETIME YEAR TO SECOND NOT NULL,
	CHECK ( Starts < Ends ) CONSTRAINT Starts_must_preceed_Ends,
	FOREIGN KEY ( Train ) REFERENCES Trains ( Id ) CONSTRAINT Train_FK,
	FOREIGN KEY ( Track ) REFERENCES Tracks ( Id ) CONSTRAINT Track_FK
);
--
------------------------------- DATA -----------------------------------------
--
INSERT INTO Tracks ( Where )
SELECT ('[ ' || T1.Val || ' to ' || T2.Val || ' ]')::VARCHAR(32)::Path
  FROM TABLE(SET{'(-125.32, 37.15)','(-115.79, 27.22)','(-121.37, 30.2)',
                 '(-117.29, 32.2)','(-125.95, 21.15)','(-121.32, 36.59)'}
            ) T1 ( Val ),
       TABLE(SET{'(-125.32, 37.15)','(-115.79, 27.22)','(-121.37, 30.2)',
                 '(-117.29, 32.2)','(-125.95, 21.15)','(-121.32, 36.59)'}
            ) T2 ( Val )
 WHERE T1.Val != T2.Val;
--
INSERT INTO Trains ( Name )
SELECT T.Name
  FROM TABLE(SET{'Conrail','Burlington Northern','Amtrak',
                 'Baja Express','NY Met Rail'}) T ( Name);
--
INSERT INTO Track_Schedule
( Train, Track, Starts, Ends )
SELECT TR.Id,
       TK.Id,
       (CURRENT::DATETIME YEAR TO SECOND + Rand(150) UNITS DAY ) - Rand((24*60)) UNITS MINUTE,
	CURRENT + 200 UNITS DAY
  FROM Tracks TK, Trains TR;
--
UPDATE Track_Schedule
   SET Ends = Starts + 1 UNITS DAY + RAND(3) UNITS DAY - 
			RAND((24*60)) UNITS MINUTE
  WHERE 1 = 1;
--
CREATE FUNCTION BIGGER( Arg1 INTEGER, Arg2 INTEGER )
RETURNS INTEGER
	IF ( Arg1 > Arg2 ) THEN
		RETURN Arg1;
	END IF;

	RETURN Arg2;
END FUNCTION;
--
--   OK. Now for the tricky part. I need to ensure that there is nothing
--  completely bogus in this test data. To achieve this I delete all
--  scheduled events for which there is a conflict.
--
SELECT DISTINCT BIGGER(T1.Id, T2.Id) AS Id
  FROM Track_Schedule T1, Track_Schedule T2
 WHERE (( T1.Track = T2.Track ) OR ( T1.Train = T2.Train ))
   AND NOT (( T1.Ends < T2.Starts ) OR ( T1.Starts > T2.Ends ))
   AND T1.Id != T2.Id
INTO TEMP Delete_TS;
--
SELECT COUNT(*) FROM Delete_TS;
--
-- SELECT Train, Starts, Ends, Track
--   FROM Track_Schedule
--  ORDER BY 1,2,3,4;
--
-- SELECT Track, Starts, Ends, Train
--  FROM Track_Schedule
--  ORDER BY 1,2,3,4;
--
DELETE FROM Track_Schedule 
 WHERE Id IN ( SELECT Id FROM Delete_Ts );
--
SELECT COUNT(*) FROM Track_Schedule;
--
--  OK. Now for the query:
--
--  Q1: "How many trains are scheduled on Tracks for the next week?"
--
SELECT COUNT( DISTINCT T.Train ) 
  FROM Track_Schedule T
 WHERE NOT (( T.Ends   < CURRENT ) OR 
            ( T.Starts > CURRENT + 7 UNITS DAY ));
--
------------------------------ CLEANUP ---------------------------------------
--
DROP TABLE Delete_TS;
DROP FUNCTION BIGGER ( INTEGER, INTEGER );
DROP TABLE Track_Schedule;
DROP TABLE Trains;
DROP TABLE Tracks;
--
DROP TYPE Path RESTRICT;
