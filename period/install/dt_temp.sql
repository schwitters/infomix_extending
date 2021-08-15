DROP TABLE Test_D_Period;
--
CREATE TABLE Test_D_Period
(	Id	SERIAL	PRIMARY KEY,
	FK	INTEGER	NOT NULL,
	When	Period	NOT NULL
);
--
DROP TABLE Test_D_Period;
--
CREATE TABLE Test_D_Period
(       Id      SERIAL  PRIMARY KEY,
        FK      INTEGER NOT NULL,
        When    Period  NOT NULL
);
--
INSERT INTO Test_D_Period
( FK, When )
SELECT Yr.Y + N1.Int + Dy.D,
       Period ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY )
  FROM
       TABLE(SET{1998, 1999, 2000, 2001}) Yr ( Y ),
       TABLE(SET{1,2,3,4,5,6,7,8,9,10,11,12}) Mnth (M),
       TABLE(SET{1,5,10,15,20,25}) Dy ( D ),
       TABLE(SET{0,3,5,7,9,10,11,12,13,15}) N1 ( Int ),
       TABLE(SET{1,2,4,6,8,10}) N2 ( Int );
--
SELECT COUNT(*) FROM Test_D_Period;
--
SET EXPLAIN ON;
--
--  Equal 
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"';
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/1999" to "10/22/1999"';
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"';
--
--  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
--                  just to check it.)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE OverlapNotTouches(T.When, '"12/20/2000" to "12/22/2001"');
--
--  Within ( Note: Check this one!)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE WithinNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"12/20/2000" to "12/22/2001"');
--
-- Contains
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE ContainsNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"12/20/2000" to "12/22/2001"');
--
--
UPDATE STATISTICS HIGH;
--
--  Equal 
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"';
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/1999" to "10/22/1999"';
--
SELECT * 
  FROM Test_D_Period T 
 WHERE T.When = '"10/20/2000" to "10/22/2000"';
--
--  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
--                  just to check it.)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Overlap(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE OverlapNotTouches(T.When, '"12/20/2000" to "12/22/2001"');
--
--  Within ( Note: Check this one!)
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE WithinNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Within(T.When, '"12/20/2000" to "12/22/2001"');
--
-- Contains
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"10/20/2000" to "10/22/2000"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE ContainsNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
--
SELECT * 
  FROM Test_D_Period T 
 WHERE Contains(T.When, '"12/20/2000" to "12/22/2001"');
--
-----------------------------------------------------------------------------
--
