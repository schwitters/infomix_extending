
-- SQLNumbers will hold numeric data for testing the ceil() and
-- sign() functions.
create table SQLNumbers
(
  decimalColumn decimal,
  integerColumn integer,
  int8Column int8,
  realColumn real,
  smallintColumn smallint,
  doubleColumn double precision
);

-- insert some simple values: +, -, and 0
insert into SQLNumbers
values (123.456, 123, 123456, 123.456, 123, 12345.6789);

insert into SQLNumbers
values (-123.456, -123, -123456, -123.456, -123, -12345.6789);

insert into SQLNumbers
values (0, 0, 0, 0, 0, 0);

-- insert some big positive values
insert into SQLNumbers
(
  decimalColumn,
  integerColumn,
  int8Column,
  realColumn,
  smallintColumn,
  doubleColumn
)
values 
(
  1234567890123456.1234567890123456, -- max default precision (16,16)
  2147483647,                        -- max integer
  9223372036854775807,               -- max int8
  1234567.8,                         -- 8 significant digits
  32767,                             -- max smallint
  1234567890.123456                  -- 16 significant digits
);

-- insert some big negative values
insert into SQLNumbers
(
  decimalColumn,
  integerColumn,
  int8Column,
  realColumn,
  smallintColumn,
  doubleColumn
)
values 
(
  -1234567890123456.1234567890123456, -- max default precision (16,16)
  -2147483647,                        -- max integer
  -9223372036854775807,               -- max int8
  -1234567.8,                         -- 8 significant digits
  -32767,                             -- max smallint
  -12345678901234.56                  -- 16 significant digits
);

-- --------------------------------------------------------------------------
-- Create a table for testing ascii() and char()
--
-- What we'll do is select a literal and the result of an "execute function"
-- call from this one-row table.
-- --------------------------------------------------------------------------
--
create table OneVal (col1 integer);
insert into OneVal values (1);

