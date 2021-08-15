#!/bin/sh
#
# demo.sh
#
echo "Run the test function..."
echo " "
dbaccess -e idn_LegacyCPPdb << END_TEST
execute function cppTraceOn();
--
execute function cppRoutine(10);
END_TEST
echo " "

