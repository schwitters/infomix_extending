#!/bin/sh
#
# setup.sh: Create a sample database and register idn_LegacyCPP.1.2
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
drop database idn_LegacyCPPdb;
END_DROPDB

echo "	Step 2: Create the demo database" >> setup.out
echo " " >> setup.out
dbaccess -e - - >> setup.out 2>&1 <<END_CREATEDB
create database idn_LegacyCPPdb with log;
END_CREATEDB

echo "	Step 3: Register the idn_LegacyCPP.1.2 DataBlade" >> setup.out
echo " " >> setup.out
blademgr >> setup.out 2>&1 <<END_BLADEMGR
register idn_LegacyCPP.1.2 idn_LegacyCPPdb
y
quit
END_BLADEMGR

echo "setup complete." >> setup.out
echo " " >> setup.out
