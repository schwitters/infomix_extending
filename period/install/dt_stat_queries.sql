--
--   File:   dt_stat_queries.sql
--
--  About:
--
--   In order to determine whether or not the selectivity estimates
--  are accurate, I've first go to generate a set of "base-line" 
--  results. These are plonked into a file in the server's directories
--  called $INFORMIXDIR/sqexplain/[username].out
--
--   To ensure that I actually get real results in this test set, first
--  run the following query which is (given the way that the data set was
--  defined above) guaranteed to return some result. The value handed back
--  by this query should be used throughout the rest of the script
--  whenever an exact match, or the basis for some other expression,
--  is desired.
--
--   Note: Someone screwed up. The DT() UDF, although it does not contain
--  any SQL queries or calls to VARIANT logic, doesn't count as a 
--  constant. Consequently, I can't use the DT() UDF to test the stats
--  and selectivity. It is useful to generate the strings, but as I 
--  don't want to depend on the usage of something like th Exec()
--  BladeLet as part of the testing process, all you can do is "cut-n-paste"
--  values.
--
SELECT MAX(T.When)
  FROM Test_DT_Period T
 WHERE Overlap ( T.When,
                 DT( 1999, 10, 10, 10, 10, 10, 10, 10, 10, 10 ));
--
SET EXPLAIN ON;
--
--  Equal: CASE #1: Exists.
--
SELECT *
  FROM Test_DT_Period T
 WHERE T.When = '"1999-10-20 15:10:10" to "1999-10-30 21:21:22"'
 ORDER BY Id;
--
--  Equal: CASE #2: Doesn't exist. Should return no rows.
--
SELECT *
  FROM Test_DT_Period T
 WHERE T.When = '"1999-10-20 15:10:11" to "1999-10-30 21:21:24"'
 ORDER BY Id;
--
--  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
--                  just to check it.)
--
--   Case # 1:  Small, 1 Day
--
EXECUTE FUNCTION DT ( 1999, 10, 10, 10, 10, 10, 1, 0, 0, 0 ); 
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Overlap( T.When, '"1999-10-10 10:10:10" to "1999-10-11 10:10:10"' )
 ORDER BY Id;
