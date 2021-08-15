#!/bin/sh
#
# main.sh 
#
echo " "
echo "main.sh "
echo " "
echo "	Step 1: Create the demo database and register the TraceDemo DataBlade"
echo " "
setup.sh > setup.out 2>&1
#
# Run the demo.sh script and display the results.
#
echo " "
echo "	Step 2: Run the demo..."
echo " "
rm -f demo.out
demo.sh > demo.out 2>&1
echo " "
echo "	Step 3: Display the results..."
echo " "
echo "The trace output file contains:"
echo " "
cat /tmp/TraceDemo.trc
echo " "
echo "main.sh complete."
echo " "
