--
--  File:  dt_regression_min.sql
--
-- About:
--
--     This is being set up to do regression testing on the body of the 
--    extensions.
--
-------------------------------- CHECK LOAD ---------------------------------
--
EXECUTE FUNCTION DT_Period ( "2000-10-10 12:10:10", "2000-10-11 12:10:10");
--
---------------------------------- HYGIENE ----------------------------------
--
-- DROP FUNCTION Make_DATETIME ( INTEGER, INTEGER, INTEGER, INTEGER, 
--                               INTEGER, INTEGER );
-- DROP FUNCTION Make_INTERVAL ( INTEGER, INTEGER, INTEGER, INTEGER );
--
-- DROP TABLE Test_DT_Period;
--
--  This is a temporary table. On occasions, when the script does not
-- complete successfully, you need to drop this in order that the
-- script runs without error.
--
-- DROP TABLE Foo;
--
----------------------- USEFUL FUNCTIONS FOR TESTING ------------------------
--
CREATE FUNCTION Make_DATETIME( ARG_YEAR INTEGER, ARG_MONTH INTEGER, 
                               ARG_DAY INTEGER, ARG_HOUR INTEGER, 
                               ARG_MINUTE INTEGER, ARG_SECOND INTEGER )
RETURNING DATETIME YEAR TO SECOND
	RETURN ARG_YEAR || '-' || ARG_MONTH || '-' || ARG_DAY || ' ' || 
               ARG_HOUR || ':' || ARG_MINUTE || ':' || ARG_SECOND;
END FUNCTION;
--
CREATE FUNCTION Make_INTERVAL ( ARG_DAYS INTEGER, ARG_HOUR INTEGER, 
                                ARG_MINUTE INTEGER, ARG_SECOND INTEGER )
RETURNING INTERVAL DAY(6) TO SECOND
	RETURN (ARG_DAYS || ' ' || 
                ARG_HOUR || ':' || ARG_MINUTE || ':' || ARG_SECOND
               )::INTERVAL DAY(6) TO SECOND;
END FUNCTION;
--
EXECUTE FUNCTION Make_INTERVAL( 10, 10, 10, 10);
EXECUTE FUNCTION Make_INTERVAL( 140, 10, 10, 10);
--
EXECUTE FUNCTION Make_DATETIME(2000, 10, 10, 12, 10, 10);
--
------------------------------ SMALL TESTS -----------------------------------
--
EXECUTE FUNCTION DT_PeriodIn('"1999-10-10 12:10:10" to "1999-12-20 22:20:20"');
EXECUTE FUNCTION DT_PeriodIn('"2000-02-09 08:30:30" to "2000-03-20 08:30:30"');
EXECUTE FUNCTION DT_PeriodIn('"2000-03-20 08:30:30" to "2000-08-07 18:40:40"');
EXECUTE FUNCTION DT_PeriodIn('"2000-04-19 18:40:40" to "2000-06-08 18:40:40"');
--
EXECUTE FUNCTION DT_PeriodIn('"EPOCH" to "2000-06-08 18:40:40"');
EXECUTE FUNCTION DT_PeriodIn('"2000-04-19 18:40:40" to "FOREVER"');
EXECUTE FUNCTION DT_PeriodIn('"EPOCH" to "FOREVER"');
--
EXECUTE FUNCTION Set_Start_Epoch ( 
                  DT_PeriodIn('"1999-10-10 12:10:10" to "1999-12-20 22:20:20"')
                 );
EXECUTE FUNCTION Set_Finish_Forever (
                  DT_PeriodIn('"2000-02-09 08:30:30" to "2000-03-20 08:30:30"')
                 );
EXECUTE FUNCTION Set_Start_Epoch (
                  Set_Finish_Forever (
		   DT_PeriodIn('"2000-03-20 08:30:30" to "2000-08-07 18:40:40"')
                 ));
