-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for idn_mrLvarchar UDT
-- Filename        : in-row.sql
-- Description     : Sanity tests for small values.
-- 
--   Tests:
--
--      1. Input / Output / Copy
--      2. Assign
--	3. Length
--      4. Sbspace
--      5. Refcount
--      6. LOHandles
--      7. Update
--      8. Concat
--      9. Delete
--     10. Import/Export
--     11. Equal/NotEqual (+ rest of the search ops)
--     12. Order by, distinct (Compare)
--     13. Send/Receive
--     14. Clip
--     15. B-tree functional index
--     16. ConcatAll aggregate: including insert using result
--     17. Union query
--
--   Additional trace output is to /tmp/in-row.trc.
-- ---------------------------------------------------------------

--
-- Turn tracing on.
--
execute procedure TraceSet_mrLvarchar ("/tmp/in-row.trc", 20);

--
-- Set explain on.
--
!rm -f sqexplain.out
set explain on;

--
-- 1. Input / Output / Copy
--
-- Calling the Input routine directly executes the Input
-- routine to convert the text value into the UDT, then
-- executes the Output routine to display the result.

execute function idn_mrLvarcharIn("here is a small value");

-- we can determine the intermediate result is not a smart blob
-- by running lohandles() on the result, which returns 0 for
-- non-sblobs.

execute function lohandles(idn_mrLvarcharIn("here is a small value"));

--
-- Now call our function that makes a copy of the UDT value.
--

select DeepCopy(idn_mrLvarcharIn("here is a small value"))
from systables
where tabid=1;

--
-- 2. Assign
--
-- Inserting a value into a table executes the Input routine
-- to convert the text value to the UDT, then the Assign
-- routine just before it is written to the table.
insert into test_mr values (0, "in-row.sql", "here is a small value");

select * from test_mr where id=0;

--
-- 3. Length: should be data size + 8 bytes UDT header
--
select id, Length(mrcol) from test_mr where id=0;

--
-- 4. Sbspace: should return NULL for an in-row value
--
select id, Sbspace(mrcol) from test_mr where id=0;

--
-- 5. Refcount: should return NULL for an in-row value
--
select id, Refcount(mrcol) from test_mr where id=0;

--
-- 6. LOHandles: should return 0 for an in-row value
--
select id, LOHandles(mrcol) from test_mr where id=0;

--
-- 7. Update: trace file will show input/assign for the 
--            new value and a destroy for the old value.
--
update test_mr 
set mrcol = "here is a modified small value"
where id = 0;

select * from test_mr where id=0;

--
-- 8. Concat
--
select id, concat(mrcol,mrcol) from test_mr where id=0;

--
-- use the concat operator
--
select id, mrcol || ' and here is some more' from test_mr where id=0;

select id, Concat(mrcol, ': the end')
from   test_mr
where  Concat(mrcol, ': the end') = "here is a modified small value: the end";

--
-- update the table based on the concat result
--
update test_mr
     set mrcol = mrcol || ' and here is some more' 
where id=0;

select * from test_mr where id=0;

--
-- 9. Delete
--
delete from test_mr where id=0;

select * from test_mr where id=0;

--
-- 10. Import/Export
--
load from small_values.txt insert into test_mr;

select * from test_mr order by 1;

-- naming the output a .log means cleanup will remove it
unload to small_valTEST.log select * from test_mr;

--
-- 11. Equal/NotEqual
--

select id, mrcol from test_mr where mrcol = 'three';
select id, mrcol from test_mr where mrcol <> 'three' order by id;
select id, mrcol, 'one' > mrcol from test_mr order by id;
select id, mrcol, 'one' >= mrcol from test_mr order by id;
select id, mrcol, 'one' < mrcol from test_mr order by id;
select id, mrcol, 'one' <= mrcol from test_mr order by id;

--
-- 12. Order by, distinct (Compare)
--
select * from test_mr order by mrcol;

select distinct mrcol 
from   test_mr
order  by mrcol;

--
-- 13. Send/Receive
--

--
-- lengths should be the same
--
select id, 
       mrcol,
       length(mrcol) mrcol_len,
       idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol)) sendrecv_result,
       length(idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) sendrecv_len
from   test_mr
order  by 1;

--
-- The "comparison" result should be 0
--
select id, 
       mrcol,
       idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol)) sendrecv_result,
       Compare(mrcol,idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) comparison
from   test_mr
order  by 1;

--
-- The "equality" result should be 't'
--
select id, 
       mrcol,
       idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol)) sendrecv_result,
       Equal(mrcol,idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) equality
from   test_mr
order  by 1;

--
-- 14. Clip
--

--
-- We need a larger entry that is still in-row.
--
insert into test_mr 
values (
6, 
'in-row.sql', 
'The Mouse, the Frog, and the Hawk: A MOUSE who always lived on the land, by an unlucky chance formed an intimate acquaintance with a Frog, who lived for the most part in the water.  The Frog, one day intent on mischief, bound the foot of the Mouse tightly to his own.  Thus joined together, the Frog first of all led his friend the Mouse to the meadow where they were accustomed to find their food.  After this, he gradually led him towards the pool in which he lived, until reaching the very brink, he suddenly jumped in, dragging the Mouse with him.  The Frog enjoyed the water amazingly, and swam croaking about, as if he had done a good deed.  The unhappy Mouse was soon suffocated by the water, and his dead body floated about on the surface, tied to the foot of the Frog.  A Hawk observed it, and, pouncing upon it with his talons, carried it aloft.  The Frog, being still fastened to the leg of the Mouse, was also carried off a prisoner, and was eaten by the Hawk.  (Moral: Harm hatch, harm catch.)');

select id, 
       length(mrcol) mrcol_len 
from   test_mr;

select id, 
       clip(mrcol) mrcol_clipped 
from   test_mr;

select id, 
       length(clip(mrcol)) clipped_len
from   test_mr;

--
-- 15. B-tree functional index
--
-- Clip returns a varchar(255), so it can participate in
-- a B-tree index.
--

create index mrndx on test_mr(clip(mrcol));

drop index mrndx;

--
-- 16. ConcatAll aggregate: including insert using result
--
delete from test_mr;

insert into test_mr values (1, "in-row.sql", "one");
insert into test_mr values (2, "in-row.sql", "two");
insert into test_mr values (3, "in-row.sql", "three");
insert into test_mr values (4, "in-row.sql", "four");
insert into test_mr values (5, "in-row.sql", "five");

select id, mrcol from test_mr;

select ConcatAll(mrcol) concat_result
from   test_mr;

--
-- 17. Union query
--
select ConcatAll(mrcol) concat_result
from   test_mr
where  id < 3
union
select ConcatAll(mrcol) concat_result
from   test_mr
where  id > 3
order by 1;

insert into test_mr values
( (select max(id)+1 from test_mr),
  "in-row.sql",
  (select ConcatAll(mrcol) from test_mr)
);

select * from test_mr
order by id;

--
-- Cleanup
--

delete from test_mr;

-- rename the explain output file to 'sqexplain.in-row.log'
!mv -f sqexplain.out sqexplain.in-row.log

-- ---------------------------------------------------------------
-- END
-- ---------------------------------------------------------------
