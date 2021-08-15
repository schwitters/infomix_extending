--
--   File: ExecIt/install/exec.sql
--
--  About: 
--
--  This script contains the definitions of the Exec() function.
--
EXECUTE PROCEDURE ifx_allow_newline('t');
DROP FUNCTION Exec ( lvarchar);
--
create function Exec ( lvarchar)
returns lvarchar
external name 
'$INFORMIXDIR/extend/ExecIt/bin/exec.bld(Exec_to_String)'
language C not variant;
--
GRANT EXECUTE ON FUNCTION Exec (lvarchar) TO PUBLIC;
--
DROP TABLE Foo;
--
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
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Severn'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Severn'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
       TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
                 ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Severn'),
                 ROW(8,'Eight'),ROW(9,'Nine')
                }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
--