--
EXECUTE FUNCTION DT_PeriodIn('"EPOCH" to "2000-06-08 18:40:40"');
EXECUTE FUNCTION DT_PeriodIn('"2000-04-19 18:40:40" to "FOREVER"');
EXECUTE FUNCTION DT_PeriodIn('"EPOCH" to "FOREVER"');
--
--
--  Test a minimal set of the functionality. The idea is to create a set
-- of DT_Period in a table, and then do some queries.
--
--     A    |----|
--     B             |---|
--     C                 |------------|
--     D                     |----|
--     E                              |----|
--     F                                   |----|
--     G                                            |----|
--     H                            |---|
--
--   This is the second batch of test data, ensuring that the
--  boundary cases (Epoch and Forever) are dealth with.
--
--        A    |----|
--       EH -|===================================================  HU
--       EI ---|=================================================  IU
--       EJ -----|===============================================  JU
--       EK --------|============================================  KU
--       EL ----------|==========================================  LU
--
--        B             |---|
--       EM  -----------|========================================  MU
--       EN  -------------|======================================  NU
--
--        C                 |------------|
--       EO  ---------------|====================================  OU
--       EP  -----------------|==================================  PU
--       EU  ----------------------------|=======================  UU
--
--        D                     |----|
--       EQ  -------------------|================================  QU
--       ER  ---------------------|==============================  RU
--       ES  ------------------------|===========================  SU
--
--        E                              |----|
--       ET  --------------------------|=========================  TU
--       EV  -------------------------------|====================  VU
--       EW  ---------------------------------|==================  WU
--
--        F                                   |----|
--       EX  --------------------------------------|=============  XU
--
--        G                                            |----|
--       EY  ----------------------------------------|===========  YU
--       EZ  ------------------------------------------|=========  ZU
--      EZA  ---------------------------------------------|======  ZAU
--      EZB  -------------------------------------------------|==  ZBU
--      E=U  ====================================================
-- 
--
CREATE TABLE Test_DT_Period (
	Id	VARCHAR(32)	NOT NULL,
	When	DT_Period	NOT NULL,
        Picture VARCHAR(70)     NOT NULL
);
--
INSERT INTO Test_DT_Period 
VALUES ( 'A', DT_Period ("1999-10-10 12:10:10"::DATETIME YEAR TO SECOND,
                         "1999-10-21 12:10:10"::DATETIME YEAR TO SECOND + 
                             Make_INTERVAL(60,10,10,10)),
       '  A    |----|');
