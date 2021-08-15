-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for idn_mrLvarchar UDT
-- Filename        : setup.sql
-- PURPOSE
--    Creates a test table for the idn_mrLvarchar UDT. 
-- 
-- ---------------------------------------------------------------

create table test_mr
(id int, description lvarchar, mrcol idn_mrLvarchar);

--
-- Load up the data
--
load from small_values.txt insert into test_mr;
load from big_values.txt insert into test_mr;   

select id, mrcol from test_mr
order by id;         

-- ---------------------------------------------------------------
-- EOF                                                            
-- ---------------------------------------------------------------
