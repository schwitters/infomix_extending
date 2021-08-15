--
--  File:  d_regression_min.sql
--
-- About:
--
--     This is being set up to do regression testing on the body of the 
--    extensions.
--
-------------------------------- CHECK LOAD ---------------------------------
--
EXECUTE FUNCTION Period ( "10/10/2000", "11/11/2000");
--
---------------------------------- HYGIENE ----------------------------------
--
-- DROP FUNCTION Make_DATE ( INTEGER, INTEGER, INTEGER );
--
-- DROP TABLE Test_D_Period;
--
--  This is a temporary table. On occasions, when the script does not
-- complete successfully, you need to drop this in order that the
-- script runs without error.
--
-- DROP TABLE Foo;
--
----------------------- USEFUL FUNCTIONS FOR TESTING ------------------------
--
CREATE FUNCTION Make_DATE( ARG_DAY INTEGER, ARG_MONTH INTEGER, 
                           ARG_YEAR INTEGER )
RETURNING DATE
	RETURN ARG_MONTH || '/' || ARG_DAY || '/' || ARG_YEAR;
END FUNCTION;
--
EXECUTE FUNCTION Make_DATE( 10, 10, 2000 );
--
------------------------------ SMALL TESTS -----------------------------------
--
EXECUTE FUNCTION PeriodIn('"10/10/2000" to "11/11/2000"');
EXECUTE FUNCTION PeriodIn('"10/10/1999" to "01/01/2000"');
EXECUTE FUNCTION PeriodIn('"01/05/2000" to "02/11/2001"');
--
EXECUTE FUNCTION PeriodIn('"EPOCH" to "06/08/2000"');
EXECUTE FUNCTION PeriodIn('"12/04/2000" to "FOREVER"');
EXECUTE FUNCTION PeriodIn('"EPOCH" to "FOREVER"');
--
EXECUTE FUNCTION Set_Start_Epoch ( 
		   PeriodIn('"10/10/1999" to "01/01/2000"')
                 );
EXECUTE FUNCTION Set_Finish_Forever (
		   PeriodIn('"10/10/1999" to "01/01/2000"')
                 );
EXECUTE FUNCTION Set_Start_Epoch (
                  Set_Finish_Forever (
		   PeriodIn('"10/10/1999" to "01/01/2000"')
                 ));
--
EXECUTE FUNCTION PeriodIn('"EPOCH" to "06/08/2000"');
EXECUTE FUNCTION PeriodIn('"06/08/1999" to "FOREVER"');
EXECUTE FUNCTION PeriodIn('"EPOCH" to "FOREVER"');
--
--  Test a minimal set of the functionality. The idea is to create a set
-- of Period in a table, and then do some queries.
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
--   Happily, the SQL-92 DATE type allows you to add INTEGER days to 
--  a DATE data value. 
--
CREATE TABLE Test_D_Period (
	Id	VARCHAR(32)	NOT NULL,
	When	Period	        NOT NULL,
        Picture VARCHAR(70)     NOT NULL  
);
--
INSERT INTO Test_D_Period 
VALUES ( 'A', Period ("10/21/1999"::DATE,
                      "10/21/1999"::DATE + 60 UNITS DAY ),
       '  A    |----|');
--
SELECT 'B' AS Id,
       Period ( Finish(T.When) + 50 UNITS DAY,
                Finish(T.When) + 90 UNITS DAY
              ) AS When,
       '  B             |---|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A'
  INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'C' AS Id,
       Period ( Finish(T.When), 
                Finish(T.When) + 140 UNITS DAY
              ) AS When,
       '  C                 |------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'B'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'D' AS Id,
       Period ( Start(T.When) + 30 UNITS DAY,
                Start(T.When) + 80 UNITS DAY
              ) AS When,
       '  D                     |----|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'E' AS Id,
       Period ( Finish(T.When),
                Finish(T.When) + 60 UNITS DAY
              ) AS When,
       '  E                              |----|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'F' AS Id,
       Period ( Finish(T.When),
                Finish(T.When) + 50 UNITS DAY
              ) AS When,
       '  F                                   |----|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'G' AS Id,
       Period ( Finish(T.When) + 50 UNITS DAY,
                Finish(T.When) + 100 UNITS DAY
              ) AS When,
       '  G                                            |----|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'F'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
DROP TABLE Foo;
--
SELECT 'H' AS Id,
       Period ( Finish(T.When) - 10 UNITS DAY,
                Finish(T.When) + 40 UNITS DAY
              ) AS When,
       '  H                            |---|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C'
 INTO TEMP Foo;
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
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
           Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union
  FROM Test_D_Period T1, Test_D_Period T2;
