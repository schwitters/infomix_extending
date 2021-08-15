#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database idn_LegacyCdb;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out
rm -f setup.out
rm -f /tmp/LegacyC.trc
echo " "
echo "Cleanup complete."
