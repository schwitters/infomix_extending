Flat-File Access
----------------

This primary access method (FFAccess) implements a read-only interface to make
external files look like tables in the server. The types supported are:

   boolean, char, date, datetime, decimal, float, int8, integer,
   interval, lvarchar, money, nchar, nvarchar, smallfloat, smallint,
   varchar

This includes any distinct types based on these types.


Accessing an External File
--------------------------

An external file is identified in a CREATE TABLE statement. For example:

CREATE TABLE mytab (
  a BOOLEAN,
  b VARCHAR(20),
  c DATE,
  d DATETIME year to second,
  e DECIMAL,
  f DOUBLE PRECISION,
  g SMALLFLOAT,
  h INT8
) USING FFAccess (path='/tmp/mytab.txt', delim=';');

The last part of the statement identifies the access method used, followed
by arguments to the access method that includes the path to the external file
and the delimiter used in the file to separate columns. The default separator
is ':'.

An external dbspace could also be used as the external file by using the 
"IN" keyword in the CREATE TABLE statement.

Once the table is created it can be used in any SELECT statements including
joining with existing tables.


Directory content
-----------------

ffvti.c   : Source code for the flat-file access method
ffutil.c  : Source code for utility functions for the flat-file access method 
ffvti.def : Exported names for an NT DLL
ffvti.h   : Include file for ffvti.c and ffutil.c
ffvti.sql : SQL script used to create the access method
ffvti_d.sql: SQL script used to remove all traces of the access method
Makefile  : Generic UNIX makefile that creates the shared library
            (Change line 2 identifying the TARGET include file)
README.txt: This file
tab.txt   : Test file
tryit.sql : Test script
WinNT.mak : NT makefile used to create the DLL (requires Visual C++)

Installation
------------
You should install the content of this directory unde $INFORMIXDIR/extend/ffvti
Once the shared library (or DLL) is created, move it from the bin directory to the
top directory ($INFORMIXDIR/extend/ffvti). Then, execute ffvti.sql in the database
that requires this access method. At this point you can create tables that access
external files as shown above.

Testing
-------
To test the access method, you can use the tryit.sql script. You first need to
copy the tab.txt file to the /tmp directory.

Tracing
-------
The source code contains tracing statement. To enable tracing in your current
session, execute the following statement:

EXECUTE FUNCTION set_tracing("ffvti", 50, "$INFORMIXDIR/extend/ffvti/trace.out");

For trace to be active, you must compile the source without the MITRACE_OFF flag

