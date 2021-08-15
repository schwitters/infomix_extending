--
--   File: ExecIt/install/exec4rows.sql
--
--  About: 
--
--  This script contains the definitions of the Exec() function.
--
DROP FUNCTION Exec_for_Rows ( lvarchar);
--
create function Exec_For_Rows ( lvarchar)
returns lvarchar
with ( iterator, STACK = 64 )
external name 
'$INFORMIXDIR/extend/ExecIt/bin/exec.bld(Exec_Iterator)'
language C not variant;
--
GRANT EXECUTE ON FUNCTION Exec_for_Rows (lvarchar) TO PUBLIC;
--
DROP TABLE Foo;
--
EXECUTE FUNCTION Exec_for_Rows("CREATE TABLE Foo ( A INTEGER, B VARCHAR(32));");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES ( 1, 'Hello' ) ;");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES ( 2, 'Good-Bye' ) ;");
EXECUTE FUNCTION Exec_for_Rows("INSERT INTO Foo VALUES ( 3, 'Stay Here' ) ;");
EXECUTE FUNCTION Exec_for_Rows("SELECT COUNT(*) FROM Foo;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo;");
--
EXECUTE FUNCTION Exec_for_Rows("
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
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 2 ORDER BY A;");
--
EXECUTE FUNCTION Exec_for_Rows("SELECT * FROM Foo WHERE A < 20 ORDER BY A;");

