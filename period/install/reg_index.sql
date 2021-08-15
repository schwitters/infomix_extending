--
--   File:  reg_index.sql
--
--  About:
--
--   This is an attempt to test the indexing routines.
--
DROP TABLE Index_Test;
DROP FUNCTION CURRENT_DATETIME();
DROP TABLE Timings;
DROP TABLE Test_Results;
--
DROP FUNCTION Make_INTERVAL ( INTEGER, INTEGER, INTEGER, INTEGER );
--
CREATE FUNCTION Make_INTERVAL ( ARG_DAYS INTEGER, ARG_HOUR INTEGER,
                                ARG_MINUTE INTEGER, ARG_SECOND INTEGER )
RETURNING INTERVAL DAY(6) TO SECOND
        RETURN (ARG_DAYS || ' ' ||
                ARG_HOUR || ':' || ARG_MINUTE || ':' || ARG_SECOND
               )::INTERVAL DAY(6) TO SECOND;
END FUNCTION;
--
CREATE TABLE Index_Test (
	Id	VARCHAR(32)	NOT NULL,
	When	DT_Period	NOT NULL,
	Padding_1 VARCHAR(255)	NOT NULL,
	Padding_2 VARCHAR(255)	NOT NULL,
	Start	DATETIME YEAR TO SECOND,
	Finish	DATETIME YEAR TO SECOND
);
--
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) + Make_Interval (N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) + Make_Interval(N1.Num+N2.Num-30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)+Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)+Make_INTERVAL(N1.Num + N2.Num - 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{50,100,150,200,250,300,350,400,450,500,550}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) - Make_Interval( N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) - Make_Interval( N1.Num+N2.Num+30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)-Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)-Make_INTERVAL(N1.Num + N2.Num + 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{50,100,150,200,250,300,350,400,450,500,550}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) + Make_Interval(N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) + Make_Interval(N1.Num+N2.Num - 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)+Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)+Make_INTERVAL(N1.Num + N2.Num - 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{25,75,125,175,225,275,325,375,425,475,525}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) - Make_Interval (N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) - Make_Interval(N1.Num + N2.Num + 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)-Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)-Make_INTERVAL(N1.Num + N2.Num + 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{25,75,125,175,225,275,325,375,425,475,525}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) + Make_Interval (N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) + Make_Interval(N1.Num + N2.Num - 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)+Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)+Make_INTERVAL(N1.Num + N2.Num -30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{550,600,650,700,750,800,850,900,950,1000,1050}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) - Make_Interval (N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) - Make_Interval(N1.Num + N2.Num + 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)-Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)-Make_INTERVAL(N1.Num + N2.Num + 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{550,600,650,700,750,800,850,900,950,1000,1050}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) + Make_Interval( N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) + Make_Interval(N1.Num+ N2.Num - 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)+Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)+Make_INTERVAL(N1.Num + N2.Num - 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{525,575,625,675,725,775,825,875,925,975,1025}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
INSERT INTO Index_Test
( Id, When, Padding_1, Padding_2, Start, Finish )
SELECT T.Id || '-A' 
          AS Id,
       DT_Period
        ( Start( T.When) - Make_Interval( N1.Num + N2.Num,N3.Num,13,N2.Num),
          Finish( T.When) - Make_Interval(N1.Num+ N2.Num + 30,N3.Num,13,N2.Num)
        ) 
        AS When,
       'Padding_1 Only                       Only Padding_1' AS Padding_1,
       'Padding_1 Only                       Only Padding_2' AS Padding_2,
       Start(T.When)-Make_INTERVAL(N1.Num + N2.Num, N3.Num, 13, N2.Num) 
          AS Start,
       Finish(T.When)-Make_INTERVAL(N1.Num + N2.Num + 30, N3.Num, 13, N2.Num) 
          AS Finish
  FROM Test_DT_Period T,
       TABLE(SET{525,575,625,675,725,775,825,875,925,975,1025}) N1 ( Num ),
       TABLE(SET{2,5,7,11,13,17,23,29,31,37,41,47}) N2 ( Num ),
       TABLE(SET{2,4,6,8,10,12}) N3 ( Num )
 WHERE T.Id IN ( 'A', 'B', 'C', 'D', 'F', 'G');
--
--
--  Check for consistency
--
SET EXPLAIN ON;
--
SELECT *
  FROM Index_Test T
 WHERE T.Start <> Start(T.When) OR
       T.Finish <> Finish(T.When);
--
SELECT SUM(Period_Interval( T.When ) ),
       MIN(Start( T.When )), 
       MAX(Finish( T.When ) ),
       MAX(Finish( T.When )) - MIN(Start(T.When)),
       MAX(Finish(T.When) - Start(T.When)),
       AVG(Period_Interval( T.When ) ),
       COUNT(*)
  FROM Index_Test T;
--
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
CREATE TABLE Test_Results (
	Step	INTEGER	NOT NULL,
	Desc	VARCHAR(64)	NOT NULL,
	Result	INTEGER		NOT NULL
);
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 1, 'Start Single Scan For Count', CURRENT_DATETIME ());
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 1, "Count",
       COUNT(*)
  FROM Index_Test;
