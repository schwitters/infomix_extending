create database tutorial;
database tutorial;

-- make gdb easier
create function tutorial_loadlib ()
returns boolean
with (not variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_loadlib)" language c;
grant execute on function tutorial_loadlib () to public;

create function tutorial_create (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_create)" language c;
grant execute on function tutorial_create (pointer) to public;

create function tutorial_drop (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_drop)" language c;
grant execute on function tutorial_drop (pointer) to public;

create function tutorial_open (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_open)" language c;
grant execute on function tutorial_open (pointer) to public;

create function tutorial_close (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_close)" language c;
grant execute on function tutorial_close (pointer) to public;

create function tutorial_insert (pointer,pointer,pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_insert)" language c;
grant execute on function tutorial_insert (pointer,pointer,pointer) to public;

create function tutorial_update (pointer,pointer,integer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_update)" language c;
grant execute on function tutorial_update (pointer,pointer,integer) to public;

create function tutorial_delete (pointer,integer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_delete)" language c;
grant execute on function tutorial_delete (pointer,integer) to public;

create function tutorial_beginscan (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_beginscan)" language c;
grant execute on function tutorial_beginscan (pointer) to public;

create function tutorial_endscan (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_endscan)" language c;
grant execute on function tutorial_endscan (pointer) to public;

create function tutorial_rescan (pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_rescan)" language c;
grant execute on function tutorial_rescan (pointer) to public;

create function tutorial_getnext (pointer,pointer,pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_getnext)" language c;
grant execute on function tutorial_getnext (pointer,pointer,pointer) to public;

create function tutorial_getbyid (pointer,pointer,integer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_getbyid)" language c;
grant execute on function tutorial_getbyid (pointer,pointer,integer) to public;

create function tutorial_cost (pointer,pointer)
returns real
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_cost)" language c;
grant execute on function tutorial_cost (pointer,pointer) to public;

create function tutorial_check (pointer,integer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_check)" language c;
grant execute on function tutorial_check (pointer,integer) to public;

create function tutorial_stats (pointer,pointer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_stats)" language c;
grant execute on function tutorial_stats (pointer,pointer) to public;

create function tutorial_truncate (pointer,integer)
returns integer
with (variant, class="tutorial")
external name "$INFORMIXDIR/extend/tutorial/tutorial.bld(tutorial_truncate)" language c;
grant execute on function tutorial_truncate (pointer,integer) to public;


create primary access_method tutorial (
    am_beginscan = tutorial_beginscan,
    am_check = tutorial_check,
    am_close = tutorial_close,
	am_cluster,
    am_create = tutorial_create,
    am_delete = tutorial_delete,
    am_drop = tutorial_drop,
    am_endscan = tutorial_endscan,
    am_getbyid = tutorial_getbyid,
    am_getnext = tutorial_getnext,
    am_insert = tutorial_insert,
    am_open = tutorial_open,
    am_readwrite,
	am_rescan = tutorial_rescan,
    am_rowids,
    am_scancost = tutorial_cost,
    am_sptype = 'S',
	am_stats = tutorial_stats,
    am_truncate = tutorial_truncate,
    am_update = tutorial_update );
