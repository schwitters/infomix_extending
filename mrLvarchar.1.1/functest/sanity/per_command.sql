-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for idn_mrLvarchar UDT
-- Filename        : per_command.sql
-- Description     : Base i/o tests for intermediate large values
--                   stored in separately allocated memory with a
--                   PER_COMMAND duration.
-- 
--   Tests:
--
--      1. Input / Output 
--      2. Assign
--	3. Length
--      4. Sbspace
--      5. Refcount
--      6. LOHandles
--      7. Update: mr threshold check -- small value to large 
--      8. Concat
--      9. Delete
--     10. Import/Export
--     11. Equal/NotEqual
--     12. Order by, distinct (Compare)
--     13. Send/Receive
--     14. Clip: includes mr threshold check -- update large value to small 
--     15. B-tree functional index
--     16. ConcatAll aggregate: including insert using result
--     17. Union query
--
--   Additional trace output is to /tmp/mrLvarchar_cmd.trc.
-- ---------------------------------------------------------------

--
-- Turn tracing on.
--
execute procedure TraceSet_mrLvarchar ("/tmp/mrLvarchar_cmd.trc", 20);

--
-- Set explain on.
--
!rm -f sqexplain.out
set explain on;

--
-- Set intermediate storage to PER_COMMAND.
--
execute function mrLvarcharTemp("PER_COMMAND");

