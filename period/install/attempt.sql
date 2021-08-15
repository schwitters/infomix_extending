--
--  OK.
--
--   Now, insert a small(is) number of values, and then do a join
--  query to exercise the algorithms, and the R-Tree index.
--
DROP TABLE DT_Test_Samples;
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
       TABLE(SET{1999, 2000}) Yr ( Y ),
       TABLE(SET{1,5,11}) Mnth (M),
       TABLE(SET{1,23}) Dy ( D ),
       TABLE(SET{2,11}) Hr ( H ),
       TABLE(SET{1,10,100}) N2 ( Int );
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
