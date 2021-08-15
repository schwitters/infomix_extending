#!/bin/sh
# setup.sh
#
# Set up for the tests

echo " "
echo "Create the test database"
echo " "
dbaccess - <<END1 >> setup.out 2>&1
create database SqlLibDB with log;
END1

echo " "
echo "Register the bladelet"
echo " "
echo "register SqlLibC.1.1 SqlLibDB" | blademgr >> setup.out 2>&1
echo " "
echo "Create the test tables"
echo " "
dbaccess -e SqlLibDB < setup.sql >> setup.out 2>&1
echo " "

echo "Done."

