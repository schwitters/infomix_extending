#!/bin/sh
#
# cleanup.sh
#
echo "cleanup.sh..."
echo "	... drop the database"
dbaccess - - <<END_DROPDB
drop database vtidemodb;
END_DROPDB
echo "	... delete the temp files"
rm -f demo.out demo.diff
rm -f setup.out
rm -f /tmp/vti_demo.trc
echo "	... remove the extspace"
onspaces -d vtidemo_extspace -y
rm -rf /tmp/VTIDEMODIR
echo " "
echo "Cleanup complete."