--
-- 1. Input / Output 
--
-- Calling the Input routine directly executes the Input
-- routine to convert the text value into the UDT, then
-- executes the Output routine to display the result.
--
-- The threshold is 2040 bytes of data (2048 - 8 bytes header info)
-- This Aesop's fable is 2244 characters, so is over the threshold.
--
execute function idn_mrLvarcharIn(
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are."');

-- We can determine if the intermediate result is a smart blob by
-- running refcount() on the result, which returns NULL for
-- non-sblobs. We expect it to return NULL for the large value
-- cached in PER_COMMAND memory by Input.

execute function refcount(idn_mrLvarcharIn(
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are."'
));


--
-- 2. Assign
--
-- Inserting a value into a table executes the Input routine
-- to convert the text value to the UDT, then the Assign
-- routine just before it is written to the table.
--

insert into test_mr 
values (
1, 
'per_command.sql', 
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are."'
);

select id, mrcol from test_mr where id=1;

--
-- 3. Length: should be data size (2242) + 8 bytes UDT header = 2250
--
select id, Length(mrcol) length from test_mr where id=1;

--
-- 4. Sbspace: should return the name of the smart blob space
--    In the system generating this test, it is "sbspace".
--
select id, Sbspace(mrcol) space_name from test_mr where id=1;

--
-- 5. Refcount: should return 1 for this smart blob
--
select id, Refcount(mrcol) refcount from test_mr where id=1;

--
-- 6. LOHandles: returns handle for smart blob value
--
--    Since the handle is different from one test run
--    to the next, run this manually.
--
-- select id, LOHandles(mrcol) from test_mr where id=1;

--
-- 7. Update: mr threshold check -- small to large 
--
--    The inserted value is 1620 bytes, + 8 for UDT header (small).
--

insert into test_mr values (
2,
'per_command.sql',
'The Thief and the Innkeeper : A THIEF hired a room in a tavern and stayed a while in the hope of stealing something which should enable him to pay his reckoning.  When he had waited some days in vain, he saw the Innkeeper dressed in a new and handsome coat and sitting before his door.  The Thief sat down beside him and talked with him.  As the conversation began to flag, the Thief yawned terribly and at the same time howled like a wolf.  The Innkeeper said, "Why do you howl so fearfully?''  "I will tell you," said the Thief, "but first let me ask you to hold my clothes, or I shall tear them to pieces.  I know not, sir, when I got this habit of yawning, nor whether these attacks of howling were inflicted on me as a judgment for my crimes, or for any other cause; but this I do know, that when I yawn for the third time, I actually turn into a wolf and attack men."  With this speech he commenced a second fit of yawning and again howled like a wolf, as he had at first.  The Innkeeper.  hearing his tale and believing what he said, became greatly alarmed and, rising from his seat, attempted to run away. The Thief laid hold of his coat and entreated him to stop, saying, "Pray wait, sir, and hold my clothes, or I shall tear them to pieces in my fury, when I turn into a wolf."  At the same moment he yawned the third time and set up a terrible howl.  The Innkeeper, frightened lest he should be attacked, left his new coat in the Thief''s hand and ran as fast as he could into the inn for safety.  The Thief made off with the coat and did not return again to the inn.  (Moral: Every tale is not to be believed.)'
);

select id, 
       Length(mrcol) length,
       Refcount(mrcol) refcount
from test_mr where id=2;

--
-- Use Concat to update the value to a large value.
--
-- 1620 + 1620 + 8 = 3248
--
update test_mr
set mrcol = mrcol || mrcol
where id=2;

select id, 
       Length(mrcol) length,
       Refcount(mrcol) refcount
from test_mr where id=2;

--
-- 8. Concat
--
-- Actually started getting tested in #7.
--

--
-- Little value + big value
--

select id, 'Aesop: ' || mrcol from test_mr
order by id;

--
-- Big value + Big value
--
select  id, 
        length(mrcol) length, 
        length(mrcol || mrcol) new_length 
from    test_mr
order by id;

-- The new refcount should be NULL because the new object 
-- does not get saved in a table.

select  id, 
        refcount(mrcol) refcount, 
        refcount(mrcol || mrcol) new_refcount 
from    test_mr
order by id;


--
-- 9. Delete
--
delete from test_mr;

--
-- 10. Import Export
--
load from big_values.txt insert into test_mr;

select * from test_mr order by id;

select id,
       Length(mrcol) length,
       Refcount(mrcol) refcount
from test_mr order by 1;

-- naming the output a .log means cleanup will remove it
unload to big_valTEST.log select * from test_mr;

--
-- 11. Equal/NotEqual
--

--
-- This should return id=5
--
select m1.id from test_mr m1
where  m1.mrcol = 
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are."' ;

--
-- More than likely, a large value would be compared to a large
-- value stored in a table.
--
select m1.id from test_mr m1
where  m1.mrcol = 
       (select m2.mrcol from test_mr m2 where m2.id=5);

--
-- This should return id=5
--
select m1.id from test_mr m1
where  m1.mrcol <> 
       (select m2.mrcol from test_mr m2 where m2.id=6);

--
-- Concat and Equal in the where clause
--
-- Should return id 5.
--
select id
from   test_mr
where  Concat(mrcol, ': the end') =
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are.": the end'; 

--
-- 12. Order by, distinct (Compare)
--
select id, mrcol from test_mr order by mrcol;

select distinct mrcol 
from   test_mr 
order  by mrcol;

--
-- 13. Send/Receive
--

--
-- Lengths should be the same
--
select id, 
       length(mrcol) mrcol_len,
       length(idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) sendrecv_len
from   test_mr
order  by 1;

--
-- The "comparison" result should be 0
--
select id, 
       Compare(mrcol,idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) comparison
from   test_mr
order  by 1;

--
-- The "equality" result should be 't'
--
select id, 
       Equal(mrcol,idn_mrLvarcharRecv(idn_mrLvarcharSend(mrcol))) equality
from   test_mr
order  by 1;

--
-- 14. Clip: includes mr threshold check -- update large value to small 
--
select id, 
       length(mrcol) mrcol_len 
from   test_mr;

select id, 
       clip(mrcol) mrcol_clipped 
from   test_mr;

select id, 
       length(clip(mrcol)) clipped_len
from   test_mr;

select id, refcount(mrcol) refcount, mrcol from test_mr order by id;

update test_mr
set    mrcol = clip(mrcol)
where  id=6;

select id, refcount(mrcol) refcount, mrcol from test_mr order by id;

--
-- clean up from this test
--
delete from test_mr;
load from big_values.txt insert into test_mr;

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

select id, mrcol from test_mr order by id;

select ConcatAll(mrcol) concat_result
from   test_mr;

insert into test_mr values
( (select max(id)+1 from test_mr),
  "per_command.sql",
  (select ConcatAll(mrcol) from test_mr)
);

select * from test_mr
order by id;

--
-- 17. Union query
--
select 1, ConcatAll(mrcol) concat_result
from   test_mr
union
select 2, ConcatAll(mrcol) concat_result
from   test_mr
order by 1;

select * from test_mr
order by id;

--
-- Cleanup
--

delete from test_mr;

-- rename the explain output file to 'sqexplain.cmd.log'
!mv -f sqexplain.out sqexplain.cmd.log

-- ---------------------------------------------------------------
-- END
-- ---------------------------------------------------------------
