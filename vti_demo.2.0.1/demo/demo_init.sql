-- -------------------------------------------------------------------------
-- Demo script for primary access method demo_am
--
-- Prerequisites:
--                1. vti_demo.2.0.1 install files have been copied to
--                   $INFORMIXDIR/extend/vti_demo.2.0.1
-- Synopsis:
--                1. Creates a table using the demo_am access method.
--                2. Loads some data.
--                3. Issues a variety of select/update/delete statements.
-- -------------------------------------------------------------------------
--
begin work;

-- Create a table of average monthly high temperatures for various Bay Area
-- cities.

create table temperatures
(
    city    char(30),
    month   char(10),
    temp    int
)
using demo_am;

--
-- load some data
--
load from 'vtidemo.dat' insert into temperatures;
-- 
-- Create a table of seasons and the months that they contain. This one
-- will be in the extspace.
--

create table seasons
(
    season    char(15),
    month     char(10)
)
in vtidemo_extspace using demo_am;
--
-- load some data
--

insert into seasons values ('winter', 'December');
insert into seasons values ('winter', 'January');
insert into seasons values ('winter', 'February');

insert into seasons values ('spring', 'March');
insert into seasons values ('spring', 'April');
insert into seasons values ('spring', 'May');

insert into seasons values ('summer', 'June');
insert into seasons values ('summer', 'July');
insert into seasons values ('summer', 'August');

insert into seasons values ('fall', 'September');
insert into seasons values ('fall', 'October');
insert into seasons values ('fall', 'November');

commit work;
