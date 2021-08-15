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
# Run the demo.sh script
#
echo " "
echo "	Step 2: Run the demo..."
echo " "
rm -f demo.out
demo.sh > demo.out 2>&1
echo " "
echo "	Step 3: Check the results..."
echo " "
echo "	The demo produced the following output:"
echo " "
cat /tmp/LegacyC.trc
echo " "
echo " "
echo "Compare against the LegacyC.trc file in this directory:"
echo " "
cat ./LegacyC.trc
echo " "
echo "main.sh complete."
echo " "
