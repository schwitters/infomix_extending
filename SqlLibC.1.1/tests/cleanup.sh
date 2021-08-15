#!/bin/sh
# cleanup.sh
#
# Drop the test database and remove the test output files
dbaccess - <<END
drop database SqlLibDB;
END

rm -f setup.out run.out

