--
--   File:    dt_stat_setup.sql
--
--
DROP TABLE Test_DT_Period;
-- DROP FUNCTION DT ( INTEGER, INTEGER, INTEGER, INTEGER, INTEGER,
--                    INTEGER, INTEGER, INTEGER, INTEGER, INTEGER );
--
CREATE TABLE Test_DT_Period
(       Id      SERIAL     PRIMARY KEY,
        FK      INTEGER    NOT NULL,
        When    DT_Period  NOT NULL
);
--
--   This useful UDF creates a new instance of the DT_Period type that
--  extends over a range starting at the DATETIME YEAR TO SECOND 
--  defined by the first six arguments for the INTERVAL DAY TO
--  SECOND defined by the last four arguements. Thus:
--
--   DT ( 1999, 10, 10, 10, 10, 10, 3, 1, 33, 33 ) |->
--
--     '"1999-10-10 10:10:10" to "1999-10-13 11:43:43"'
--
--   DT() is used extensively throughout these tests. 
--
CREATE FUNCTION DT ( ArgYear INTEGER, ArgMonth INTEGER, ArgDay INTEGER,
                     ArgHour INTEGER, ArgMinute INTEGER, ArgSecond INTEGER,
                     ArgIntDays INTEGER, ArgIntHrs INTEGER, ArgIntMins INTEGER,
                     ArgIntSecs INTEGER )
RETURNS DT_Period

        DEFINE DTStart   DATETIME YEAR TO SECOND;
        DEFINE DTFinish  DATETIME YEAR TO SECOND;
        DEFINE InDiff    INTERVAL DAY(4) TO SECOND;

        LET DTStart = (ArgYear || '-' || ArgMonth || '-' || ArgDay || ' ' ||
                       ArgHour || ':' || ArgMinute || ':' || ArgSecond
                      )::DATETIME YEAR TO SECOND;

        LET InDiff  = (ArgIntDays || ' ' || ArgIntHrs || ':' || ArgIntMins ||
                       ':' || ArgIntSecs
                      )::INTERVAL DAY(4) TO SECOND;

        LET DTFinish = DTStart + InDiff;

        RETURN DT_Period ( DTStart, DTFinish );

END FUNCTION;
--
--   Test it, just this once.
--
EXECUTE FUNCTION DT ( 1999, 12, 10, 10, 10, 10, 5, 2, 10, 15 );
--
--   OK.
--
--   To make any sense of this, I need to get a large row set to work with.
-- 
--   The following pair of queries create a reasonably large row set over
--  a four year interval of time. This is broken into two seperate queries
--  because of limitations on the size of log records the engine supports.
--
INSERT INTO Test_DT_Period
( FK, When )
SELECT N1.Int * 10 + N1.Int,
       DT ( Yr.Y, Mnth.M, (Dy.D + N1.Int), Hr.H, 10, 10,
            N2.Int, MOD((N2.Int*111),24), 11, 12 )
  FROM
       TABLE(SET{2000, 2001}) Yr ( Y ),
       TABLE(SET{1,2,3,4,5,6,7,8,9,10,11,12}) Mnth (M),
       TABLE(SET{1,15,20,23}) Dy ( D ),
       TABLE(SET{2,6,12,15}) Hr ( H ),
       TABLE(SET{0,1,2,4,5}) N1 ( Int ),
       TABLE(SET{1,2,4,6,8,10}) N2 ( Int );
--
INSERT INTO Test_DT_Period
( FK, When )
SELECT N1.Int * 10 + N1.Int,
       DT ( Yr.Y, Mnth.M, (Dy.D + N1.Int), Hr.H, 10, 10,
            N2.Int, MOD((N2.Int*111),24), 11, 12 )
  FROM
       TABLE(SET{1998, 1999}) Yr ( Y ),
       TABLE(SET{1,2,3,4,5,6,7,8,9,10,11,12}) Mnth (M),
       TABLE(SET{1,15,20,23}) Dy ( D ),
       TABLE(SET{2,6,12,15}) Hr ( H ),
       TABLE(SET{0,1,2,4,5}) N1 ( Int ),
       TABLE(SET{1,2,4,6,8,10}) N2 ( Int );
--
--  Add some EPOCH/FOREVER examples.
--
INSERT INTO Test_DT_Period
( FK, When )
SELECT N1.Int * 10 + N1.Int,
       Set_Start_Epoch( DT ( Yr.Y, Mnth.M, (Dy.D + N1.Int), Hr.H, 10, 10,
            N2.Int, MOD((N2.Int*111),24), 11, 12 ))
  FROM
       TABLE(SET{1998}) Yr ( Y ),
       TABLE(SET{1,2,3}) Mnth (M),
       TABLE(SET{1,15}) Dy ( D ),
       TABLE(SET{2,6,12,15}) Hr ( H ),
       TABLE(SET{0,1,2,4,5}) N1 ( Int ),
       TABLE(SET{1,10,30}) N2 ( Int );
--
INSERT INTO Test_DT_Period
( FK, When )
SELECT N1.Int * 10 + N1.Int,
       Set_Finish_Forever( DT ( Yr.Y, Mnth.M, (Dy.D + N1.Int), Hr.H, 10, 10,
            N2.Int, MOD((N2.Int*111),24), 11, 12 ))
  FROM
       TABLE(SET{2001}) Yr ( Y ),
       TABLE(SET{10,11,12}) Mnth (M),
       TABLE(SET{15,20}) Dy ( D ),
       TABLE(SET{1,5,13,16}) Hr ( H ),
       TABLE(SET{0,1,2,4,5}) N1 ( Int ),
       TABLE(SET{1,10,30}) N2 ( Int );
--
--  How many?
--
SELECT COUNT(*) FROM Test_DT_Period;
--
