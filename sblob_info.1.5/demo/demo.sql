-- -------------------------------------------------------------------------
-- Script for the sblob_info demo.
--
-- Prerequisite:
--                sblob_info.1.5 install files have been copied to
--                $INFORMIXDIR/extend
-- Synopsis:
--                Calls server functions to operate on sblobs.
-- -------------------------------------------------------------------------
--
-- Drop and create a test table, and load it with a couple of rows.
drop table sblob_test;

create table sblob_test (description lvarchar, blob_column blob);

insert into sblob_test values
(
  'stdio.h: standard i/o header', 
  FileToBlob('/tmp/sblob_info1.txt', 'server')
);

insert into sblob_test values
(
  'stdlib.h: standard library header', 
  FileToBlob('/tmp/sblob_info2.txt', 'server')
);

-- Turn on tracing.
execute procedure SblobDebug(30);

--
-- SblobStatSize() determines the size of a smart blob...
select description, blob_column, SblobStatSize(blob_column)
from sblob_test;

--
-- SblobSbspace() reports the name of the sbspace where the sblob is stored...
select description, blob_column, SblobSbspace(blob_column)
from sblob_test;

--
-- SblobMaxSize() if a max size was specified when the sblob was created,
-- then this routine reports that size. (-1 if none specified)
select description, blob_column, SblobMaxSize(blob_column)
from sblob_test;

--
-- SblobEstSize() reports the "estimated size" allocated for the sblob.
select description, blob_column, SblobEstSize(blob_column)
from sblob_test;

--
-- SblobExtSize() reports the extent size specified for the sblob.
select description, blob_column, SblobExtSize(blob_column)
from sblob_test;

-- SblobLog() reports whether the user data portion of the sblob is logged (T)
-- or not (F).
select description, blob_column, SblobLog(blob_column)
from sblob_test;

--
-- SblobCreate() creates an sblob, and allows you to specify some of its
-- attributes.
insert into sblob_test values
(
  'create a 1500 byte sblob',
  SblobCreate("default", 1500, "nolog", -1, -1)
);

select  description,
        SblobStatSize(blob_column) as statsize,
        SblobSbspace(blob_column)  as sbspace,
        SblobEstSize(blob_column)  as estimated_size,
        SblobMaxSize(blob_column)  as max_size
from    sblob_test
where   description="create a 1500 byte sblob";

insert into sblob_test values
('1024 byte sblob',
SblobCreate("default", 1024, "log", 2048, 4096));

select  description,
        SblobStatSize(blob_column) as statsize,
        SblobSbspace(blob_column)  as sbspace,
        SblobEstSize(blob_column)  as estimated_size,
        SblobMaxSize(blob_column)  as max_size
 from    sblob_test
 where   description="1024 byte sblob";

--
-- SblobUpdate() updates an sblob. You can append or replace existing data.
insert into sblob_test values
('foo', SblobCreate("default", 1024, "log", 2048, 4096));

-- Add 2000 bytes.
select SblobUpdate(blob_column, 2000, "append")
from    sblob_test
where   description="foo";

-- The resulting sblob should be 3024 bytes.
select  description,
        SblobStatSize(blob_column) as statsize,
        SblobSbspace(blob_column)  as sbspace,
        SblobEstSize(blob_column)  as estimated_size,
        SblobMaxSize(blob_column)  as max_size,
        SblobLog(blob_column)      as logged
from    sblob_test
where   description="foo";

-- Replace with 1800 bytes.
select SblobUpdate(blob_column, 1800, "truncate")
from    sblob_test
where   description="foo";

-- The resulting sblob should be 1800 bytes.
select  description,
        SblobStatSize(blob_column) as statsize,
        SblobSbspace(blob_column)  as sbspace,
        SblobEstSize(blob_column)  as estimated_size,
        SblobMaxSize(blob_column)  as max_size,
        SblobLog(blob_column)      as logged
from    sblob_test
where   description="foo";

--
-- SblobRefCount() reports how many database columns refer to an LO handle.
create table sblob_tab (blob_column blob);

insert into sblob_tab values
( FileToBlob('/tmp/sblob_info1.txt', 'server'));

-- our brand-new sblob has one reference to it:
select SblobRefCount(blob_column)
from sblob_tab;

-- create another reference:
select blob_column from sblob_tab
into temp sblob_temp_tab;

-- now it should have 2 references:
select SblobRefCount(blob_column)
from sblob_tab;

-- Done.