--
SELECT 'B' AS Id,
       DT_Period ( Finish(T.When) + Make_INTERVAL(  50,10,10,10 ),
                   Finish(T.When) + Make_INTERVAL(  90,10,10,10 )
                 ) AS When,
       '  B             |---|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'C' AS Id,
       DT_Period ( Finish(T.When), 
                   Finish(T.When) + Make_INTERVAL(140,10,10,10)
                 ) AS When,
       '  C                 |------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'B'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'D' AS Id,
       DT_Period ( Start(T.When) + Make_INTERVAL(30,10,10,10),
                   Start(T.When) + Make_INTERVAL(80,10,10,10)
                 ) AS When,
       '  D                     |----|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'E' AS Id,
       DT_Period ( Finish(T.When),
                   Finish(T.When) + Make_INTERVAL(60,10,10,10)
                 ) AS When,
       '  E                              |----|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'F' AS Id,
       DT_Period ( Finish(T.When),
                   Finish(T.When) + Make_INTERVAL(50,10,10,10)
                 ) AS When,
       '  F                                   |----|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'G' AS Id,
       DT_Period ( Finish(T.When) + Make_INTERVAL(50,10,10,10),
                   Finish(T.When) + Make_INTERVAL(100,10,10,10)
                 ) AS When,
       '  G                                            |----|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'F'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'H' AS Id,
       DT_Period ( Finish(T.When) - Make_INTERVAL(10,10,10,10),
                   Finish(T.When) + Make_INTERVAL(40,10,10,10)
                 ) AS When,
       '  H                            |---|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       '             Equal (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Equal( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Equal,

       '          NotEqual (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           NotEqual ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS NotEqual,

       'ContainsNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           ContainsNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS ContainsNotTouches,

       '          Contains (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Contains ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Contains,

       '  WithinNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           WithinNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS WithinNotTouches,

       '            Within (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Within ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Within,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Overlap,

       '            Before (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Before ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Before,

       '     BeforeTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           BeforeTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS BeforeTouches,

       '             After (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           After ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS After,

       '      AfterTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           AfterTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS AfterTouches,

       '----------------------------------------------------------' AS Spacer_2,
       'GetIntersect (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetIntersect ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Intersect,
       'GetUnion (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetUnion ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Union,
       'Start( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Start( GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Start_Union,
       'Finish( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Finish(GetIntersect ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Finish_Intersect,
       'Length( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union,
       'Period_Interval( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Period_Interval(GetIntersect (T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Intersect_Interval
  FROM Test_DT_Period T1, Test_DT_Period T2;
--
EXECUTE FUNCTION DT_Period ( "2000-10-10 12:10:10", "2000-10-11 12:10:10");
--
--  OK. Now for the second lot.
--
CREATE TEMP TABLE Foo (
	Id	VARCHAR(32)	NOT NULL,
	When	DT_Period	NOT NULL,
        Picture VARCHAR(70)     NOT NULL
);
--
--        A    |----|
--       EH -|===================================================  HU
--       EI ---|=================================================  IU
--       EJ -----|===============================================  JU
--       EK --------|============================================  KU
--       EL ----------|==========================================  LU
--
--  EH/HU
--
INSERT INTO Foo
SELECT 'EH' AS Id,
       Set_Start_Epoch( 
        DT_Period ( Start(T.When) - Make_INTERVAL( 20,10,10,10 ),
                    Start(T.When) - Make_INTERVAL( 10,10,10,10 )
                  )
       ) AS When,
       ' EH -|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'HU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Start(T.When) - Make_INTERVAL( 10,10,10,10 ),
                    Start(T.When)
                  )
       ) AS When,
       '     |===================================================  HU' 
            AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
--  EI/IU
--
INSERT INTO Foo
SELECT 'EI' AS Id,
       Set_Start_Epoch( 
        DT_Period ( Start(T.When) - Make_INTERVAL( 10,10,10,10 ),
                    Start(T.When)
                  )
       ) AS When,
       ' EI ---|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'IU' AS Id,
       Set_Finish_Forever( 
         DT_Period(
                    Start(T.When), 
                    Start(T.When) + Make_INTERVAL(10,10,10,10)
         )
       ) AS When,
       '       |================================================== IU' 
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
--  EJ/JU
--
INSERT INTO Foo
SELECT 'EJ' AS Id,
       Set_Start_Epoch( 
        DT_Period ( Start(T.When) - Make_INTERVAL( 20,10,10,10 ),
                    Start(T.When) + Make_INTERVAL( 10,10,10,10 )
                  )
       ) AS When,
       ' EJ -----|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'JU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Start(T.When) + Make_INTERVAL( 10,10,10,10 ),
                    Start(T.When) + Make_INTERVAL( 20,10,10,10 )
                  )
       ) AS When,
       '         |================================================ JU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
--  EK/KU
--
INSERT INTO Foo
SELECT 'EK' AS Id,
       Set_Start_Epoch( T.When )
        AS When,
       ' EK --------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'KU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Finish(T.When),
                    Finish(T.When) + Make_INTERVAL( 10,10,10,10 )
                  )
       ) AS When,
       '            |============================================= KU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
--  EL/LU
--
INSERT INTO Foo
SELECT 'EL' AS Id,
       Set_Start_Epoch(  
         DT_Period( 
                    Finish ( T.When ),
                    Finish ( T.When ) + Make_INTERVAL( 10,10,10,10 )
         )
        ) AS When,
       ' EL   --------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'LU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Finish(T.When) + Make_INTERVAL( 10,10,10,10 ),
                    Finish(T.When) + Make_INTERVAL( 20,10,10,10 )
                  )
       ) AS When,
       '              |=========================================== LU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'A';
