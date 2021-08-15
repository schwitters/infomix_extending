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
# Run the demo.sh script and check the results against a standard.
#
echo " "
echo "	Step 2: Run the demo..."
echo " "
rm -f demo.out
rm -f demo.diff
demo.sh > demo.out 2>&1
echo " "
echo "	Step 3: Check the results..."
diff demo.out demo.std > demo.diff 2>&1
if (test $? -ne 0)
then
	echo " "
	echo "	Demo Failed. See the following files for more info:"
	echo " "
	echo "	  setup.out  - Output from the setup step."
	echo "	  demo.out   - Output from the SQL commands."
	echo "	  demo.diff  - Summary of differences from expected output."
	echo " "
else
	echo " "
	echo "	Demo Succeeded."
	echo " "
	echo " "
	# cleanup.sh > /dev/null 2>&1
fi
echo " "
echo "main.sh complete."
echo " "
