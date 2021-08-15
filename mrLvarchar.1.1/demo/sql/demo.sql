--------------------------------------------------------------------------
-- Name:          demo.sql
-- Description:   Demonstrates usage of the mrLvarchar DataBlade module.
--
-- This demo script expects that you have already created the table and
-- loaded the data by running ../data/create.sql.
--
-- Next it does the following:
--
--    1. Sets trace on 
--    2. Basic operations ('order by', equal / not equal, concat, clip)
--    3. Admin info (Length, IsSmall, IsLarge, Sbspace)
--    4. String functions (Upper, Lower, Regexp_Match, Regexp_Replace, Instr,
--       Instrb, Snip)
--    5. Queries that are expected to fail
--------------------------------------------------------------------------

-- --------------------------------------------------------------------------
-- 1. Uncomment to turn trace on
-- --------------------------------------------------------------------------
-- execute procedure TraceSet_mrLvarchar ("/tmp/mrdemo.trc", 20);

-- --------------------------------------------------------------------------
-- 2. Basic operations ('order by', 'distinct', equal / NotEqual, 
--    concat, clip)
-- --------------------------------------------------------------------------

--
-- 'order by' the idn_mrLvarchar column.
--

select id, fable
from   aesop
where  id < 10
order by fable;

--
-- 'distinct'
--
select distinct fable 
from   aesop
where  id < 10;

--
-- Equal / NotEqual operators 
--
select id from aesop
where fable =
 'A TRAVELER hired an Ass to convey him to a distant place.  The day being intensely hot, and the sun shining in its strength, the Traveler stopped to rest, and sought shelter from the heat under the Shadow of the Ass.  As this afforded only protection for one, and as the Traveler and the owner of the Ass both claimed it, a violent dispute arose between them as to which of them had the right to the Shadow.  The owner maintained that he had let the Ass only, and not his Shadow.  The Traveler asserted that he had, with the hire of the Ass, hired his Shadow also.  The quarrel proceeded from words to blows, and while the men fought, the Ass galloped off.  In quarreling about the shadow we often lose the substance.'
;

select id from aesop
where fable <>
 'A TRAVELER hired an Ass to convey him to a distant place.  The day being intensely hot, and the sun shining in its strength, the Traveler stopped to rest, and sought shelter from the heat under the Shadow of the Ass.  As this afforded only protection for one, and as the Traveler and the owner of the Ass both claimed it, a violent dispute arose between them as to which of them had the right to the Shadow.  The owner maintained that he had let the Ass only, and not his Shadow.  The Traveler asserted that he had, with the hire of the Ass, hired his Shadow also.  The quarrel proceeded from words to blows, and while the men fought, the Ass galloped off.  In quarreling about the shadow we often lose the substance.'
;

--
-- Concatenate the value with another value.
--

select 'A fable by Aesop: ' || fable
from   aesop
where  id=1;

--
-- Clip the first 255 bytes.
--
select id, clip(fable) 
from   aesop
where  id between 20 and 30;

-- --------------------------------------------------------------------------
-- 3. Admin info (Length, IsSmall, IsLarge, Sbspace)
-- --------------------------------------------------------------------------

--
-- Find out the min, max and average sizes of the fables. (The size reported 
-- is the text length + 8 bytes of UDT header information.)
--
select min(length(fable)) min_length, 
       avg(length(fable)) avg_length,
       max(length(fable)) max_length
from   aesop;

--
-- Find out how many are stored in-row and how-many are storied in a
-- smart blob.
--
select count(*) total_in_row
from   aesop
where  IsSmall(fable);

select count(*) total_sblob
from   aesop
where  IsLarge(fable);

--
-- Get more information about the big ones.
--

select id, title,
       Length(fable) length, 
       Sbspace(fable) sbspace
from   aesop
where  IsLarge(fable);


-- -------------------------------------------------------------------------
-- 4. String functions (Upper, Lower, Regexp_Match, Regexp_Replace, Instr)
-- -------------------------------------------------------------------------