--
--        B             |---|
--       EM  -----------|========================================  MU
--       EN  -------------|======================================  NU
--
--  EM/MU
--
INSERT INTO Foo
SELECT 'EM' AS Id,
       Set_Start_Epoch(  
        DT_Period ( Start ( T.When ) - Make_INTERVAL( 10,10,10,10),
                    Start ( T.When )
        )
       ) AS When,
       ' EM ------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'B';
--
INSERT INTO Foo
SELECT 'MU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Finish(T.When) + Make_INTERVAL( 10,10,10,10 ),
                    Finish(T.When) + Make_INTERVAL( 20,10,10,10 )
                  )
       ) AS When,
       '                |========================================= MU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'B';
--
--  EN/NU
--
INSERT INTO Foo
SELECT 'EN' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ),
                     Start ( T.When ) + Make_INTERVAL( 10,10,10,10)
         )
       ) AS When,
       ' EN --------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'B';
--
INSERT INTO Foo
SELECT 'NU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Start(T.When) + Make_INTERVAL( 10,10,10,10 ),
                    Finish(T.When) + Make_INTERVAL ( 10,10,10,10 )
                  )
       ) AS When,
       '                  |======================================= NU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'B';
--
--        C                 |------------|
--       EO  ---------------|====================================  OU
--       EP  -----------------|==================================  PU
--       EU  ----------------------------|=======================  UU
--
--  EO/OU
--
INSERT INTO Foo
SELECT 'EO' AS Id,
       Set_Start_Epoch (  
         DT_Period ( Start ( T.When ) - Make_INTERVAL( 10,10,10,10),
                     Start ( T.When )
         )
       ) AS When,
       ' EO ----------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'OU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                    |===================================== OU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
--  EP/PU
--
INSERT INTO Foo
SELECT 'EP' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ),
                     Start ( T.When ) + Make_INTERVAL( 10,10,10,10)
         )
       ) AS When,
       ' EP ------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'PU' AS Id,
       Set_Finish_Forever( 
        DT_Period ( Start ( T.When ) + Make_INTERVAL(10,10,10,10),
                    Start ( T.When ) + Make_INTERVAL(20,10,10,10)
        )
       ) AS When,
       '                      |=================================== PU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'EU' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' EU -----------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'UU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ),
                     Finish ( T.When ) + Make_INTERVAL(20,10,10,10)
         )
       ) AS When,
       '                                 |======================== UU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'C';
--
--        D                     |----|
--       EQ  -------------------|================================  QU
--       ER  ---------------------|==============================  RU
--       ES  ------------------------|===========================  SU
--
--  EQ/QU
--
INSERT INTO Foo
SELECT 'EQ' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ) - Make_INTERVAL( 10,10,10,10),
                     Start ( T.When )
         )
       ) AS When,
       ' EQ --------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'QU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                        |================================ QU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
--  ER/RU
--
INSERT INTO Foo
SELECT 'ER' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ),
                     Start ( T.When ) + Make_INTERVAL( 10,10,10,10)
         )
       ) AS When,
       ' ER ----------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'RU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Start ( T.When ) + Make_INTERVAL(10,10,10,10),
                     Start ( T.When ) + Make_INTERVAL(20,10,10,10)
         )
       ) AS When,
       '                          |============================== RU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
--  ES/SU
--
INSERT INTO Foo
SELECT 'ES' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' ES -------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'SU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ),
                     Finish ( T.When ) + Make_INTERVAL(20,10,10,10)
         )
       ) AS When,
       '                             |=========================== SU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'D';
--
--        E                              |----|
--       ET  ----------------------------|=======================  TU
--       EV  -------------------------------|====================  VU
--       EW  ---------------------------------|==================  WU
--
--  ET/TU  (Brutus?)
--
INSERT INTO Foo
SELECT 'ET' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ) - Make_INTERVAL( 10,10,10,10),
                     Start ( T.When )
         )
       ) AS When,
       ' ET -----------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'TU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                 |====================== TU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
