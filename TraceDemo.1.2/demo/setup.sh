#!/bin/sh
#
# setup.sh: Create a demo database, register TraceDemo.1.2, add the trace
# class, and create the TraceSet_TraceDemo() procedure.
#
rm -f setup.out
echo " " > setup.out
echo "setup.sh" >> setup.out
echo " " >> setup.out
echo "	Step 1: Drop the demo database" >> setup.out
echo " " >> setup.out
echo "	Note that this will generate an error message if the database does" >> setup.out 
echo "	not already exist." >> setup.out
echo " " >> setup.out
dbaccess -e - - >> setup.out 2>&1 << END_DROPDB
drop database tracedemodb;
END_DROPDB

echo "	Step 2: Create the demo database" >> setup.out
echo " " >> setup.out
dbaccess -e - - >> setup.out 2>&1 <<END_CREATEDB
create database tracedemodb with log;
END_CREATEDB

echo "	Step 3: Register the TraceDemo.1.2 DataBlade" >> setup.out
echo " " >> setup.out
blademgr >> setup.out 2>&1 <<END_BLADEMGR
register TraceDemo.1.2 tracedemodb
y
quit
END_BLADEMGR

echo " " >> setup.out
echo "	Step 4: Add the trace class"  >> setup.out
echo " " >> setup.out
dbaccess -e tracedemodb - >> setup.out 2>&1 <<END_ADD_TC
insert into systraceclasses(name) values ('TraceDemo');
END_ADD_TC

echo "	Step 5: Create the TraceSet and SetmyErrors SQL procedures" >> setup.out
echo " " >> setup.out
dbaccess -e tracedemodb - >> setup.out 2>&1 << END_ADD_PROC
create procedure TraceSet_TraceDemo
(
        lvarchar,
        int
)
with
(
        not variant
)
external name "$INFORMIXDIR/extend/TraceDemo.1.2/TraceDemo.bld(TraceSet_TraceDemo)"
language c
end procedure;
END_ADD_PROC

echo "setup complete." >> setup.out
echo " " >> setup.out
