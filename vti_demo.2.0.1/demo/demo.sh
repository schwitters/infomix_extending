#!/bin/sh
#
# demo.sh
#
echo "Create a table using the demo_am access method and load some data"
dbaccess -e vtidemodb < demo_init.sql

echo "Issue some select statements"
dbaccess -e vtidemodb < demo_select.sql

echo "Update some records"
dbaccess -e vtidemodb < demo_update.sql

echo "Issue the same select statements"
dbaccess -e vtidemodb < demo_select.sql

echo "Done."