--  EV/VU
--
INSERT INTO Foo
SELECT 'EV' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ),
                     Start ( T.When ) + Make_INTERVAL( 20,10,10,10)
         )
       ) AS When,
       ' EV --------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'VU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Start ( T.When ) + Make_INTERVAL(20,10,10,10),
                     Start ( T.When ) + Make_INTERVAL(30,10,10,10)
         )
       ) AS When,
       '                                    |=================== VU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
--  EW/WU
--
INSERT INTO Foo
SELECT 'EW' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' EW ----------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'WU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ),
                     Finish ( T.When ) + Make_INTERVAL(10,10,10,10)
         )
       ) AS When,
       '                                      |================= WU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'E';
--
--        F                                   |----|
--       EX  --------------------------------------|=============  XU
--
--  EX/XU
--
INSERT INTO Foo
SELECT 'EX' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' EX  --------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'F';
--
INSERT INTO Foo
SELECT 'XU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ),
                     Finish ( T.When ) + Make_INTERVAL(10,10,10,10)
         )
       ) AS When,
       '                                           |============ XU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'F';
--
--        G                                            |----|
--       EY  ----------------------------------------|===========  YU
--       EZ  ------------------------------------------|=========  ZU
--      EZA  ---------------------------------------------|======  ZAU
--      EZB  -----------------------------------------------|====  ZBU
--      EZC  -------------------------------------------------|==  ZCU
--      E=U  ====================================================
-- 
--  EY/YU
--
INSERT INTO Foo
SELECT 'EY' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start ( T.When ) - Make_INTERVAL( 10,10,10,10),
                     Start ( T.When )
         )
       ) AS When,
       ' EY -----------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'YU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                             |========== YU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
--  EZ/ZU
--
INSERT INTO Foo
SELECT 'EZ' AS Id,
       Set_Start_Epoch( 
         DT_Period ( Start(T.When) - Make_INTERVAL(10,10,10,10),
                     Start(T.When)
         )
       ) AS When,
       ' EZ -------------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                               |======== ZU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
--  EZA/ZAU
--
INSERT INTO Foo
SELECT 'EZA' AS Id,
       Set_Start_Epoch(  
         DT_Period ( Start(T.When),
                     Start(T.When) + Make_INTERVAL(10,10,10,10)
         )
       ) AS When,
       'EZA ----------------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZAU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Start ( T.When ) + Make_INTERVAL(10,10,10,10),
                     Finish ( T.When ) + Make_INTERVAL(20,10,10,10)
         )
       ) AS When,
       '                                                  |===== ZAU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
--  EZB/ZBU
--
INSERT INTO Foo
SELECT 'EZB' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       'EZB ------------------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZBU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ),
                     Finish ( T.When ) + Make_INTERVAL(10,10,10,10)
         )
       ) AS When,
       '                                                    |=== ZBU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'EZC' AS Id,
       Set_Start_Epoch(  
        DT_Period ( Finish(T.When),
                    Finish(T.When) + Make_INTERVAL(10,10,10,10)
                  )
       ) AS When,
       'EZC --------------------------------------------------|' AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZCU' AS Id,
       Set_Finish_Forever( 
         DT_Period ( Finish ( T.When ) + Make_INTERVAL(10,10,10,10),
                     Finish ( T.When ) + Make_INTERVAL(20,10,10,10)
         )
       ) AS When,
       '                                                      |= ZCU'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
--  E=U
--
INSERT INTO Foo
SELECT 'ZCU' AS Id,
       Set_Start_Epoch (Set_Finish_Forever( T.When )) AS When,
       'E=U ==================================================== E=U'
         AS Picture
  FROM Test_DT_Period T
 WHERE Id = 'G';
