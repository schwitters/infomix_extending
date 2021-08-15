--
--   File: ExecIt/install/register.sql
--
--  About: 
--
--  This script contains the definitions of the Exec functions.
--
-- DROP FUNCTION Exec ( lvarchar);
-- DROP FUNCTION Exec_for_Rows ( lvarchar);
-- DROP FUNCTION Exec_for_MSet ( lvarchar);
--
CREATE FUNCTION Exec ( lvarchar)
RETURNS LVARCHAR
EXTERNAL NAME 
'$INFORMIXDIR/extend/ExecIt/bin/exec.bld(Exec_to_String)'
LANGUAGE C
DOCUMENT 
" Exec ( lvarchar ) is a dynamic SQL mechanism. The argument is taken to be a ",
" SQL statement, and it is executed in the current server context. For DDL and",
" write SQL queries, the UDR returns a status string. For SQL SELECT queries, ",
" it returns a string that is the public form of an unnamed ROW type with a   ",
" structure that corresponds to the record produced by the query, and data    ",
" from the first row. All subsequent rows are ignored.                        ",
" To get all rows, see the Exec_for_Rows( lvarchar ) ITERATOR function.       ";
--
GRANT EXECUTE ON FUNCTION Exec (lvarchar) TO PUBLIC;
--
--
--
CREATE FUNCTION Exec_For_Rows ( lvarchar)
RETURNS LVARCHAR
WITH ( ITERATOR, STACK = 64 )
EXTERNAL NAME 
'$INFORMIXDIR/extend/ExecIt/bin/exec.bld(Exec_Iterator)'
LANGUAGE C
DOCUMENT 
" Exec_for_Rows ( lvarchar ) is a dynamic SQL mechanism. The argument is taken",
" to be a SQL statement which is executed in the current server context. For  ",
" DDL and write SQL queries, the UDR returns a status string. For SQL SELECT  ",
" statements it returns a series of strings that are the public form of an    ",
" unnamed ROW type with a structure that corresponds to the record produced by",
" the query, and data from the result rows. ";
--
GRANT EXECUTE ON FUNCTION Exec_for_Rows (lvarchar) TO PUBLIC;
--
CREATE FUNCTION Exec_for_MSet ( Arg1 lvarchar )
RETURNS MULTISET( LVarchar NOT NULL)

    DEFINE  msLvRetVal  MULTISET( LVARCHAR NOT NULL );
    DEFINE  lvIter      LVARCHAR;

    FOREACH EXECUTE FUNCTION Exec_for_Rows ( Arg1 ) INTO lvIter
        INSERT INTO Table(msLvRetVal) VALUES ( lvIter );
    END FOREACH;

    RETURN msLvRetVal;
END FUNCTION
DOCUMENT
" Exec_For_MSet ( lvarchar ) -> MULTISET( LVARCHAR NOT NULL ) takes a SQL    ",
" query as its argument and executes that query against the current database.",
" It returns a single MultiSet() object that contains all of the rows the    ",
" query produces; or at least all of the rows it can put into the collection.";
--
GRANT EXECUTE ON FUNCTION Exec_for_MSet ( LVARCHAR ) TO PUBLIC;
