#!/bin/sh
#
# main.sh 
#
echo " "
echo "main.sh "
echo " "
echo "	Step 1: Create the test database and register the demo DataBlade"
echo " "
setup.sh > setup.out 2>&1
#
# Run the demo.sql script and save the results.
#
echo " "
echo "	Step 2: Run the demo..."
echo " "
rm -f demo.out
dbaccess -e sock2demodb < demo.sql > demo.out 2>&1
echo " "
echo "main.sh complete."
echo " "
echo "Output is in demo.out. Compare against demo.out.sample"
echo " "