--
EXECUTE FUNCTION Period ( "2000-10-10 12:10:10", "2000-10-11 12:10:10");
--
--  OK. Now for the second lot.
--
CREATE TEMP TABLE Foo (
	Id	VARCHAR(32)	NOT NULL,
	When	Period	        NOT NULL,
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
        Period ( 
		 Start(T.When) -  20 UNITS DAY,
                 Start(T.When) -  10 UNITS DAY
               )
       ) AS When,
       ' EH -|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'HU' AS Id,
       Set_Finish_Forever( 
        Period ( 
		 Start(T.When) -  10 UNITS DAY,
                 Start(T.When)
               )
       ) AS When,
       '     |===================================================  HU' 
            AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
--  EI/IU
--
INSERT INTO Foo
SELECT 'EI' AS Id,
       Set_Start_Epoch( 
        Period ( 
		 Start(T.When) - 10 UNITS DAY,
                 Start(T.When)
               )
       ) AS When,
       ' EI ---|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'IU' AS Id,
       Set_Finish_Forever( 
         Period (
                 Start(T.When), 
                 Start(T.When) + 10 UNITS DAY
         )
       ) AS When,
       '       |================================================== IU' 
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
--  EJ/JU
--
INSERT INTO Foo
SELECT 'EJ' AS Id,
       Set_Start_Epoch( 
        Period ( 
		 Start(T.When) - 20 UNITS DAY,
                 Start(T.When) + 10 UNITS DAY
               )
       ) AS When,
       ' EJ -----|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'JU' AS Id,
       Set_Finish_Forever( 
        Period ( Start(T.When) + 10 UNITS DAY,
                 Start(T.When) + 20 UNITS DAY
               )
       ) AS When,
       '         |================================================ JU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
--  EK/KU
--
INSERT INTO Foo
SELECT 'EK' AS Id,
       Set_Start_Epoch( T.When )
        AS When,
       ' EK --------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'KU' AS Id,
       Set_Finish_Forever( 
        Period ( Finish(T.When),
                 Finish(T.When) + 10 UNITS DAY
               )
       ) AS When,
       '            |============================================= KU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
--  EL/LU
--
INSERT INTO Foo
SELECT 'EL' AS Id,
       Set_Start_Epoch(  
         Period( 
                Finish ( T.When ),
                Finish ( T.When ) + 10 UNITS DAY
         )
        ) AS When,
       ' EL   --------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'A';
--
INSERT INTO Foo
SELECT 'LU' AS Id,
       Set_Finish_Forever( 
        Period ( Finish(T.When) + 10 UNITS DAY,
                 Finish(T.When) + 20 UNITS DAY
               )
       ) AS When,
       '              |=========================================== LU'
         AS Picture
  FROM Test_D_Period T
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
        Period ( Start ( T.When ) -  10 UNITS DAY,
                 Start ( T.When )
        )
       ) AS When,
       ' EM ------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'B';
--
INSERT INTO Foo
SELECT 'MU' AS Id,
       Set_Finish_Forever( 
        Period ( Finish(T.When) + 10 UNITS DAY,
                 Finish(T.When) + 20 UNITS DAY
               )
       ) AS When,
       '                |========================================= MU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'B';
