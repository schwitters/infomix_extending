--------------------------------------------------------------------------
-- Name:          create.sql
-- Description:   Creates aesop table and loads sample data set.
--------------------------------------------------------------------------

-- --------------------------------------------------------------------------
-- 1. Create table, insert some data
-- --------------------------------------------------------------------------

create table aesop
(id     int,
 title  varchar(50),
 fable  idn_mrLvarchar
);

load from aesop11.dat insert into aesop;
