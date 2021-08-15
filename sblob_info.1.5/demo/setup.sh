#!/bin/sh
#
# setup.sh: Create a sample database, register sblob_info.1.5 .
#           Copy the test data to /tmp.
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
drop database SblobDB;
END_DROPDB

echo "	Step 2: Create the demo database" >> setup.out
echo " " >> setup.out
dbaccess -e - - >> setup.out 2>&1 <<END_CREATEDB
create database SblobDB with log;
END_CREATEDB

echo "	Step 3: Register the sblob_info.1.5 DataBlade" >> setup.out
echo " " >> setup.out
blademgr >> setup.out 2>&1 <<END_BLADEMGR
register sblob_info.1.5 SblobDB
y
quit
END_BLADEMGR

echo "	Step 4: Copy the test data to /tmp"  >> setup.out
echo " " >> setup.out
cp data/sblob_info1.txt /tmp
cp data/sblob_info2.txt /tmp

echo "setup complete." >> setup.out
echo " " >> setup.out