--
--  EN/NU
--
INSERT INTO Foo
SELECT 'EN' AS Id,
       Set_Start_Epoch(  
         Period ( Start ( T.When ),
                  Start ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       ' EN --------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'B';
--
INSERT INTO Foo
SELECT 'NU' AS Id,
       Set_Finish_Forever( 
        Period ( Start(T.When)  + 10 UNITS DAY,
                 Finish(T.When) + 10 UNITS DAY
               )
       ) AS When,
       '                  |======================================= NU'
         AS Picture
  FROM Test_D_Period T
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
         Period ( Start ( T.When ) -  10 UNITS DAY,
                  Start ( T.When )
         )
       ) AS When,
       ' EO ----------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'OU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                    |===================================== OU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C';
--
--  EP/PU
--
INSERT INTO Foo
SELECT 'EP' AS Id,
       Set_Start_Epoch(  
         Period ( Start ( T.When ),
                  Start ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       ' EP ------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'PU' AS Id,
       Set_Finish_Forever( 
        Period ( Start ( T.When ) + 10 UNITS DAY,
                 Start ( T.When ) + 20 UNITS DAY
        )
       ) AS When,
       '                      |=================================== PU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'EU' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' EU -----------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'C';
--
INSERT INTO Foo
SELECT 'UU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ),
                  Finish ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       '                                 |======================== UU'
         AS Picture
  FROM Test_D_Period T
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
         Period ( Start ( T.When ) -  10 UNITS DAY,
                  Start ( T.When )
         )
       ) AS When,
       ' EQ --------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'QU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                        |================================ QU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'D';
--
--  ER/RU
--
INSERT INTO Foo
SELECT 'ER' AS Id,
       Set_Start_Epoch(  
         Period ( Start ( T.When ),
                  Start ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       ' ER ----------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'RU' AS Id,
       Set_Finish_Forever( 
         Period ( Start ( T.When ) + 10 UNITS DAY,
                  Start ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       '                          |============================== RU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'D';
--
--  ES/SU
--
INSERT INTO Foo
SELECT 'ES' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' ES -------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'D';
--
INSERT INTO Foo
SELECT 'SU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ),
                  Finish ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       '                             |=========================== SU'
         AS Picture
  FROM Test_D_Period T
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
         Period ( Start ( T.When ) -  10 UNITS DAY,
                  Start ( T.When )
         )
       ) AS When,
       ' ET -----------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'TU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                 |====================== TU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E';
--
--  EV/VU
--
INSERT INTO Foo
SELECT 'EV' AS Id,
       Set_Start_Epoch(  
         Period ( Start ( T.When ),
                  Start ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       ' EV --------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'VU' AS Id,
       Set_Finish_Forever( 
         Period ( Start ( T.When ) + 20 UNITS DAY,
                  Start ( T.When ) + 30 UNITS DAY
         )
       ) AS When,
       '                                    |=================== VU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E';
--
--  EW/WU
--
INSERT INTO Foo
SELECT 'EW' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       ' EW ----------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'E';
--
INSERT INTO Foo
SELECT 'WU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ),
                  Finish ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       '                                      |================= WU'
         AS Picture
  FROM Test_D_Period T
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
  FROM Test_D_Period T
 WHERE Id = 'F';
--
INSERT INTO Foo
SELECT 'XU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ),
                  Finish ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       '                                           |============ XU'
         AS Picture
  FROM Test_D_Period T
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
         Period ( Start ( T.When ) -  10 UNITS DAY,
                  Start ( T.When )
         )
       ) AS When,
       ' EY -----------------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'YU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                             |========== YU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
--  EZ/ZU
--
INSERT INTO Foo
SELECT 'EZ' AS Id,
       Set_Start_Epoch( 
         Period ( Start(T.When) - 10 UNITS DAY,
                  Start(T.When)
         )
       ) AS When,
       ' EZ -------------------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZU' AS Id,
       Set_Finish_Forever( T.When ) AS When,
       '                                               |======== ZU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
--  EZA/ZAU
--
INSERT INTO Foo
SELECT 'EZA' AS Id,
       Set_Start_Epoch(  
         Period ( Start(T.When),
                  Start(T.When) + 10 UNITS DAY
         )
       ) AS When,
       'EZA ----------------------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZAU' AS Id,
       Set_Finish_Forever( 
         Period ( Start ( T.When )  + 10 UNITS DAY,
                  Finish ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       '                                                  |===== ZAU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
--  EZB/ZBU
--
INSERT INTO Foo
SELECT 'EZB' AS Id,
       Set_Start_Epoch(  T.When ) AS When,
       'EZB ------------------------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZBU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ),
                  Finish ( T.When ) + 10 UNITS DAY
         )
       ) AS When,
       '                                                    |=== ZBU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'EZC' AS Id,
       Set_Start_Epoch(  
        Period ( Finish(T.When),
                 Finish(T.When) + 10 UNITS DAY
               )
       ) AS When,
       'EZC --------------------------------------------------|' AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
INSERT INTO Foo
SELECT 'ZCU' AS Id,
       Set_Finish_Forever( 
         Period ( Finish ( T.When ) + 10 UNITS DAY,
                  Finish ( T.When ) + 20 UNITS DAY
         )
       ) AS When,
       '                                                      |= ZCU'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
--  E=U
--
INSERT INTO Foo
SELECT 'ZCU' AS Id,
       Set_Start_Epoch (Set_Finish_Forever( T.When )) AS When,
       'E=U ==================================================== E=U'
         AS Picture
  FROM Test_D_Period T
 WHERE Id = 'G';
--
--
INSERT INTO Test_D_Period SELECT * FROM Foo;
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
           Length(GetUnion ( T1.When, T2.When))::LVARCHAR::VARCHAR(32) AS Length_Union
  FROM Test_D_Period T1, Test_D_Period T2;
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
  FROM Test_D_Period T1, Test_D_Period T2
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
  FROM Test_D_Period T1, Test_D_Period T2
 WHERE Overlap ( T1.When, T2.When, Overlap_Length # INTEGER );
--
-------------------------------- BOOK-KEEPING ------------------------------
--
--   1. UNION
--
SELECT * FROM Test_D_Period
UNION
SELECT * FROM Test_D_Period;
--
--  2. ORDER BY
--
SELECT * FROM Test_D_Period ORDER BY When;
--
--
--  3. Aggregate
--
SELECT T1.Id,
       Min_Period_Overlap( T2.When )
  FROM Test_D_Period T1, Test_D_Period T2
 WHERE Overlap ( T1.When, T2.When )
 GROUP BY T1.Id
 HAVING NVL( Min_Period_Overlap( T2.When)::LVARCHAR, "NO") <> "NO";
--
