#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database sock1demodb;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out demo.diff
rm -f setup.out
rm -f demo.sql
echo " "
echo "Cleanup complete."
