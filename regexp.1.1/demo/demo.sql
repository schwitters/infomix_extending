--
-- Demo for regexp bladelet.
--

execute procedure ifx_allow_newline('t');

drop table tongue_twisters;

create table tongue_twisters (id int, twister lvarchar);

--
-- Insert some data.
--
-- English tongue twisters from:
--    http://www.uebersetzung.at/twister/en.htm
--

insert into tongue_twisters values
(246,
'Sally sells sea shells by the sea shore. But if Sally sells sea
shells by the sea shore then where are the sea shells Sally sells?'
);

insert into tongue_twisters values
(278,
'Peter Piper picked a peck of pickled peppers.
A peck of pickled peppers Peter Piper picked.
Peter Piper picked a peck of pickled peppers,
Where''s the peck of pickled peppers Peter Piper picked?'
);

insert into tongue_twisters values
(286,
'If two witches would watch two watches, which witch would watch which watch?'
);

insert into tongue_twisters values
(301,
'Fuzzy Wuzzy was a bear, Fuzzy Wuzzy had no hair, 
Fuzzy Wuzzy wasn''t very fuzzy, was he?'
);

insert into tongue_twisters values
(306,
'I slit a sheet, a sheet I slit, and on that slitted sheet I sit.'
);

insert into tongue_twisters values
(313,
'Betty bought some bitter butter and it made her batter bitter,
so Betty bought some better butter to make her bitter batter better.'
);

 
insert into tongue_twisters values
(335,
'How much wood could a woodchuck chuck if a woodchuck could chuck wood?
A woodchuck could chuck as much wood as a woodchuck would chuck
if a woodchuck could chuck wood.'
);


insert into tongue_twisters values
(361,
'She sells seashells on the seashore. The seashells she
sells are seashore seashells.'
);

--
-- List the contents of the table.
--
select id, twister from tongue_twisters
order by id;


--
-- This regular expression looks for 'wood' or 'would': 
--
--     wo[ou]l?d
--

select id, twister 
from   tongue_twisters
where  regexp_match(twister, 'wo[ou]l?');

--
-- This regular expression looks for 'wood' or 'would': followed
-- by 0 or more characters until end-of-word identified by a space
-- or punctuation character. Note that the hyphen at the start of
-- the character class means a real hyphen and not a range of values.
--
--     wo[ou]l?d[a-z]*[- .?!]
--

select id, 
       regexp_replace(twister,  'wo[ou]l?d[a-z]*[- .?!]', '~&~')
from   tongue_twisters
where  regexp_match(twister, 'wo[ou]l?');

--
-- This next regular expression groups expression so we can put the 
-- ~ marks around just the word and not the word plus the ending
-- punctuation:
--
--     (wo[ou]l?d[a-z]*)([- .?!])
--     \_______________/\_______/
--            |             |
--            |             +------> punctuation gets referenced as \2
--            +--------------------> found word gets referenced as \1
--
select id,
       regexp_replace(twister,
       '(wo[ou]l?d[a-z]*)([- .?!])', '~\1~\2')
from   tongue_twisters
where  regexp_match(twister, 'wo[ou]l?');



--
-- Compare extract and split operations
--
execute function regexp_extract(
   'How much wood could a woodchuck chuck if a woodchuck could chuck wood?
    A woodchuck could chuck as much wood as a woodchuck would chuck
    if a woodchuck could chuck wood.',
    'wo[ou]l?d[a-z]*[- .?!]'
);

execute function regexp_split(
   'How much wood could a woodchuck chuck if a woodchuck could chuck wood?
    A woodchuck could chuck as much wood as a woodchuck would chuck
    if a woodchuck could chuck wood.',
    'wo[ou]l?d[a-z]*[- .?!]'
);

--
-- Limit the # results returned.
--
execute function regexp_extract(
   'How much wood could a woodchuck chuck if a woodchuck could chuck wood?
    A woodchuck could chuck as much wood as a woodchuck would chuck
    if a woodchuck could chuck wood.',
    'wo[ou]l?d[a-z]*[- .?!]',
    3
);

execute function regexp_split(
   'How much wood could a woodchuck chuck if a woodchuck could chuck wood?
    A woodchuck could chuck as much wood as a woodchuck would chuck
    if a woodchuck could chuck wood.',
    'wo[ou]l?d[a-z]*[- .?!]',
    3
);

drop table tongue_twisters;