select id,
       Lower(fable)
from   aesop
where  id=64;

select id,
       Upper(fable)
from   aesop
where  id=158;

--
-- Count the number of fables that involve birds of some sort.
-- 
-- We convert the input value to lowercase because this dataset has
-- lots of words with the first character in uppercase or even the entire
-- word in uppercase.
select count(*) from aesop where  
regexp_match(lower(fable), 
    'bird|eagle|swallow|crow|crane|raven|pigeon|hawk|stork');

--
-- Count how many involve foxes.
--
select count(*) from aesop
where  regexp_match(fable, ' [Ff][Oo][Xx]([Ee][Ss])?');

--
-- Look for fables that involve a tortoise, then highlight the word
-- tortoise with ***.
--
select regexp_replace(fable, '[Tt][Oo][Rr][Tt][Oo][Ii][Ss][Ee]', '***&***')
from   aesop where  regexp_match(lower(fable), 'tortoise');

--
-- You can also limit the number of replaces.
--
select regexp_replace(fable, '[Tt][Oo][Rr][Tt][Oo][Ii][Ss][Ee]', '***&***', 1)
from   aesop where  regexp_match(lower(fable), 'tortoise');

--
-- Instr / Instrb / Snip
--

-- Find the end of the first sentence in each fable.
select id, 
       instr(fable, '.') first,
       length(fable)-8 text_length 
from   aesop
where  id between 275 and 285;

-- Find the start of the last sentence in each fable.
select id, 
       instr(fable, '. ', -1) last,
       length(fable)-8 text_length 
from   aesop
where  id between 275 and 285;

-- Snip out the first and last sentences

select id,
       title,
       snip(fable, 1, instr(fable, '.')) first_sentence,
       snip(fable, instr(fable, '. ', -1)+2) last_sentence
from   aesop
where  id between 275 and 285;

-- You can also get the last word. This one uses instrb instead of instr.
select id,
       title,
       snip(fable, instrb(fable, ' ', -1)+1) last_word
from   aesop
where  id between 275 and 285;

--
-- Output the text immediately following a critter.
--
select id,
       snip(fable, instrb(lower(fable), 'tortoise'), 40) snippet
from   aesop
where  instrb(lower(fable), 'tortoise') > 0;

--
-- If we want to output the preceding text, we have to be careful
-- because a negative value passed to snip means search from the
-- end of the string. We could implement an SPL UDR that would
-- reset a negative value to the start of the string.
--

create function reset_neg (input_val integer) 
returning integer;
  define result integer;
  if input_val < 0 then
     let result=1;
  else
     let result=input_val;
  end if
  return result;
end function;

--
-- We can use the start point to reset any negative values:
--
select id,
       instrb(lower(fable), 'tortoise')-20, 
       reset_neg(instrb(lower(fable), 'tortoise')-20)
from   aesop
where  instrb(lower(fable), 'tortoise') > 0;

-- And use that expression so leading values will also be output
select id,
       title,
       snip(fable, reset_neg(instrb(lower(fable), 'tortoise')-20), 60) snippet
from   aesop
where  instrb(lower(fable), 'tortoise') > 0;

-- -------------------------------------------------------------------------
-- 5. Queries that are expected to fail
-- -------------------------------------------------------------------------

--
-- We expect the queries below to fail because idn_mrLvarchar
-- is not hashable.
--
select id 
from   aesop
where  fable in
       (select fable from aesop);

--
-- Notice that 'distinct' by itself is OK because it
-- calls the type's compare routine.
--
-- But 'count distinct' uses the built-in hash function,
-- so raises an error if we try to use it with an 
-- idn_mrLvarchar column.
--
select count (distinct fable)
from   aesop;

--
-- 'group by' also uses the built-in hash function,
-- so raises an error if we try to use it with an 
-- idn_mrLvarchar column.
--

select count(id), fable
from   aesop
group by 2;

--
-- Cleanup.
--
drop function reset_neg (integer);
