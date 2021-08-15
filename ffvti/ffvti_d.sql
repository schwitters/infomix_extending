drop access_method FFAccess restrict;
drop function ff_open;
drop FUNCTION ff_beginscan;
drop FUNCTION ff_getnext;
drop FUNCTION ff_endscan;
drop FUNCTION ff_close;
drop FUNCTION ff_drop;
drop function set_tracing(lvarchar, integer, lvarchar);
delete from systraceclasses where name = "ffvti";
