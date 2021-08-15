#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database sock2demodb;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out
rm -f setup.out
rm -f /tmp/idn_sockserver.log
rm -f /tmp/idn_sockclient.log
echo " "
echo "Cleanup complete."
