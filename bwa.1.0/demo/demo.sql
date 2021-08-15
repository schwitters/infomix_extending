--
-- Filename:     demo.sql
-- Description:  Demonstrates usage of the bwa DataBlade module
--

--
-- <<<<<<<<<<<<<<<<< Insert data >>>>>>>>>>>>>>>>>>>
--

--
-- Create a table that contains a BWA column
--
create table products (id int, product BWA);

--
-- Insert some rows
--
insert into products values 
(1, "Hats.Baseball.Professional Baseball.SF Giants");

insert into products values 
(2, "Hats.Baseball.Professional Baseball.SD Padres");

insert into products values 
(3, "Hats.Baseball.Professional Baseball.Oakland A's");

insert into products values 
(4, "Shirts.Baseball.Professional Baseball.SF Giants");

insert into products values 
(5, "Shirts.Baseball.Professional Baseball.SD Padres");

insert into products values 
(6, "Shirts.Baseball.Professional Baseball.Oakland A's");

insert into products values 
(7, "Shirts.Dress.Ralph Lauren.Long Sleeve Button-down Collar");

insert into products values 
(8, "Shirts.Casual.Polo.Dark blue w/ red pony");

insert into products values 
(9, "Shirts.Tees.Fruit of the Loom.V-Neck");

insert into products values 
(10, "Shirts.Tees.Fruit of the Loom.Standard"); 

--
-- Part 3 for the Straw Hat is empty.
--
insert into products values (11, "Hats.Sun..Straw");

--
-- But notice that we get an error if we forget to include
-- a placeholding dot for part 3.
--
insert into products values (0, "Hats.Sun.Straw");

--
-- Insert a completely empty product.
--
insert into products values (12, "...");

--
-- We also get an error if any part is too long.
--
insert into products values 
(0, "Part one is way too long, exceeding the max 32 character limit...");

--
-- Output all the data we inserted.
--
select * from products
order by 1;

--
-- Unload the data to a file, delete it, then reload it.
--
unload to "bwa_demo.unl" select * from products;

delete from products;

load from "bwa_demo.unl" insert into products;

select * from products
order by 1;

--
-- <<<<<<<<<<<<<<<<< Search for data >>>>>>>>>>>>>>>>>>>
--

--
-- The BWA operators are not case-sensitive, so the next two 
-- queries return the same result.
--
select * from products
where  product = "Hats.Sun..Straw" 
order by 1;

select * from products
where  product = "hats.sun..straw" 
order by 1;

--
-- Output everything except for the straw hat.
--
select * from products
where  product <> "Hats.Sun..Straw" 
order by 1;

--
-- Equal() considers all parts of a BWA, so the next query 
-- does not return any rows.
--
select * from products
where  product = "hats..." 
order by 1;

--
-- Contains() will find all Hats.
-- Like the BWA operators, Contains() is not case-sensitive.
--
select * from products
where Contains(product, "hats...")
order by 1;

--
-- Find all Straw Hats.
--
select * from products
where Contains(product, "hats...straw")
order by 1;

-- Find all Hats that are not straw
select * from products
where Contains(product, "hats...")
  and not Contains (product, "hats...straw")
order by 1;

--
-- Extract each part separately.
--
select id, 
       product,
       GetCategory(product) category,
       GetFamily(product)   family,
       GetGroup(product)    group,
       GetIdentity(product) identity
from   products
order by 1;

--
-- Use the "Get" UDRs to search for data.
-- These searches are case-sensitive because
-- each "Get" UDR returns a varchar(32) and
-- the built-in Equal() operator for the varchar 
-- type is case-sensitive.
--

select * from products
where GetCategory(product) = "hats"
order by 1;

select * from products
where GetCategory(product) = "Hats"
order by 1;

select * from products
where GetFamily(product) = "Tees"
order by 1;

select * from products
where GetGroup(product) = "Polo"
order by 1;

select * from products
where GetIdentity(product) = "V-Neck"
order by 1;

--
-- We can combine the "Get" UDRs, and we end up
-- with a case-sensitive version of the Contains
-- query.
--
select   * from products
 where   GetCategory(product) = "Hats"
   and   GetIdentity(product) = "Straw"
order by 1;

select * from products
where Contains(product, "hats...straw")
order by 1;


--
-- BWA allows a part to be empty, so sometimes you
-- might want to eliminate empty BWA's from the
-- the result set.
--
-- But the SQL "IS NOT NULL" syntax is not supported
-- and returns the following error:
--
--   293: IS [NOT] NULL predicate may be used only with simple columns.
--

select * from products
where GetIdentity(product) is not null
order by 1;


--
-- The Empty() and NotEmpty() UDRs let you perform operations
-- based on whether any part is empty or filled in.
--
-- Empty(BWA) returns true if any part is empty.
-- NotEmpty(BWA) return true if no parts are empty.
--
-- They return the opposite results, so are called "negator" UDRs.
--

select id, 
       product, 
       Empty(product)     empty, 
       NotEmpty(product)  notempty
from   products
order by 1;

--
-- Return any BWA that has an empty part.
--
select * from products
where    Empty(product)
order by 1;

--
-- Find all hats.
--
select * from products
where Contains(product, "hats...")
order by 1;

--
-- Find all hats, but restrict the search to
-- just the BWA's that are completely filled in.
--
--
select * from products
where Contains(product, "hats...")
  and NotEmpty(product)
order by 1;

--
-- Empty() and NotEmpty() are negator functions
-- so the next query should return opposite results.
--
select * from products
where Contains(product, "hats...")
  and Empty(product)
order by 1;

--
-- Finally, NotEmpty() and NOT Empty() should return the 
-- same results.
--
select * from products
where Contains(product, "hats...")
  and NOT Empty(product)
order by 1;

--
-- Empty(BWA,part) returns true if the specified part is empty.
-- NotEmpty(BWA,part) return true if the specified part is not empty.
--

select id, 
       product, 
       Empty(product, 1)     Part1_empty, 
       NotEmpty(product, 1)  Part1_notempty,
       Empty(product, 2)     Part2_empty, 
       NotEmpty(product, 2)  Part2_notempty,
       Empty(product, 3)     Part3_empty, 
       NotEmpty(product, 3)  Part3_notempty,
       Empty(product, 4)     Part4_empty, 
       NotEmpty(product, 4)  Part4_notempty
from   products
order by 1;

--
-- Return any BWA whose first part is empty.
--
select * from products
where    Empty(product, 1)
order by 1;

--
-- Return hats for any BWA whose third part is not empty.
--
select * from products
where  Contains(product, "hats...")
  and  NOT Empty(product, 3)
order by 1;

select * from products
where  Contains(product, "hats...")
  and  NotEmpty(product, 3)
order by 1;

drop table products;

! \rm -f bwa_demo.unl
