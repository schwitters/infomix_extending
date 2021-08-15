database tutorial;

drop access_method tutorial restrict;

drop function tutorial_loadlib ();
drop function tutorial_create (pointer);
drop function tutorial_drop (pointer);
drop function tutorial_open (pointer);
drop function tutorial_close (pointer);
drop function tutorial_insert (pointer,pointer,pointer);
drop function tutorial_update (pointer,pointer,integer);
drop function tutorial_delete (pointer,integer);
drop function tutorial_beginscan (pointer);
drop function tutorial_endscan (pointer);
drop function tutorial_rescan (pointer);
drop function tutorial_getnext (pointer,pointer,pointer);
drop function tutorial_getbyid (pointer,pointer,integer);
drop function tutorial_cost (pointer,pointer);
drop function tutorial_check (pointer,integer);
drop function tutorial_stats (pointer,pointer);
drop function tutorial_truncate (pointer,integer);

database sysmaster;

drop database tutorial;
