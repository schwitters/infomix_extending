
This directory contains a simple test for VTable. The test was built using
KSH scripts on NT, but it has also been run on Solaris.

Prerequisites
-------------

  1. On NT, you will need support for the Korn shell (KSH). The scripts were
     developed using MKS Toolkit.

  2. The Informix server must be up and running.

  3. It is assumed that the VTable runtime files have already been installed
     on the server. ($INFORMIXDIR/extend/VTable/)


Running the test
----------------

  The test runs from a single KSH script: run.ksh

  To run the test under the Korn shell:

    $ run

  To run the test under the C shell:

    % sh run.ksh


Evaluating the output
---------------------

  The test displays some information to the screen, and writes some information
  to the following audit files:

     register.out     - output from registration of VTable to the database
     unregister.out   - output from unregistration of VTable
     run.out          - general output from setup and run of the test
     run1.out         - query output, first pass
     run2.out         - query output, second pass

  If your test ran successfully, run1.out and run2.out will be identical. 


What the test does
------------------

  The test verifies that when a session accesses a versioned table, only the
  rows appropriate to the session's state are visible. It confirms that the
  unregister.sql script successfully removes VTable by re-registering VTable
  to the same database and re-running the query portion of the test.

  The test contains the following steps:

    1. Drop the vtabdb database if it exists, and create a new one.

    2. Register VTable to the new database.

    3. Create a table that will later be versioned, and load it with some
       initial data.

    4. Version the new table.

    5. Add/Change/Delete rows in the versioned table using two different 
       states.  This part of the test is run twice, and is driven by the 
       "run.sql" script.

    6. Unregister VTable.

    7. Delete the base table.

    8. Re-register VTable.

    9. Recreate the test table.

   10. Version the newly recreated table.

   11. Rerun the queries from step #5.


Files
-----

  In addition to the transient files shown above, the test includes the
  following files:

    readme.txt     -- This file.
    run.ksh        -- The driver script.
    run.sql        -- SQL script to exercise VTable routines.
    cleanup.ksh    -- KSH script to drop database and remove transient files.


Note that these tests are fairly cursory, and do not include systematic
checks for error conditions (so-called "negative" tests).

