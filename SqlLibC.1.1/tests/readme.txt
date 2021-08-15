
This directory contains test scripts for the SqlLib Bladelet. The scripts
are written for the Bourne Shell.

To run the tests:

  0. Install the bladelet in $INFORMIXDIR/extend/SqlLibC.1.1

  1. Run the setup.sh script, and check the "setup.out" file for errors.
  2. Run the run.sh script, and check "run.out" for errors.



  setup.sh   - creates the test database, registers the bladelet, and
                builds/loads test tables. Calls setup.sql

  run.sh     - runs the tests, sending the output to run.out. Calls:

               -> testceil.sql
               -> testchar.sql
               -> testinstr.sql
               -> testsign.sql

  run.std     - Expected output from run.sh

  cleanup.sh - Drops the test database and removes the test output files.

