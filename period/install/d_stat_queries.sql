--
--   File:  d_stat_queries.sql
--
--  About:
-- 
--     This set of queries is used in several places throughout the
--   testing scripts. In addition to ensuring that different parts of
--   the Bladelet works, these are also used to test the statistics
--   and selectivity estimation code for both correctness and accuracy.
--
--     In all cases, a copy of the DBMS query plan will be placed in the
--   %INFORMIXDIR%\sqexpln\[username].out file. This information, 
--   particularly details of the row count estimates, should be compared
--   with the actual result values.
--
--     In every case, the result of these queries ought to be identical.
--   Any different indicates a bug. Note that this is why there is an
--   ORDER BY clause on every query.
--
SET EXPLAIN ON;
--
--  Equal 
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"'
  ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/1999" to "10/22/1999"'
  ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"'
 ORDER BY Id;
--
--  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
--                  just to check it.)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/2000" to "10/22/2000"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/1999" to "12/22/1999"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE OverlapNotTouches(T.When, '"12/20/2000" to "12/22/2001"')
ORDER BY Id;
--
--  Within ( Note: Check this one!)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"10/20/2000" to "10/22/2000"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE WithinNotTouches(T.When, '"10/20/1999" to "12/22/1999"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"12/20/2000" to "12/22/2001"')
ORDER BY Id;
--
-- Contains
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"10/20/2000" to "10/22/2000"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE ContainsNotTouches(T.When, '"10/20/1999" to "12/22/1999"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"12/20/2000" to "12/22/2001"')
ORDER BY Id;
--
--  BeforeTouches
--
SELECT * 
  FROM Test_D_Period T 
 WHERE BeforeTouches(T.When, '"10/20/1998" to "10/22/1998"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE BeforeTouches(T.When, '"10/20/1999" to "12/22/1999"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE BeforeTouches(T.When, '"12/20/2000" to "12/22/2001"')
ORDER BY Id;
--
--  AfterTouches
--
SELECT * 
  FROM Test_D_Period T 
 WHERE AfterTouches(T.When, '"10/20/2001" to "10/22/2001"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE AfterTouches(T.When, '"10/20/2000" to "12/22/2000"')
ORDER BY Id;
--
SELECT * 
  FROM Test_D_Period T 
 WHERE AfterTouches(T.When, '"12/20/2000" to "12/22/2001"')
ORDER BY Id;
--
--
-----------------------------------------------------------------------------
--
--  OK.
--
--   Now, insert a small(is) number of values, and then do a join
--  query to exercise the algorithms, and the R-Tree index.
--
CREATE TABLE D_Test_Samples (
	Id	SERIAL	PRIMARY KEY,
	Val	Period	NOT NULL
);
--
INSERT INTO D_Test_Samples
( Val )
SELECT 
       Period ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY )
  FROM
       TABLE(SET{1998, 2001}) Yr ( Y ),
       TABLE(SET{1,5,10,12}) Mnth (M),
       TABLE(SET{2,11,21}) Dy ( D ),
       TABLE(SET{0,5}) N1 ( Int ),
       TABLE(SET{1,10,100,300}) N2 ( Int );
--
INSERT INTO D_Test_Samples
( Val )
SELECT 
       Set_Start_Epoch ( Period ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + 
                                  N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY ))
  FROM
       TABLE(SET{1998}) Yr ( Y ),
       TABLE(SET{1,3}) Mnth (M),
       TABLE(SET{2,11}) Dy ( D ),
       TABLE(SET{0,5}) N1 ( Int ),
       TABLE(SET{1,10}) N2 ( Int );
--
INSERT INTO D_Test_Samples
( Val )
SELECT 
       Set_Finish_Forever ( Period ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + 
                            N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY ))
  FROM
       TABLE(SET{2001}) Yr ( Y ),
       TABLE(SET{11,12}) Mnth (M),
       TABLE(SET{20,25}) Dy ( D ),
       TABLE(SET{0,5}) N1 ( Int ),
       TABLE(SET{1,10}) N2 ( Int );
--
INSERT INTO D_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
INSERT INTO D_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
INSERT INTO D_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
--
--  For each of the UDFs, do a join query for each of the values
--
UPDATE STATISTICS HIGH FOR TABLE D_Test_Samples;
SET EXPLAIN ON;
--
--  Case # 1: Overlap
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE Overlap ( S.Val, T.When )
 GROUP BY S.Val;
--
--  Case # 2: Equal
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE S.Val = T.When
 GROUP BY S.Val;
--
--  Case # 3: Contains
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE Contains( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 4: Within
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE Within( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 5: ContainsNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE ContainsNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 6: WithinNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE WithinNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 7: OverlapNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE OverlapNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
-- Case # 8: BeforeTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE BeforeTouches( S.Val , T.When )
 GROUP BY S.Val;
--
-- Case # 9: AfterTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_D_Period T,
       D_Test_Samples S
 WHERE AfterTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--   Clean-up for next time
--
DROP TABLE D_Test_Samples;
