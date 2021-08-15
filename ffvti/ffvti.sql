
CREATE FUNCTION set_tracing(lvarchar, integer, lvarchar)
RETURNING int
WITH(HANDLESNULLS)
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(set_tracing)"
LANGUAGE C;

CREATE FUNCTION ff_open(ptr pointer)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_open)"
LANGUAGE C;

CREATE FUNCTION ff_beginscan(ptr pointer)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_beginscan)"
LANGUAGE C;

CREATE FUNCTION ff_getnext(ptr pointer, ptr2 pointer, OUT rowid int)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_getnext)"
LANGUAGE C;

CREATE FUNCTION ff_endscan(ptr pointer)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_endscan)"
LANGUAGE C;

CREATE FUNCTION ff_close(ptr pointer)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_close)"
LANGUAGE C;

CREATE FUNCTION ff_drop(ptr pointer)
RETURNING int
EXTERNAL NAME "$INFORMIXDIR/extend/ffvti/ffvti.bld(ff_drop)"
LANGUAGE C;

INSERT INTO systraceclasses(name) VALUES("ffvti");

CREATE PRIMARY ACCESS_METHOD FFAccess(
am_open = ff_open,
am_close = ff_close,
am_drop = ff_drop,
am_beginscan = ff_beginscan,
am_endscan = ff_endscan,
am_getnext = ff_getnext,
am_sptype = "A");

