-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for UDT BWA
-- Filename        : compare.sql
-- Author          : generated by DBDK
-- PURPOSE
--    Tests compare routine for UDT BWA
-- 
-- ---------------------------------------------------------------

-- start transaction
begin work;


-- call support routine directly
select	a.id, a.value, 
	b.id, b.value,
	Compare(a.value,b.value)	compare
  from	tst_BWA a, tst_BWA b
     ;
  
--
-- test Compare, Equal and NotEqual support functions 
-- through selects
-- be aware, that the qeury optimizer may decide not
-- to use them!
--
select * from tst_BWA;
  
select distinct value from tst_BWA;
  
select * from tst_BWA order by value;
  

-- end transaction
rollback work;

-- ---------------------------------------------------------------
-- EOF                                                            
-- ---------------------------------------------------------------