#!/bin/sh
#
# main.sh 
#
echo " "
echo "********************************************************************"
echo "main.sh "
echo " "
echo "	Step 1: Create the test database and register the demo DataBlade"
echo " "
setup.sh > setup.out 2>&1
#
# Run the demo.sh script and send the results to an output file.
#
echo "********************************************************************"
echo " "
echo "	Step 2: Run the demo..."
echo " "
rm -f demo.out
#
echo "execute the function, passing it a variety of input strings"
dbaccess -e fifodb < demo.sql > demo.out
echo " "
echo "********************************************************************"
echo "	Step 3: Display the results..."
echo " "
cat demo.out
echo "********************************************************************"
echo " "
echo "To clean up, run the cleanup.sh script."
echo " "
echo "main.sh complete."
echo " "
