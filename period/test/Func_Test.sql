--
--  Func_Test.sql
--
--   This file contains a script that does both the functional testing
--  of the Period BladeLet, and also the Performance testing.
--  The first thing to note is that this uses the Random BladeLet to
--  generate values that test a variety of values.
--
--   Note: Make sure that the Random stuff is loaded before you try for this.
--
--
---------------------------  -- DROPS ---------------------------------
--
--  This section drops the various parts of the test, but is only 
--  useful where you are repeatedly running these tests, and you're
--  getting errors.
--
-- DROP TABLE Foo;
-- DROP TABLE Bar;
-- DROP ROW TYPE Foo_Type restrict;
-- DROP FUNCTION CURRENT_DATETIME();
-- DROP TABLE Timings;
-- DROP TABLE Numbers;
-- DROP TABLE Ints;
-- DROP ROW TYPE Number_Type RESTRICT;
-- DROP TABLE Reservations;
-- DROP TABLE Conferences;
-- DROP TABLE QueryResults;
--
-------------------------------------------------------------------
--
--   This function is intended for usage with the performance estimates
--  stuff.
--
CREATE FUNCTION CURRENT_DATETIME ()
RETURNING datetime year to fraction(3);
        RETURN CURRENT::datetime year to fraction(3);
END FUNCTION;
--
--  Timings: Used to store the timings.
--
CREATE TABLE Timings (
	Step	INTEGER	NOT NULL,
	Name	VARCHAR(128)	NOT NULL,
	Time	datetime year to fraction(3)
);
--
CREATE ROW TYPE Foo_Type
(       A       INTEGER         NOT NULL,
        B       Period   NOT NULL
);
--
CREATE TABLE Foo
OF TYPE Foo_Type;
--
CREATE TABLE Bar
OF TYPE Foo_Type;
--
--  Stuff . .
--
INSERT INTO Foo
VALUES
(1, "01/01/96 to 01/31/96");
--
INSERT INTO Foo
VALUES
(2, "01/01/95 to 01/31/95");
--
INSERT INTO Foo
VALUES
(3, "01/01/97 to 01/31/97");
--
INSERT INTO Foo
VALUES
(4, "01/13/96 to 01/13/96");
--
INSERT INTO Foo
VALUES
(5, "01/10/96 to 01/20/96");
--
INSERT INTO Foo
VALUES
(6, "12/15/95 to 01/15/96");
--
INSERT INTO Foo
VALUES
(7, "01/15/95 to 02/15/96");
--
--
INSERT INTO Bar
VALUES
(1, "01/01/96 to 01/31/96");
--
--
--  Now the tests . . . .
--
--   Q1: Just some book-keeping tests
--
SELECT * FROM Foo
UNION
SELECT * FROM Bar;
--
SELECT F1.A,
       COUNT(*) AS Overlap
  FROM Foo F1, Foo F2
 WHERE Overlap(F1.B, F2.B)
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       SUM( Days_Overlap )
  FROM Foo F1, Foo F2
 WHERE Overlap(F1.B, F2.B, Days_Overlap # INTEGER )
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       COUNT(*) AS Contains
  FROM Foo F1, Foo F2
 WHERE Contains(F1.B, F2.B)
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       COUNT(*) AS Contained
  FROM Foo F1, Foo F2
 WHERE Contained(F1.B, F2.B)
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT SUM(Length(F1.B))
  FROM Foo F1;
--
SELECT Start(F1.B), Finish(F1.B)
  FROM Foo F1;
--
SELECT Period(Start(F1.B), Finish(F1.B))
  FROM Foo F1;
--
SELECT F1.A,
       COUNT(*) AS Overlap
  FROM Foo F1, Foo F2
 WHERE Overlap( Period ( Start(F1.B), Finish(F1.B) ),
                Period ( Start(F2.B), Finish(F2.B) ))
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       SUM( Days_Overlap )
  FROM Foo F1, Foo F2
 WHERE Overlap( Period ( Start(F1.B), Finish(F1.B) ),
                Period ( Start(F2.B), Finish(F2.B) ),
                Days_Overlap # INTEGER  )
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       COUNT(*) AS Contains
  FROM Foo F1, Foo F2
 WHERE Contains( Period ( Start(F1.B), Finish(F1.B) ),
                Period ( Start(F2.B), Finish(F2.B) ))
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       SUM ( Days_Overlap )
  FROM Foo F1, Foo F2
 WHERE Contains( Period ( Start(F1.B), Finish(F1.B) ),
                 Period ( Start(F2.B), Finish(F2.B) ),
                 Days_Overlap # INTEGER )
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       COUNT(*) AS Contained
  FROM Foo F1, Foo F2
 WHERE Contained( Period ( Start(F1.B), Finish(F1.B) ),
                  Period ( Start(F2.B), Finish(F2.B) ))
   AND F1.A != F2.A
 GROUP BY F1.A;
--
SELECT F1.A,
       SUM ( Days_Overlap )
  FROM Foo F1, Foo F2
 WHERE Contained( Period ( Start(F1.B), Finish(F1.B) ),
                  Period ( Start(F2.B), Finish(F2.B) ),
                  Days_Overlap # INTEGER )
   AND F1.A != F2.A
 GROUP BY F1.A;
--
--------------------------------------- Scale Up ----------------------------
--
--
CREATE ROW TYPE Number_Type ( A INTEGER NOT NULL, B VARCHAR(32) NOT NULL );
CREATE TABLE Numbers OF TYPE Number_Type;
--
INSERT INTO Numbers VALUES ( 0, 'Zero' );
INSERT INTO Numbers VALUES ( 1, 'One' );
INSERT INTO Numbers VALUES ( 2, 'Two' );
INSERT INTO Numbers VALUES ( 3, 'Three' );
INSERT INTO Numbers VALUES ( 4, 'Four' );
INSERT INTO Numbers VALUES ( 5, 'Five' );
INSERT INTO Numbers VALUES ( 6, 'Six' );
INSERT INTO Numbers VALUES ( 7, 'Seven' );
INSERT INTO Numbers VALUES ( 8, 'Eight' );
INSERT INTO Numbers VALUES ( 9, 'Nine' );
--
CREATE TABLE Ints OF TYPE Number_Type;
--
INSERT INTO Ints
SELECT N1.A * 1000 + N2.A * 100 + N3.A * 10 + N4.A,
       N1.B || '-' || N2.B || '-' || N3.B || '-' || N4.B
  FROM Numbers N1, Numbers N2, Numbers N3, Numbers N4;
--
CREATE INDEX Ints_Ndx1 ON Ints(A);
--
UPDATE STATISTICS HIGH;
--
--  This is the Conferences/Reservations example. The basic idea is that this
-- mimics a HOTEL reservation system. The tests here are intended to
-- be both scalability testing, and also an example of how to use
-- the BladeLet. Therefore, this set of tests includes a comparison
-- with the alternative approach, which uses standard relational
-- techniques to address the same problem.
--
CREATE TABLE Reservations
(
        Room_Id         char(4)             NOT NULL,
        Customer_Name   varchar(128)        NOT NULL,
        Reservation     Period       NOT NULL
);
--
--
CREATE TABLE Conferences
(
        Conf_Name       varchar(32)         NOT NULL,
        Duration        Period       NOT NULL
);                                                              
--
CREATE TABLE QueryResults (
	Query		INTEGER	NOT NULL,
	Conf_Name	VARCHAR(32)	NOT NULL,
	Cnt		INTEGER	NOT NULL
);
--
DELETE FROM Timings WHERE 1 = 1;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 1, 'Start Time', CURRENT_DATETIME ());
--
INSERT INTO Reservations
SELECT N1.A,
       'Customer ' || N1.A || '-' || N2.A || ' with this padding',
       Period((34000 + (N2.A * 5) + Mod(N1.A,7)),
                 (34000 + (N2.A * 5) + Mod(N1.A,7) + Binomial(4, 0.75)))
  FROM Ints N1,
             Ints N2
 WHERE N1.A < 25
   AND N2.A < 500;
--      
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 2, 'First Insert into Reservations', CURRENT_DATETIME ());    
--
INSERT INTO Reservations
SELECT N1.A,
       'Customer ' || N1.A || '-' || N2.A || ' with this padding',
       Period((34000 + (N2.A * 5) + Mod(N1.A,7)),
                 (34000 + (N2.A * 5) + Mod(N1.A,7) + Binomial(4, 0.75)))
  FROM Ints N1,
             Ints N2
 WHERE N1.A > 24 AND N1.A < 50
   AND N2.A < 500;
--  
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 3, 'Second Insert into Reservations', CURRENT_DATETIME ());    
--
INSERT INTO Reservations
SELECT N1.A,
       'Customer ' || N1.A || '-' || N2.A || ' with this padding',
       Period((36500 + (N2.A * 5) + Mod(N1.A,7)),
                 (36500 + (N2.A * 5) + Mod(N1.A,7) + Binomial(4, 0.75)))
  FROM Ints N1,
             Ints N2
 WHERE N1.A < 25
   AND N2.A < 500;        
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 4, 'Third Insert into Reservations', CURRENT_DATETIME ());      
--
INSERT INTO Reservations
SELECT N1.A,
       'Customer ' || N1.A || '-' || N2.A || ' with this padding',
       Period((36500 + (N2.A * 5) + Mod(N1.A,7)),
                 (36500 + (N2.A * 5) + Mod(N1.A,7) + Binomial(4, 0.75)))
  FROM Ints N1,
             Ints N2
 WHERE N1.A > 24 AND N1.A < 50
   AND N2.A < 500;  
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 5, 'Fourth Insert into Reservations', CURRENT_DATETIME ());  
--
INSERT INTO Conferences
SELECT 'Conference on ' || N1.A || '-' || N2.A,
       Period((34000 + (N2.A * 16) + (N1.A * 2)),
                 (34000 + (N2.A * 16) + (N1.A * 2) + Binomial(11, 0.5)))
  FROM Ints N1,
             Ints N2
 WHERE N1.A < 3
   AND N2.A < 500;
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 6, 'Insert into Conferences', CURRENT_DATETIME ());  
--
INSERT INTO QueryResults
 SELECT 1,
	    C.Conf_Name,
        COUNT(*)
  FROM Conferences C,
       Reservations R
 WHERE Overlap(C.Duration,'01/01/96 to 03/31/96')
   AND Overlap(C.Duration,R.Reservation)
  GROUP BY C.Conf_Name;                    
