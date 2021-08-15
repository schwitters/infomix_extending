-- ------------------------------------------------------------
-- run.sql
--
-- Exercise the VTable routines
-- ------------------------------------------------------------
-- 0. What version is this?
--
execute procedure vtrelease();

-- ------------------------------------------------------------

-- 1. Verify the current state and lineage. Should be "1" and
--    "0,1" respectively.
--
execute procedure current_state();
execute procedure current_lineage();

-- ------------------------------------------------------------
-- 2. Create two new states that are separate branches from 
--    state #1

-- verify that state 1 is open
execute procedure state_is_open(1);

-- verify that state 1 is not closed
execute procedure state_is_closed(1);

-- close it
execute procedure state_close(1);

-- verify the new state of state 1 (both methods)
execute procedure state_is_open(1);
execute procedure state_is_closed(1);

-- create two new states with state 1 as their parent
execute procedure state_create(1, "First Branch");
execute procedure state_create(1, "Second Branch");

-- are the new states open or closed?
execute procedure state_is_open(2);
execute procedure state_is_closed(2);

execute procedure state_is_open(3);
execute procedure state_is_closed(3);

-- what are the description fields for them?
execute procedure state_description(2);
execute procedure state_description(3);

-- ------------------------------------------------------------
-- 3. set state to our new state #2 and make some changes

execute procedure set_state(2);
execute procedure current_state();
execute procedure current_lineage();

select sequence, city, objective 
from v_itinerary
order by sequence;

-- INSERT
insert into v_itinerary
(
  sequence, city, objective
)
values
(
  5, "Miami", "Spring Break!"
);

select sequence, city, objective 
from v_itinerary
order by sequence;

-- UPDATE
update v_itinerary
set objective = "Swim with dolphins"
where sequence = 4;

select sequence, city, objective 
from v_itinerary
order by sequence;

-- DELETE
delete from v_itinerary
where sequence = 3;

select sequence, city, objective 
from v_itinerary
order by sequence;

-- ------------------------------------------------------------
-- 4. set state to our new #3 and verify that state #2's 
--    changes aren't visible here.
--
execute procedure set_state(3);
execute procedure current_state();
execute procedure current_lineage();

select sequence, city, objective 
from v_itinerary
order by sequence;

-- ------------------------------------------------------------
-- 5. Make some changes in state #3

insert into v_itinerary
(
  sequence, city, objective
)
values
(
  6, "Key West", "Cheesburgers in paradise"
);

update v_itinerary
set objective = "Meet with Disney executives"
where city = "Orlando";

-- ------------------------------------------------------------
-- 6. Review the table contents as seen from all 3 states.

execute procedure set_state(1);
execute procedure current_state();

select sequence, city, objective 
from v_itinerary
order by sequence;

execute procedure set_state(2);
execute procedure current_state();

select sequence, city, objective 
from v_itinerary
order by sequence;

execute procedure set_state(3);
execute procedure current_state();

select sequence, city, objective 
from v_itinerary
order by sequence;

-- ------------------------------------------------------------
-- 7. close both of our new states. closing a state allows it
--    to be used as the parent of a new state, but it prevents
--    any more changes to be made to it. (unless, of course,
--    it's opened again!)

execute procedure state_close(2);
execute procedure state_close(3);

-- Verify that both states are now closed (and not open)
execute procedure state_is_open(2);
execute procedure state_is_closed(2);

execute procedure state_is_open(3);
execute procedure state_is_closed(3);

-- Done.
