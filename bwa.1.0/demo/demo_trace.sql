--
-- Filename:     demo_trace.sql
-- Description:  Demonstrates how to output UDR entry/exit messages.
--               The following environment variable must be set:
--
--                     setenv DB_LOCALE en_us.8859-1
--
-- <<<<<<<<<<<<<<<<< Turn Tracing On >>>>>>>>>>>>>>>>>>>
--
-- TraceSet_BWA() sets the filename for messages and sets the
-- level for the "BWA" trace class.  It also sets the level of 
-- the built-in __myErrors__ trace class so that any errors will 
-- also be output to the tracefile.

execute procedure TraceSet_BWA("/tmp/bwa.log", 20); 

--
-- If you "tail" /tmp/bwa.log after running TraceSet_BWA, you'll
-- see a message that looks like this:
--
--	Successfully exiting TraceSet_BWA (support.c)

--
-- <<<<<<<<<<<<<<<<< Insert data >>>>>>>>>>>>>>>>>>>
--

--
-- Create a table that contains a BWA column
--
create table products (id int, product BWA);

--
-- Insert a row 
--
insert into products values 
(1, "Hats.Baseball.Professional Baseball.SF Giants");

--
-- Check /tmp/bwa.log to see messages that indicate that
-- BWAInput was entered and exited.
--

insert into products values (11, "Hats.Sun..Straw");

--
-- The next statement generates an error. The error message 
-- gets captured in the trace file because TraceSet_BWA
-- also sets the __myErrors__ trace class level.
--
insert into products values (0, "Hats.Sun.Straw");

--
-- Insert a completely empty product.
--
insert into products values (12, "...");

--
-- Output all the data we inserted. Notice that BWAOutput gets called.
--
select * from products
order by 1;

--
-- Order by the BWA column; notice that both BWAOutput and BWACompare
-- get called. (Internally, all the search operators call BWACompare.)
--
select * from products
order by 2;

--
-- <<<<<<<<<<<<<<<<< Search for data >>>>>>>>>>>>>>>>>>>
--

--
-- You'll notice that Compare is the work horse for this blade.
--
select * from products
where  product = "Hats.Sun..Straw" 
order by 1;

--
-- Contains() will find all Hats.
--
select * from products
where Contains(product, "hats...")
order by 1;

-- Internally, btree calls Compare to order index keys.
-- If you create an index on a lot of data, you'll notice
-- that the btree calls Compare more than once per key value.

create index bwa_ndx1 on products (product);

drop table products;
