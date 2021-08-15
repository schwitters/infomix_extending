--
--   File: ExecIt/install/regression.sql
--
--  About: 
-- 
--   This script handles the minimal regression testing for the Exec() and
--  Exec_for_Rows() user-defined routines. 
--
--   If you make any changes, make sure that this script runs clean and that
--  it doesn't produce any memory leaks (set the size of the next buffer to
--  be assigned to less than 8Meg).
--
--   Also, in parallel with this regression set, run a couple of mu_test.sql
--  in parallel. These are the same as the "memory leak" query, but having 
--  a couple of them going stretches the system.
--
--  Basic Stuff
--
-------------------------------------  SETUP  --------------------------------
--
EXECUTE PROCEDURE ifx_allow_newline('t');
--
----------------------- First: Testing the Exec() UDR ------------------------
--
DROP TABLE Foo;
--
SELECT Exec("CREATE TABLE Foo ( A INTEGER, B VARCHAR(32));") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES ( 1, 'Hello' ) ;") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES ( 2, 'Good-Bye' ) ;") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES ( 3, 'Stay Here' ) ;") FROM TABLE(SET{1});
SELECT Exec("SELECT COUNT(*) FROM Foo;") FROM TABLE(SET{1});
SELECT Exec("SELECT * FROM Foo;") FROM TABLE(SET{1});
--
--
EXECUTE FUNCTION Exec("
INSERT INTO Foo
SELECT T1.Num * 100 + T2.Num * 10 + T3.Num,
       T1.Val || ',' || T2.Val || ',' || T3.Val
  FROM TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
--
SELECT Exec("SELECT COUNT(*) FROM Foo;") FROM TABLE(SET{1});
SELECT Exec("SELECT * FROM Foo;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B) FROM Foo F;") FROM TABLE(SET{1});
SELECT Exec("UPDATE Foo SET B = 'Zap!' WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B) FROM Foo F WHERE A = 4;") FROM TABLE(SET{1});
--
SELECT Exec("SELECT ROW(F.A, F.B || '-' || F.B || '-' || F.A) FROM Foo F WHERE A = 4;") FROM TABLE(SET{1});
--
DROP TABLE Foo;
--
CREATE DISTINCT TYPE Num AS INTEGER;
DROP CAST ( INTEGER AS Num );
CREATE IMPLICIT CAST ( INTEGER AS Num );
--
CREATE DISTINCT TYPE Val AS VARCHAR(32);
DROP CAST ( VARCHAR(32) AS Val );
CREATE IMPLICIT CAST ( VARCHAR(32) AS Val );
--
SELECT Exec("CREATE TABLE Foo ( A Num, B Val, C SET(INTEGER NOT NULL));") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES (1,'Hello',SET{1,2,3});") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES (2,'Good-Bye',SET{4,5,6});") FROM TABLE(SET{1});
SELECT Exec("INSERT INTO Foo VALUES (3,'Stay Here',SET{7,8,9});") FROM TABLE(SET{1});
SELECT Exec("SELECT COUNT(*) FROM Foo;") FROM TABLE(SET{1});
SELECT Exec("SELECT * FROM Foo;") FROM TABLE(SET{1});
--
--   
EXECUTE FUNCTION Exec("
INSERT INTO Foo
SELECT ( T1.Num * 100 + T2.Num * 10 + T3.Num )::Integer::Num,
       ( T1.Val || ',' || T2.Val || ',' || T3.Val )::Val,
       SET{ T1.Num, T2.Num, T3.Num }
  FROM TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
--
SELECT Exec("SELECT COUNT(*) FROM Foo;") FROM TABLE(SET{1});
SELECT Exec("SELECT * FROM Foo F WHERE MOD(F.A, 10) = 0;") FROM TABLE(SET{1});
SELECT Exec("SELECT F.A, F.C FROM Foo F WHERE MOD(F.A,10) = 0;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B, F.C) FROM Foo F WHERE MOD(F.A,10) = 0;") FROM TABLE(SET{1});
SELECT Exec("UPDATE Foo SET B = 'Zap!' WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B, F.C) FROM Foo F WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT F.A, F.B FROM Foo F WHERE A = -4;") FROM TABLE(SET{1});
--
SELECT Exec("
SELECT ROW(F.A,F.B::LVARCHAR || '-' || F.B::LVARCHAR || '-' || F.B::LVARCHAR)
  FROM Foo F 
 WHERE F.A = 4;") 
  FROM TABLE(SET{1});
--
--   Clean-up
--
DROP TABLE Foo;
--
DROP TYPE Num RESTRICT;
DROP TYPE Val RESTRICT;
--
---------------------  Second: Test Exec_Iterator --------------------------
--
--  DROP TABLE Foo;
--
EXECUTE FUNCTION Exec_for_Rows("CREATE TABLE Foo ( A INTEGER, B VARCHAR(32));");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES ( 1, 'Hello' ) ;");
EXECUTE FUNCTION Exec_For_Rows("INSERT INTO Foo VALUES ( 2, 'Good-Bye' ) ;");
EXECUTE FUNCTION Exec_For_Rows("INSERT INTO Foo VALUES ( 3, 'Stay Here' ) ;");
EXECUTE FUNCTION Exec_For_Rows("SELECT COUNT(*) FROM Foo;");
EXECUTE FUNCTION Exec_For_Rows("SELECT * FROM Foo;");
--
--
EXECUTE FUNCTION Exec_for_Rows("
INSERT INTO Foo
SELECT T1.Num * 100 + T2.Num * 10 + T3.Num,
       T1.Val || ',' || T2.Val || ',' || T3.Val
  FROM TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
--
EXECUTE FUNCTION Exec_for_Rows("SELECT COUNT(*) FROM Foo;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo;");
EXECUTE FUNCTION Exec_for_Rows("SELECT ROW(F.A, F.B) FROM Foo F;");
EXECUTE FUNCTION Exec_for_Rows("UPDATE Foo SET B = 'Zap!' WHERE A = 4;");
EXECUTE FUNCTION Exec_for_Rows("SELECT ROW(F.A, F.B) FROM Foo F WHERE A = 4;");
--
EXECUTE FUNCTION Exec_for_Rows("SELECT ROW(F.A, F.B || '-' || F.B || '-' || F.A) FROM Foo F WHERE A = 4;") ;
--
DROP TABLE Foo;
--
CREATE DISTINCT TYPE Num AS INTEGER;
DROP CAST ( INTEGER AS Num );
CREATE IMPLICIT CAST ( INTEGER AS Num );
--
CREATE DISTINCT TYPE Val AS VARCHAR(32);
DROP CAST ( VARCHAR(32) AS Val );
CREATE IMPLICIT CAST ( VARCHAR(32) AS Val );
--
EXECUTE FUNCTION Exec_for_Rows("CREATE TABLE Foo ( A Num, B Val, C SET(INTEGER NOT NULL));");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES (1,'Hello',SET{1,2,3});");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES (2,'Good-Bye',SET{4,5,6});");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES (3,'Stay Here',SET{7,8,9});");
EXECUTE FUNCTION Exec_for_Rows("SELECT COUNT(*) FROM Foo;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo;");
--
--   
--
EXECUTE FUNCTION Exec("
INSERT INTO Foo
SELECT ( T1.Num * 100 + T2.Num * 10 + T3.Num )::Integer::Num,
       ( T1.Val || ',' || T2.Val || ',' || T3.Val )::Val,
       SET{ T1.Num, T2.Num, T3.Num }
  FROM TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
--
EXECUTE FUNCTION Exec_for_Rows("SELECT COUNT(*) FROM Foo;");
SELECT Exec("SELECT * FROM Foo F WHERE MOD(F.A, 10) = 0;") FROM TABLE(SET{1});
SELECT Exec("SELECT F.A, F.C FROM Foo F WHERE MOD(F.A,10) = 0;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B, F.C) FROM Foo F WHERE MOD(F.A,10) = 0;") FROM TABLE(SET{1});
SELECT Exec("UPDATE Foo SET B = 'Zap!' WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT ROW(F.A, F.B, F.C) FROM Foo F WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT F.A, F.B, F.C, SET{ F.A } FROM Foo F WHERE A = 4;") FROM TABLE(SET{1});
SELECT Exec("SELECT F.A, F.B FROM Foo F WHERE A = -4;") FROM TABLE(SET{1});
--
SELECT Exec("
SELECT ROW(F.A,F.B::LVARCHAR || '-' || F.B::LVARCHAR || '-' || F.B::LVARCHAR)
  FROM Foo F 
 WHERE F.A = 4;") 
  FROM TABLE(SET{1});
--
-----------------  And the famous memory leak test -------------------------
--
EXECUTE FUNCTION Exec_For_Rows("SELECT F1.A, ROW(F2.A, F1.B), SET{F1.B, F2.B}, F2.B, F2.C FROM Foo F1, Foo F2;");
--
--   Clean-up
--
DROP TABLE Foo;
--
DROP TYPE Num RESTRICT;
DROP TYPE Val RESTRICT;