--
--  Case # 2:   Medium, 10 days 
--
EXECUTE FUNCTION DT ( 1999, 10, 10, 10, 10, 10, 10, 0, 0, 0 ); 
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Overlap( T.When, '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
 ORDER BY Id;
--
--  Case # 3:   Large, 100 days 
--
EXECUTE FUNCTION DT ( 1999, 10, 10, 10, 10, 10, 100, 0, 0, 0 ); 
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Overlap( T.When, '"1999-10-10 10:10:10" to "2000-01-18 10:10:10"' )
 ORDER BY Id;
--
--  Within ( Note: Check this one!)
--
--   Case # 1:  Small, 1 Day
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Within( T.When, '"1999-10-10 10:10:10" to "1999-10-11 10:10:10"' )
 ORDER BY Id;
--
--   Case # 2:  Medium, 10 Days
--
-- SELECT * 
--   FROM Test_DT_Period T 
--  WHERE WithinNotTouches( T.When, 
--                          '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
--  ORDER BY Id;
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Within( T.When, 
               '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
 ORDER BY Id;
--
--   Case # 3:  Large, 100 Days
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Within( T.When, '"1999-10-10 10:10:10" to "2000-01-18 10:10:10"' )
 ORDER BY Id;
--
-- Contains
--
--   Case # 1:  Small, 1 Day
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Contains( T.When, '"1999-10-10 10:10:10" to "1999-10-11 10:10:10"' )
 ORDER BY Id;
--
--   Case # 2:  Medium, 10 Days
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE ContainsNotTouches( T.When, 
                           '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
 ORDER BY Id;
--
--   Case # 3:  Large, 100 Days
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE Contains( T.When, '"1999-10-10 10:10:10" to "2000-01-18 10:10:10"' )
 ORDER BY Id;
--
-- BeforeTouches
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE BeforeTouches( T.When, '"1999-10-10 10:10:10" to "1999-10-11 10:10:10"' )
 ORDER BY Id;
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE BeforeTouches( T.When, 
                      '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
 ORDER BY Id;
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE BeforeTouches( T.When, '"1999-10-10 10:10:10" to "2000-01-18 10:10:10"' )
 ORDER BY Id;
--
-- AfterTouches
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE AfterTouches( T.When, '"1999-10-10 10:10:10" to "1999-10-11 10:10:10"' )
 ORDER BY Id;
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE AfterTouches( T.When, 
                      '"1999-10-10 10:10:10" to "1999-10-20 10:10:10"')
 ORDER BY Id;
--
SELECT * 
  FROM Test_DT_Period T 
 WHERE AfterTouches( T.When, '"1999-10-10 10:10:10" to "2000-01-18 10:10:10"' )
 ORDER BY Id;
--
------------------------------------------------------------------------------
--
--  OK.
--
--   Now, insert a small(is) number of values, and then do a join
------------------------------------------------------------------------------
--
--  OK.
--
------------------------------------------------------------------------------
--
--  OK.
--
--   Now, insert a small(is) number of values, and then do a join
------------------------------------------------------------------------------
--
--  OK.
--
--   Now, insert a small(is) number of values, and then do a join
--  query to exercise the algorithms, and the R-Tree index.
--
CREATE TABLE DT_Test_Samples (
	Id	SERIAL	PRIMARY KEY,
	Val	DT_Period	NOT NULL
);
--
INSERT INTO DT_Test_Samples
( Val )
SELECT 
       DT ( Yr.Y, Mnth.M, Dy.D, Hr.H, 10, 10,
            N2.Int, MOD((N2.Int*111),24), 11, 12 )
  FROM
       TABLE(SET{1999, 2001}) Yr ( Y ),
       TABLE(SET{1,5,11}) Mnth (M),
       TABLE(SET{1,23}) Dy ( D ),
       TABLE(SET{2,11}) Hr ( H ),
       TABLE(SET{1,10,100}) N2 ( Int );
--
--  Add some data with EPOCH and FOREVER
--
INSERT INTO DT_Test_Samples
( Val )
SELECT 
       Set_Start_Epoch ( DT ( Yr.Y, Mnth.M, Dy.D, Hr.H, 10, 10,
            		      N2.Int, MOD((N2.Int*111),24), 11, 12 ))
  FROM
       TABLE(SET{1999}) Yr ( Y ),
       TABLE(SET{1,2}) Mnth (M),
       TABLE(SET{1,10}) Dy ( D ),
       TABLE(SET{11}) Hr ( H ),
       TABLE(SET{1,10,20}) N2 ( Int );
--
INSERT INTO DT_Test_Samples
( Val )
SELECT 
       Set_Finish_Forever ( DT ( Yr.Y, Mnth.M, Dy.D, Hr.H, 10, 10,
            		         N2.Int, MOD((N2.Int*111),24), 11, 12 ))
  FROM
       TABLE(SET{2001}) Yr ( Y ),
       TABLE(SET{11,12}) Mnth (M),
       TABLE(SET{20,25}) Dy ( D ),
       TABLE(SET{11}) Hr ( H ),
       TABLE(SET{1,10,20}) N2 ( Int );
--
INSERT INTO DT_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
INSERT INTO DT_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
INSERT INTO DT_Test_Samples ( Val ) VALUES ( '"EPOCH" to "FOREVER"');
--
--  For each of the UDFs, do a join query for each of the values
--
UPDATE STATISTICS HIGH FOR TABLE DT_Test_Samples;
SET EXPLAIN ON;
--
--  Case # 1: Overlap
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE Overlap ( S.Val, T.When )
 GROUP BY S.Val;
--
--  Case # 2: Equal
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE S.Val = T.When
 GROUP BY S.Val;
--
--  Case # 3: Contains
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE Contains( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 4: Within
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE Within( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 5: ContainsNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE ContainsNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 6: WithinNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE WithinNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 7: OverlapNotTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE OverlapNotTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 8: BeforeTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE BeforeTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--  Case # 9: AfterTouches
--
SELECT S.Val, COUNT(*)
  FROM Test_DT_Period T,
       DT_Test_Samples S
 WHERE AfterTouches( S.Val , T.When )
 GROUP BY S.Val;
--
--   Cleanup.
--
DROP TABLE DT_Test_Samples;
