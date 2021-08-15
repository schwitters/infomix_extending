-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for idn_mrLvarchar UDT
-- Filename        : cannothash.sql
-- Description     : Test queries that invoke built-in hash() function
-- 
--
--   idn_mrLvarchar is created with the 'cannothash' modifier, which
--   tells the server that it cannot use the built-in hash function.
--   The hash join directive will be ignored and nested loop used
--   instead. Queries 2-3 will raise exceptions.
--
--   (If you remove the 'cannothash' modifier, these queries return
--   incorrect results.)
--
--   Tests:
--
--      1. hash join
--      2. IN
--	3. COUNT DISTINCT
--      4. GROUP BY
--
--   Additional trace output is to /tmp/mrLvarchar_hash.trc.
-- ---------------------------------------------------------------

--
-- Turn tracing on.
--
execute procedure TraceSet_mrLvarchar ("/tmp/mrLvarchar_hash.trc", 20);

--
-- Set explain on.
--
!rm -f sqexplain.out
set explain on;

--
-- Load data
--
load from small_values.txt insert into test_mr; 
load from big_values.txt insert into test_mr; 
select count(*) from test_mr;

--
-- 1. hash join
--
-- The query plan should show that the server rejects the 
-- USE_HASH directive and does a nested loop join instead.
--
select {+USE_HASH(b)}
       count(*) 
from   test_mr a, test_mr b 
where  a.mrcol=b.mrcol;  

--
-- 2. IN
--
-- This query is expected to fail with the following error:
--
--	  999: Not implemented yet.
--
select count(*) 
from   test_mr
where  mrcol in 
	(select mrcol from test_mr);

--
-- 3. COUNT DISTINCT
--
-- This query is expected to fail with the following error:
--
--	9643: Type (idn_mrlvarchar) is not hashable.
--

select count (distinct mrcol)
from   test_mr;


-- But DISTINCT by itself works because the server calls
-- the UDT's compare routine.

select distinct mrcol
from   test_mr;

--
--      4. GROUP BY
--
-- This query is expected to fail with the following error:
--
--	9643: Type (idn_mrlvarchar) is not hashable.
--
select count(id), mrcol
from   test_mr
group by 2;

--
-- Cleanup
--

delete from test_mr;

-- rename the explain output file to 'sqexplain.hash.log'
!mv -f sqexplain.out sqexplain.hash.log

-- ---------------------------------------------------------------
-- END
-- ---------------------------------------------------------------
