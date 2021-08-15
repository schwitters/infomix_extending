#!/bin/sh
#
# setup.sh: Create a sample database and register vti_demo.2.0.1
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
drop database vtidemodb;
END_DROPDB

echo "	Step 2: Create the demo database" >> setup.out
echo " " >> setup.out
dbaccess -e - - >> setup.out 2>&1 <<END_CREATEDB
create database vtidemodb with log;
END_CREATEDB

echo "	Step 3: Register the vti_demo.2.0.1 DataBlade" >> setup.out
echo " " >> setup.out
blademgr >> setup.out 2>&1 <<END_BLADEMGR
register vti_demo.2.0.1 vtidemodb
y
quit
END_BLADEMGR

echo "	Step 4: Create an extspace" >> setup.out
mkdir /tmp/VTIDEMODIR
onspaces -c -x vtidemo_extspace -l /tmp/VTIDEMODIR >> setup.out 2>&1

echo "	Step 5: Create a trace class and an SQL function to turn on tracing" >> setup.out
dbaccess -e vtidemodb - >> setup.out 2>&1 << END_TC
insert into systraceclasses (name) values ('vti_demo');
END_TC

dbaccess -e vtidemodb - >> setup.out 2>&1 << END_CF
create function vti_demo_traceon(integer)
returning integer;
external name "$INFORMIXDIR/extend/vti_demo.2.0.1/vti_demo.bld(vti_demo_traceon)"
language c
end function;
END_CF

echo "setup complete." >> setup.out
echo " " >> setup.out
