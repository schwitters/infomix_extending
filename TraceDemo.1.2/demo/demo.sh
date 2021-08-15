#!/bin/sh
#
# demo.sh
#
echo "Turn on tracing, then call the UpOne function."
dbaccess -e tracedemodb < demo.sql >> demo.out 2>&1

echo "Done."
