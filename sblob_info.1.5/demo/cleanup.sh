#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database SblobDB;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out demo.diff
rm -f setup.out
rm -f /tmp/sblob_info.trc
echo " "
echo "Cleanup complete."
