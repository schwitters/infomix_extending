--
--   File: ExecIt/install/clean.sql
--
--  About: 
--
--    This cleans up from any aborted regression_tests.sql run.
--
--   Clean-up
--
DROP TABLE Foo;
--
DROP TYPE Num RESTRICT;
DROP TYPE Val RESTRICT;
