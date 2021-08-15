echo off
rem -- 
rem -- This script;
rem -- 
rem --    a. Creates a new database called 'period' with buffered logging.
rem --
rem --    b. Loads the 'Period' BladeLet.
rem --        
rem --    c. Runs the regression tests in dt_test.sql
rem --
rem ------------------------------  CREATE DATABASE -------------------------
rem --
echo "======================================================================"
echo "==                                                                  =="
echo "== a.  Create period with logging and set broad permissions         =="
echo "==                                                                  =="
echo "======================================================================"
rem -- 
dbaccess -e sysmaster Drop.sql
dbaccess -e sysmaster Create.sql
rem --
rem ----------------------------  INSTALL BLADELET -------------------------
rem --
echo "======================================================================"
echo "==                                                                  =="
echo "== b.     Loading the Period BladeLet into period                   =="
echo "==                                                                  =="
echo "======================================================================"
rem --
dbaccess -e period reg_d_period.sql
dbaccess -e period reg_dt_period.sql
rem --
rem ------------------------------  RUN TEST SUITE -------------------------
rem --
echo "======================================================================"
echo "==                                                                  =="
echo "== c.     Running the regression test suite against period          =="
echo "==        Output to files called .\dt_tests.txt and .\d_tests.txt.  =="
echo "==        Check these to determine the validity of any changes.     =="
echo "==                                                                  =="
echo "======================================================================"
rem --
dbaccess -e period < dt_regression_min.sql > dt_tests.txt
dbaccess -e period < d_regression_min.sql > d_tests.txt
rem --
echo "======================================================================"
echo "==                                                                  =="
echo "== d.     Running the statistics and selectivity checks.            =="
echo "==                                                                  =="
echo "======================================================================"
rem --
dbaccess -e period < d_stat_setup.sql > d_stat_tests.txt
dbaccess -e period < dt_stat_setup.sql > dt_stat_tests.txt
rem --
dbaccess -e period < d_stat_queries.sql > d_stat_tests_1.txt
dbaccess -e period < dt_stat_queries.sql > dt_stat_tests_1.txt
rem --
dbaccess -e period < update_stats.sql
rem --
dbaccess -e period < d_stat_queries.sql > d_stat_tests_2.txt
dbaccess -e period < dt_stat_queries.sql > dt_stat_tests_2.txt
rem --
rem --  This tests that the StatPrint() UDF works
rem --
dbschema -hd test_d_period -d period
dbschema -hd test_dt_period -d period
rem --
rem --   Once the script is run, you should check the contents of the
rem --  %INFORMIXDIR$\sqlexpln\[username].out file. The estimates of
rem --  rows returned by each of these queries ought to 
rem --
echo "======================================================================"
echo "==                                                                  =="
echo "== e.     Running the R-Tree checks.                                =="
echo "==                                                                  =="
echo "======================================================================"
rem --
rem --  The idea here is to create an index, and to re-use the
rem -- statistics queries to check that it is working OK.
rem --
dbaccess -e period < create_index.sql 
rem --
dbaccess -e period < d_stat_queries.sql > d_stat_tests_3.txt
dbaccess -e period < dt_stat_queries.sql > dt_stat_tests_3.txt
rem --
rem --    The three files d_stat_tests_[123].txt must be identical, and the
rem --  three files dt_stat_tests_[123].txt must be identical. The differences
rem --  should be reflected in the performance, which is handled separately.
rem --
