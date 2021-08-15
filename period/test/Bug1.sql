--
--   File: Bug1.sql
--
--  About:
--
CREATE TABLE Reservations (
	Id			SERIAL8,
	Who			INTEGER	NOT NULL,
	Hotel		INTEGER NOT NULL,
	Room		INTEGER	NOT NULL,
	Check_In	DATE	NOT NULL,
	Check_Out	DATE	NOT NULL
);
--
LOAD FROM 'Res.dat'
INSERT INTO Reservations;
--
SELECT 'There should be 1 of these, and there are ' || COUNT(*) 
  FROM Reservations V
 WHERE Overlap ( Period ( V.Check_In, V.Check_Out ),
                 Period ( DATE('06/05/1998'),
                                 DATE('06/10/1998')
                               )
               );
--
--  OK. 
-- 
CREATE INDEX Res_Ndx2
ON Reservations ( Period ( Check_In, Check_Out ) rtree_ops )
USING RTREE;
--
UPDATE STATISTICS HIGH FOR TABLE Reservations;
--
UPDATE systables
   SET nrows = 100000,
       npused = 1500
 WHERE tabname = 'reservations';
--
--  At this point you will need to bounce the database to get these
-- numbers to take effect so that you can step through the
-- index.
--
