# <font color="0000FF">Exec BladeLet</font>

* * *

<big>

<center>_The Exec BladeLet is a UDR that provides dynamic SQL functionality within an SPL procedure. It is a pair of user-defined functions -- a simple UDF and an Iterator -- that take arbitrary SQL statements, execute them, and return a result to whatever invoked it in the first place. This BladeLet is also a useful example of how to issue and handle SQL queries using SAPI._</center>

</big>

## <font color="0000FF"><a name="contents">Contents</a></font>

> *   [Introduction and Overview.](#first)
> *   [Design Details.](#second)
> *   [List of User-Defined Functions.](#third)
> *   [Glossary](#glossary)

* * *

### <a name="first"><font color="0000FF">Introduction and Overview.</font></a>

> Often, it is desirable to execute a SQL query that is generated at run-time within the ORDBMS. For example, a developer may not know the name of the temporary table they wish to run the query against, or they might want to append predicates to a query. In external programs, this can be accomplished using the ESQL/C SQLCA and DESCRIPTOR facilities.

> Unfortunately the INFORMIX Stored Procedure Language (SPL) does not support dynamic SQL. Queries must be hard-coded into the SPL logic.

> The objective of the Exec BladeLet is to remedy this. Exec consists of some user-defined functions (UDFs) that take a SQL query as an argument, execute it, and return a result (the format of which varies depending on the function and the kind of query). The Exec functions can handle most Data Definition Language (DDL) statements, and all Data Manipulation Language (DML) queries.

[Contents](#contents)

* * *

### <a name="second"><font color="0000FF">Design Details.</font></a>

> There are three UDRs in the Exec BladeLet. Two of them are 'C' EXTERNAL FUNCTIONS that use the Server API (SAPI). These must be compiled into shared libraries on the target machine, and you need to declare them to the server using CREATE FUNCTION statements (which are shipped as part of the BladeLet). The third UDF is an SPL routine that uses the first two UDFs to do useful things; implement a general UDF that returns a MULTISET of rows. It is intended as an example.

> The bulk of the 'C' code is to be found in ./src/exec.c and ./src/exec/h. This wad included makefiles for Unix and NT. The SQL registration script is ./install/register.sql, and there is a ./install/regression_tests.sql file that you can use to test any changes that you may be considering. Exec is a rather simpler beast than other Blades: there are no errors to register (all errors are generated directly from the code), and there is an explanation of each UDF's functionality included as part of the CREATE FUNCTION.

> An important part of the Bladelet's code is the large scale testing you can do using the scripts in ./install. Whenever you change anything in this BladeLet, I recommend running the regression_tests.sql against an engine with several CPU-VPs active. While this is completing, set off a couple of mu_tests.sql in parallel.

> The reason for this is that one of the trickiest aspects of developing this blade has been getting the memory management code correct. The Iterator function must hold memory allocated within SAPI between calls. This means that I must run most of the code at PER_COMMAND memory duration, which makes memory leaks a problem. Using a tail on the online.log, check for the allocation of additional memory segements. At the moment, the code is clean. Change with caution.

[Contents](#contents)

* * *

### <a name="third"><font color="0000FF">List of User-Defined Functions.</font></a>

*   <font face="Courier">**EXEC ( LVARCHAR ) RETURNS LVARCHAR**</font>

> This function takes an LVARCHAR that it treats as a SQL query. It executes the query an returns a single, LVARCHAR result string. Depending on what kind of SQL statement is submitted, Exec() returns a different result format.

> If the query is a Data Declaration Language expression (DDL) then Exec() either returns a string "OK", or it will generate an exception. For example:

> <font face="Courier">SELECT Exec("  
> CREATE TABLE Foo ( A Num, B Val, C SET(INTEGER NOT NULL));  
> ") FROM TABLE(SET{1});  
>   
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>OK</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
> </font>

> Data Manipulation Language (DML) expressions (more typically referred to as _queries_) return different result formats depending on whether they are <font face="COURIER">**SELECT**</font> queries or write queries (<font face="COURIER">**INSERT**</font>, <font face="COURIER">**UPDATE**</font> or <font face="COURIER">**DELETE**</font>).

> Write queries either return a single result string which indicates how many rows were affected by the query, or they generate a SQL error. For example:

> <font face="Courier">SELECT Exec("  
> INSERT INTO Foo VALUES (1,'Hello',SET{1,2,3});  
> ") FROM TABLE(SET{1});  
>   
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>1 rows affected</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> SELECT Exec("  
> INSERT INTO Foo VALUES (2,'Good-bye',SET{4,5,6});  
> ") FROM TABLE(SET{1});  
>   
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>1 rows affected</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> SELECT Exec("  
> UPDATE Foo SET B = 'Zap!' WHERE A < 4;  
> ") FROM TABLE(SET{1});  
>   
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>2 rows affected</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> 
> <pre>EXECUTE FUNCTION Exec("
> INSERT INTO Foo
> SELECT ( T1.Num * 100 + T2.Num * 10 + T3.Num )::Integer::Num,
>        ( T1.Val || ',' || T2.Val || ',' || T3.Val )::Val,
>        SET{ T1.Num, T2.Num, T3.Num }
>   FROM TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
>                  ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
>                  ROW(8,'Eight'),ROW(9,'Nine')
>                 }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T1,
>        TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
>                  ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
>                  ROW(8,'Eight'),ROW(9,'Nine')
>                 }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T2,
>        TABLE(SET{ROW(0,'Zero'),ROW(1,'One'),ROW(2,'Two'),ROW(3,'Three'),
>                  ROW(4,'Four'),ROW(5,'Five'),ROW(6,'Six'),ROW(7,'Seven'),
>                  ROW(8,'Eight'),ROW(9,'Nine')
>                 }::SET(ROW(Num INTEGER, Val LVARCHAR) NOT NULL)) T3;");
> </pre>
> 
>   
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>1000 rows affected</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> </font>

> <font face="COURIER">**SELECT**</font> queries submitted through Exec return a single LVARCHAR result that is the public format of an unnamed ROW TYPE corresponding to the format of the query's return result. Such a string can be cast into a named ROW TYPE and then handled intelligently by the rest of the calling program.

> Because it is a simple UDF, Exec() can only return a single result. To return more than one row you need to use the Exec_For_Rows() UDF (see below.) If the query produces more than one row, Exec() returns as its result data from the very first row. Then it terminates the query.

> <font face="COURIER">
> 
> <pre>SELECT Exec("SELECT COUNT(*) FROM Foo;") 
>   FROM TABLE(SET{1});
> </pre>
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1003.00000000)</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> 
> <pre>SELECT Exec("SELECT * FROM Foo;") 
>   FROM TABLE(SET{1});
> </pre>
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1,'Zap!',SET{1,2,3})</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> </font>

> Although Exec() returns a LVARCHAR result, this can easily be cast into a named ROW TYPE with an equivalent structure. This is likely to be very valuable in most practical applications.

> <font face="COURIER">
> 
> <pre>
> CREATE ROW TYPE Named_Row_Type (
> 	A	INTEGER,
> 	B	VARCHAR(32),
> 	C	SET(INTEGER NOT NULL)
> );
> 
> SELECT Exec("SELECT * FROM Foo WHERE A = 3;")::Named_Row_Type
>   FROM TABLE(SET{1});
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1,'Zap!',SET{1,2,3})</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
> </pre>
> 
> </font>

> Note that although these two examples appear identical the first is simply a string, while the result of the second query is in fact an instance of the Named_Row_Type ROW TYPE (which is then turned back into a string). When using the two UDFs in another SPL routine, this is very useful.

> As with any UDF, the Exec() function can be included in a SQL query's WHERE or SELECT list, or you can EXECUTE FUNCTION on it directly. Note that there are certain limits on the DROP statement, which cannot generally be issued inside a SELECT query, and that write queries in a SELECT statement have odd consequences on transactions. **Use these with caution!**

*   <font face="Courier">**EXEC_FOR_ROWS ( LVARCHAR ) RETURNS LVARCHAR WITH ( ITERATOR )**</font>

> The Exec_For_Rows() UDF is an Iterator, which means that it can return more than one result row. Of course, it only does so when it is asked to execute a <font face="COURIER">**SELECT**</font>. Otherwise, it behaves exactly as the Exec() UDF. For example:

> <font face="COURIER">
> 
> <pre>EXECUTE FUNCTION Exec_For_Rows("SELECT * FROM Foo WHERE A IN ( 1,2,3,4);") ;
> </pre>
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1,'Zap!',SET{1,2,3})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(2,'Zap!',SET{4,5,6})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(3,'Stay Here',SET{7,8,9})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1,'Zero,Zero,One',SET{0,1})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(2,'Zero,Zero,Two',SET{0,2})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(3,'Zero,Zero,Three',SET{0,3})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(4,'Zero,Zero,Four',SET{0,4})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(1,'Zero,Zero,One',SET{0,1})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(2,'Zero,Zero,Two',SET{0,2})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(3,'Zero,Zero,Three',SET{0,3})</td>
> 
> </tr>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>ROW(4,'Zero,Zero,Four',SET{0,4})</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
>   
> </font>

> Of course, being an iterator limits the ways in which such a UDR can be used. It can't be used in another SQL query, for example. In fact, about the only place it can be used (at the moment) is inside an SPL routine. Mind you, you can do a lot with it there. And shortly you will be able to put Iterators in the FROM clause of a query, so that ought to help.

*   <font face="Courier">**EXEC_FOR_MSET ( LVARCHAR ) RETURNS MULTISET(LVARCHAR NOT NULL)**</font>

> The Exec_for_MSet() UDF is an SPL routine that uses the Exec_For_Rows() UDF introduced above. Instead of returning a set of rows as an iterator, or a single row as the Exec() UDF does, this UDF collects the results of the SQL query together into a single object: a multi-set.

> <font face="COURIER">
> 
> <pre>CREATE FUNCTION Exec_for_MSet ( Arg1 lvarchar )
> RETURNS MULTISET( LVarchar NOT NULL)
> 
>     DEFINE  msLvRetVal  MULTISET( LVARCHAR NOT NULL );
>     DEFINE  lvIter      LVARCHAR;
> 
>     FOREACH EXECUTE FUNCTION Exec_for_Rows ( Arg1 ) INTO lvIter
>         INSERT INTO Table(msLvRetVal) VALUES ( lvIter );
>     END FOREACH;
> 
>     RETURN msLvRetVal;
> END FUNCTION
> 
> EXECUTE FUNCTION Exec_For_Mset ( " SELECT DISTINCT A FROM Foo WHERE A < 20 AND 7 IN C; " );
> 
> <table border="1">
> 
> <tbody>
> 
> <tr>
> 
> <td>(expression)</td>
> 
> <td>MULTISET{'ROW(3)','ROW(7)','ROW(17)'}</td>
> 
> </tr>
> 
> </tbody>
> 
> </table>
> 
> </pre>
> 
> </font>

> Why is this interesting? Well for one thing, it lets you create a new data type called SQL (which is simply a distinct type of LVARCHAR). The results of Exec_For_Mset() run over a SQL query is a "relation" too. This raises all kinds of intriguing design possibilities.

[Contents](#contents)

* * *

### <a name="glossary"><font color="0000FF">Glossary</font></a>

> Terms and acronyms used by this tech note include:
> 
> > <table border="0" cellpadding="5">
> > 
> > <tbody>
> > 
> > <tr>
> > 
> > <td>Blade</td>
> > 
> > <td>Set of semantically related extensions to the ORDBMS.</td>
> > 
> > </tr>
> > 
> > <tr>
> > 
> > <td>COLLECTION</td>
> > 
> > <td>Non-first normal form object. That is, a set of data values that can be considered as a single data value for some purposes (variables). COLLECTIONS can also be thought of as small, in-memory, temporary tables for the purpose of querying.</td>
> > 
> > </tr>
> > 
> > <tr>
> > 
> > <td>Iterator</td>
> > 
> > <td>An iterator is a special kind of UDF that returns more than one result. Implementing Iterators raises conceptual and engineering difficulties. This Bladelet contains an example of a quite complex Iterator.</td>
> > 
> > </tr>
> > 
> > <tr>
> > 
> > <td>User-defined Function (UDF)</td>
> > 
> > <td>Module of procedural logic that extends SQL. This Bladelet included UDFs implemented in 'C' and SPL. Through out this document I use the term Routine synonymously with UDF.</td>
> > 
> > </tr>
> > 
> > </tbody>
> > 
> > </table>

[Contents](#contents)

* * *

Last updated 31-March-2000\.