--   
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 7, 'Finished First Query', CURRENT_DATETIME ());  
--
CREATE INDEX Res_ndx1 ON Reservations(Reservation Period_ops) USING rtree;
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 8, 'Finished Creating the Reservations Index', CURRENT_DATETIME ());  
--
CREATE INDEX Conf_ndx1 ON Conferences(Duration Period_ops) USING rtree; 
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 9, 'Finished Creating the Conferences Index', CURRENT_DATETIME ());  
--
UPDATE STATISTICS MEDIUM;
--
COMMIT WORK;
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 10, 'Finished UPDATE STATISTICS', CURRENT_DATETIME ());  
--
INSERT INTO QueryResults
 SELECT 2,
  	    C.Conf_Name,
        COUNT(*)
  FROM Conferences C,
       Reservations R
 WHERE Overlap(C.Duration,'01/01/96 to 03/31/96')
   AND Overlap(C.Duration,R.Reservation)
  GROUP BY C.Conf_Name;                    
--   
--
INSERT INTO Timings 
VALUEs
( 11, 'Finished Second  Query', CURRENT_DATETIME ());  
--
COMMIT WORK;
--
--  This is the relational version of the same thing.
BEGIN WORK;

COMMIT WORK;
--
--------------------------------------- Results ------------------------------
--
SELECT * FROM QueryResults;
--
SELECT * FROM Timings ORDER BY Time;
--
---------------------------  DROPS ---------------------------------
--
DROP TABLE Foo;
DROP TABLE Bar;
DROP ROW TYPE Foo_Type restrict;
DROP FUNCTION CURRENT_DATETIME();
DROP TABLE Timings;
DROP TABLE Numbers;
DROP TABLE Ints;
DROP ROW TYPE Number_Type RESTRICT;
DROP TABLE Reservations;
DROP TABLE Conferences;
DROP TABLE QueryResults;
--
-------------------------------------------------------------------
