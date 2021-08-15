#!/bin/sh
#
# demo.sh
#
echo "Run the test function..."
echo " "
dbaccess -e idn_LegacyCdb << END_TEST
execute function cTraceOn();
--
execute function cRoutine(10);
END_TEST
echo " "