--
--
INSERT INTO Test_DT_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
--   And Repeat the Tests
--
SELECT '==========================================================' AS Spacer,
       T1.Id AS One, T2.Id AS Two,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       '             Equal (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Equal( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Equal,

       '          NotEqual (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           NotEqual ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS NotEqual,

       'ContainsNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           ContainsNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS ContainsNotTouches,

       '          Contains (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Contains ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Contains,

       '  WithinNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           WithinNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS WithinNotTouches,

       '            Within (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Within ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Within,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Overlap,

       '            Before (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Before ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Before,

       '     BeforeTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           BeforeTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS BeforeTouches,

       '             After (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           After ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS After,

       '      AfterTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           AfterTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS AfterTouches,

       '----------------------------------------------------------' AS Spacer_2,
       'GetIntersect (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetIntersect ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Intersect,
       'GetUnion (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           GetUnion ( T1.When, T2.When)::LVARCHAR::VARCHAR(48) AS Union,
       'Start( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Start( GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Start_Union,
       'Finish( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Finish(GetIntersect ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Finish_Intersect,
       'Length( GetUnion (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union,
       'Period_Interval( GetIntersect (' || T1.Id || ' , ' || T2.Id || ' )) is ' || 
           Period_Interval(GetIntersect (T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Union_Interval
  FROM Test_DT_Period T1, Test_DT_Period T2;
--
------------------------------- OUT PARAMETERS -----------------------------
--
SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2) ||
       ' with Length ' || Overlap_Length AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) AS Overlap,

       '----------------------------------------------------------' AS Spacer_2
  FROM Test_DT_Period T1, Test_DT_Period T2
 WHERE OverlapNotTouches ( T1.When, T2.When, Overlap_Length # INTEGER );
--
SELECT '==========================================================' AS Spacer,
       T1.When AS First,
       T2.When AS Second,
       T1.Picture AS First_Pic,
       T2.Picture AS Secon_Pic,
       '----------------------------------------------------------' AS Spacer_1,
       '     CompareString (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           CompareString( T1.When, T2.When )::LVARCHAR AS CompareString,

       ' OverlapNotTouches (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           OverlapNotTouches ( T1.When, T2.When )::LVARCHAR::CHAR(2)
       AS OverlapNotTouches,

       '           Overlap (' || T1.Id || ' , ' || T2.Id || ' ) is ' || 
           Overlap ( T1.When, T2.When )::LVARCHAR::CHAR(2) ||
       ' with length ' || Overlap_Length AS Overlap,

       '----------------------------------------------------------' AS Spacer_2
  FROM Test_DT_Period T1, Test_DT_Period T2
 WHERE Overlap ( T1.When, T2.When, Overlap_Length # INTEGER );
--
-------------------------------- BOOK-KEEPING ------------------------------
--
--   1. UNION
--
SELECT * FROM Test_DT_Period
UNION
SELECT * FROM Test_DT_Period;
--
--  2. ORDER BY
--
SELECT * FROM Test_DT_Period ORDER BY When;
--
--
--  3. Aggregate
--
SELECT T1.Id,
       Min_Overlap( T2.When )
  FROM Test_DT_Period T1, Test_DT_Period T2
 WHERE Overlap ( T1.When, T2.When )
 GROUP BY T1.Id
 HAVING NVL( Min_Overlap( T2.When)::LVARCHAR, "NO") <> "NO";
--
------------------------------ STATISTICS ------------------------------------
-- --
-- DROP TABLE Test_D_Period;
-- DROP FUNCTION DT ( INTEGER, INTEGER, INTEGER, INTEGER, INTEGER,
--                    INTEGER, INTEGER, INTEGER, INTEGER, INTEGER );
-- --
-- CREATE TABLE Test_D_Period
-- (       Id      SERIAL     PRIMARY KEY,
--         FK      INTEGER    NOT NULL,
--         When    DT_Period  NOT NULL
-- );
-- --
-- CREATE FUNCTION DT ( ArgYear INTEGER, ArgMonth INTEGER, ArgDay INTEGER,
--                      ArgHour INTEGER, ArgMinute INTEGER, ArgSecond INTEGER,
--                      ArgIntDays INTEGER, ArgIntHrs INTEGER, ArgIntMins INTEGER,
--                      ArgIntSecs INTEGER )
-- RETURNS DT_Period
-- 
--         DEFINE DTStart   DATETIME YEAR TO SECOND;
--         DEFINE DTFinish  DATETIME YEAR TO SECOND;
--         DEFINE InDiff    INTERVAL DAY(4) TO SECOND;
-- 
--         LET DTStart = (ArgYear || '-' || ArgMonth || '-' || ArgDay || ' ' ||
--                        ArgHour || ':' || ArgMinute || ':' || ArgSecond
--                       )::DATETIME YEAR TO SECOND;
-- 
--         LET InDiff  = (ArgIntDays || ' ' || ArgIntHrs || ':' || ArgIntMins ||
--                        ':' || ArgIntSecs
--                       )::INTERVAL DAY(4) TO SECOND;
-- 
--         LET DTFinish = DTStart + InDiff;
-- 
--         RETURN DT_Period ( DTStart, DTFinish );
-- 
-- END FUNCTION;
-- --
-- EXECUTE FUNCTION DT ( 1999, 12, 10, 10, 10, 10, 5, 2, 10, 15 );
-- --
-- INSERT INTO Test_D_Period
-- ( FK, When )
-- SELECT N1.Int * 10 + N1.Int,
--        DT ( Yr.Y, Mnth.M, (Dy.D + N1.Int), Hr.H, 10, 10,
--             N2.Int, MOD((N2.Int*111),24), 11, 12 )
--   FROM
--        TABLE(SET{1998,1999, 2000, 2001}) Yr ( Y ),
--        TABLE(SET{1,2,3,4,5,6,7,8,9,10,11,12}) Mnth (M),
--        TABLE(SET{1,15,20,23}) Dy ( D ),
--        TABLE(SET{2,6,12,15}) Hr ( H ),
--        TABLE(SET{0,1,2,4,5}) N1 ( Int ),
--        TABLE(SET{1,2,4,6,8,10}) N2 ( Int );
-- --
-- EXECUTE FUNCTION DT ( 1999, 12, 10, 10, 10, 10, 5, 2, 10, 15 );
-- --
-- SELECT COUNT(*) FROM Test_D_Period;
-- --
-- SET EXPLAIN ON;
-- --
-- --  Equal 
-- --
-- SELECT *
--   FROM Test_D_Period T
--  WHERE T.When = DT ( 1999, 10, 20, 9, 10, 10,
--                      2, 0, 0, 0 );
-- --
-- SELECT *
--   FROM Test_D_Period T
--  WHERE T.When = "1999-01-01 02:10:10" to "1999-01-02 17:21:22";
-- --
-- SELECT * 
--   FROM Test_D_Period T 
-- --
-- --  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
-- --                  just to check it.)
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Overlap(T.When, 
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Overlap(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE OverlapNotTouches(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- --  Within ( Note: Check this one!)
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Within(T.When, '"10/20/2000" to "10/22/2000"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE WithinNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Within(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- -- Contains
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Contains(T.When, '"10/20/2000" to "10/22/2000"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE ContainsNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Contains(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- --
-- UPDATE STATISTICS HIGH;
-- --
-- --  Equal 
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE T.When = '"10/20/2000" to "10/22/2000"';
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE T.When = '"10/20/1999" to "10/22/1999"';
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE T.When = '"10/20/2000" to "10/22/2000"';
-- --
-- --  Overlap  (Note: OverlapNotTouches will be equivalent, but use both
-- --                  just to check it.)
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Overlap(T.When, '"10/20/2000" to "10/22/2000"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Overlap(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE OverlapNotTouches(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- --  Within ( Note: Check this one!)
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Within(T.When, '"10/20/2000" to "10/22/2000"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE WithinNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Within(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- -- Contains
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Contains(T.When, '"10/20/2000" to "10/22/2000"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE ContainsNotTouches(T.When, '"10/20/1999" to "12/22/1999"');
-- --
-- SELECT * 
--   FROM Test_D_Period T 
--  WHERE Contains(T.When, '"12/20/2000" to "12/22/2001"');
-- --
-- -----------------------------------------------------------------------------
-- --
