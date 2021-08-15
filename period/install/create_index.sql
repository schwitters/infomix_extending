--
--   File:   create_index.sql
--
--  About:
--
--    This script creates an index on the Period column. Subsequently,
--  you ought to re-run the queries in the test workload to determine
--  whether it is OK.
--
CREATE INDEX Test_Index_1 ON Test_D_Period ( When Period_ops ) USING RTREE;
CREATE INDEX Test_Index_2 ON Test_DT_Period ( When DT_Period_ops ) USING RTREE;
--
--  
--
