-- --------------------------------------------------------------------------
-- 
-- Module          : Functional Test for upper(idn_mrLvarchar)
-- Filename        : upper.sql
-- Description     : Tests additional string functions introduced in 1.1
-- 
-- Tests:
-- 
--    1. Small values
--    2. Big values
--    3. Dynamically generated small and big values
--
-- If enabled, additional trace info gets output to /tmp/mrLvarchar_upper.trc.
-- --------------------------------------------------------------------------

--
-- Turn tracing on.
--
execute procedure TraceSet_mrLvarchar ("/tmp/mrLvarchar_upper.trc", 20);

--
-- Set explain on.
--
!\rm -f sqexplain.out
set explain on;

--
-- 1. Small values
--

--
-- This value is small enough for the built-in upper() function to be
-- invoked.
--
execute function upper (
'The Swallow and the Crow: THE SWALLOW and the Crow had a contention about their plumage. The Crow put an end to the dispute by saying, "Your feathers are all very well in the spring, but mine protect me against the winter."  (Moral: Fair weather friends are not worth much.)');

--
-- Now invoke the idn_mrLvarchar routine.
--
execute function upper (
'The Swallow and the Crow: THE SWALLOW and the Crow had a contention about their plumage. The Crow put an end to the dispute by saying, "Your feathers are all very well in the spring, but mine protect me against the winter."  (Moral: Fair weather friends are not worth much.)'::idn_mrLvarchar);

 
select id, 
       mrcol, 
       upper(mrcol) value_uppercased
from   test_mr
where  id <= 5
order by id;

--
-- 2. Big values
--

execute function upper(
'A fable by Aesop: The Buffoon and the Countryman : A RICH NOBLEMAN once opened the theaters without charge to the people, and gave a public notice that he would handsomely reward any person who invented a new amusement for the occasion. Various public performers contended for the prize.  Among them came a Buffoon well known among the populace for his jokes, and said that he had a kind of entertainment which had never been brought out on any stage before.  This report being spread about made a great stir, and the theater was crowded in every part. The Buffoon appeared alone upon the platform, without any apparatus or confederates, and the very sense of expectation caused an intense silence.  He suddenly bent his head towards his bosom and imitated the squeaking of a little pig so admirably with his voice that the audience declared he had a porker under his cloak, and demanded that it should be shaken out.  When that was done and nothing was found, they cheered the actor, and loaded him with the loudest applause.  A Countryman in the crowd, observing all that has passed, said, "So help me, Hercules, he shall not beat me at that trick!"  and at once proclaimed that he would do the same thing on the next day, though in a much more natural way.  On the morrow a still larger crowd assembled in the theater, but now partiality for their favorite actor very generally prevailed, and the audience came rather to ridicule the Countryman than to see the spectacle.  Both of the performers appeared on the stage.  The Buffoon grunted and squeaked away first, and obtained, as on the preceding day, the applause and cheers of the spectators.  Next the Countryman commenced, and pretending that he concealed a little pig beneath his clothes (which in truth he did, but not suspected by the audience ) contrived to take hold of and to pull his ear causing the pig to squeak.  The Crowd, however, cried out with one consent that the Buffoon had given a far more exact imitation, and clamored for the Countryman to be kicked out of the theater.  On this the rustic produced the little pig from his cloak and showed by the most positive proof the greatness of their mistake.  "Look here," he said, "this shows what sort of judges you are."'::idn_mrLvarchar);

select id, 
       upper(mrcol) value_uppercased
from   test_mr
where  id > 5
order by id;

--
-- 3. Dynamically generated small and big values
--

select id, 
       mrcol, 
       upper(mrcol || '  *****  ' || mrcol) value_uppercased
from   test_mr
where  id <= 5
order by id;

select id, 
       upper(mrcol || '  *****  ' || mrcol) value_uppercased
from   test_mr
where  id > 5
order by id;

-- rename the explain output file to 'sqexplain.upper.log'
!mv -f sqexplain.out sqexplain.upper.log

-- ---------------------------------------------------------------
-- END
-- ---------------------------------------------------------------
