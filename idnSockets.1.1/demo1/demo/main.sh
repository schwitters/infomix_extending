#!/bin/sh
#
# main.sh 
#
# minimal syntax check:
case $# in
  0) echo " "
     echo "Usage: $0 server-port-number"
     echo " "
     echo "Be sure the socket server is running. At startup, it will tell you"
     echo "which port it is running on. Use that port number as the command "
     echo "line argument for this script."
     echo " "
     echo "Example:"
     echo " "
     echo "	% $0 1234"
     echo " "
     exit 1
esac

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
# Put the port number in demo.sql
#
sed -e "s/SERVER_PORT/$1/g" demo.tpl > demo.sql
#
echo "execute the function, passing it a variety of input strings"
dbaccess -e sock1demodb < demo.sql > demo.out
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