--
--
INSERT INTO Timings 
VALUEs
( 1, 'Finish Single Scan For Count', CURRENT_DATETIME ());
--
COMMIT WORK;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 2, 'Start No Index Overlaps', CURRENT_DATETIME ());
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 2, "Overlap no Index 1",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "1998-10-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "1998-10-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 3, "Overlap no Index 2",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "1998-12-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "1998-12-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 4, "Overlap no Index 3",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "2000-12-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "2000-12-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Timings 
VALUEs
( 2, 'End No Index Overlaps', CURRENT_DATETIME ());
--
COMMIT WORK;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 3, 'Start No Index Logical Version', CURRENT_DATETIME ());
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 5, "Logic Version No Index 1",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "1998-10-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "1998-10-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 6, "Logic Version No Index 2",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "1998-12-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "1998-12-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 7, "Logic Version No Index 3",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "2000-12-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "2000-12-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Timings 
VALUEs
( 3, 'End No Index Logical Version', CURRENT_DATETIME ());
--
COMMIT WORK;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 4, 'Start Building Index', CURRENT_DATETIME ());
--
CREATE INDEX Index_Index_1 ON Index_Test ( When DT_Period_ops) USING RTREE;
--
INSERT INTO Timings 
VALUEs
( 4, 'End Building Index', CURRENT_DATETIME ());
--
COMMIT WORK;
--
UPDATE STATISTICS HIGH;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 5, 'Start Index Overlaps', CURRENT_DATETIME ());
--
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 8, "Overlap Index 1",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "1998-10-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "1998-10-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 9, "Overlap Index 2",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "1998-12-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "1998-12-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 10, "Overlap Index 3",
       COUNT(*)
  FROM Index_Test T
 WHERE Overlap( T.When,
                DT_Period (
		     "2000-12-10 12:10:11"::DATETIME YEAR TO SECOND,
		     "2000-12-11 12:10:11"::DATETIME YEAR TO SECOND
                    )
       );
--
INSERT INTO Timings 
VALUEs
( 5, 'End Index Overlaps', CURRENT_DATETIME ());
--
COMMIT WORK;
--
BEGIN WORK;
--
INSERT INTO Timings 
VALUEs
( 6, 'Start Index Logical Version', CURRENT_DATETIME ());
--
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 11, "Logic Version Index 1",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "1998-10-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "1998-10-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 12, "Logic Version Index 2",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "1998-12-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "1998-12-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Test_Results
( Step, Desc, Result )
SELECT 13, "Logic Version Index 3",
       COUNT(*)
  FROM Index_Test T
 WHERE T.Finish > "2000-12-10 12:10:11"::DATETIME YEAR TO SECOND
   AND T.Start < "2000-12-11 12:10:11"::DATETIME YEAR TO SECOND;
--
INSERT INTO Timings 
VALUEs
( 6, 'End Index Logical Version', CURRENT_DATETIME ());
--
COMMIT WORK;
--
SELECT * FROM systables WHERE tabname = 'index_test';
--
 SELECT T.tabname, D.*
   FROM sysdistrib D, systables T
  WHERE T.tabname = 'index_test'
    AND T.tabid   = D.tabid
  ORDER BY D.colno, D.seqno;
--
SELECT * FROM Test_Results;
--
SELECT T1.Step, MIN(T1.Name),
       MAX(T2.Time - T1.Time)
  FROM Timings T1, Timings T2
 WHERE T1.Step = T2.Step
   AND T2.Time > T1.Time
 GROUP BY T1.Step
 ORDER BY T1.Step;
--
--  NOTE: Check the SQLEXPLAIN output to ensure that the index
--       was used.    
--
---------------------------  DROPS ---------------------------------
