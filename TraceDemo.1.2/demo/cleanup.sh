#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database tracedemodb;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out
rm -f setup.out
rm -rf /tmp/TraceDemo.trc
echo " "
echo "Cleanup complete."
