--
--   File:  d_stat_setup.sql
--
--  About:
--
--     This script creates a table and populates it with a large amount of
--  data values to be used by later queries which test all of the various
--  UDFs being introduced by this BladeLet.
--
--     The setup is very similar to the dt_stat_setup.sql: single large
-- table with a column
---------------------------- STATISTICS CHECKS --------------------------------
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
--  Add some open ends.
--
INSERT INTO Test_D_Period
( FK, When )
SELECT Yr.Y + N1.Int + Dy.D,
       Set_Start_Epoch ( Period 
             ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY ))
  FROM
       TABLE(SET{1998}) Yr ( Y ),
       TABLE(SET{1,2,3}) Mnth (M),
       TABLE(SET{1,10,20}) Dy ( D ),
       TABLE(SET{0,10,20}) N1 ( Int ),
       TABLE(SET{2, 20, 40 }) N2 ( Int );
--
INSERT INTO Test_D_Period
( FK, When )
SELECT Yr.Y + N1.Int + Dy.D,
       Set_Finish_Forever ( Period 
             ((Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE + N1.Int UNITS DAY,
               (Mnth.M || '/' || Dy.D || '/' || Yr.Y)::DATE +
               ( N1.Int + N2.Int ) UNITS DAY ))
  FROM
       TABLE(SET{2001}) Yr ( Y ),
       TABLE(SET{10,11,12}) Mnth (M),
       TABLE(SET{10,20}) Dy ( D ),
       TABLE(SET{0,10,20}) N1 ( Int ),
       TABLE(SET{2, 20, 40 }) N2 ( Int );
--
SELECT COUNT(*) FROM Test_D_Period;
