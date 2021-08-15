-- ---------------------------------------------------------------------------
-- register.sql
--
-- Register all of the IUtil functions.
-- ---------------------------------------------------------------------------

CREATE FUNCTION ascii(lvarchar)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(ascii)"
LANGUAGE C;

CREATE FUNCTION chr(integer)
RETURNING char(1)
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(chr)"
LANGUAGE C;

CREATE FUNCTION iday(date)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(day_date)"
LANGUAGE C;

CREATE FUNCTION iday(datetime year to fraction)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(day_dt)"
LANGUAGE C;

CREATE FUNCTION dayname(date)
RETURNING varchar(100)
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayname_date)"
LANGUAGE C;

CREATE FUNCTION dayname(datetime year to fraction)
RETURNING varchar(100)
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayname_dt)"
LANGUAGE C;

CREATE FUNCTION dayofweek(date)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayofweek_date)"
LANGUAGE C;

CREATE FUNCTION dayofweek(datetime year to fraction)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayofweek_dt)"
LANGUAGE C;

CREATE FUNCTION dayofyear(date)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayofyear_date)"
LANGUAGE C;

CREATE FUNCTION dayofyear(datetime year to fraction)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(dayofyear_dt)"
LANGUAGE C;

CREATE FUNCTION degrees(double precision)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(degrees_dbl)"
LANGUAGE C;

CREATE FUNCTION degrees(decimal)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(degrees_dec)"
LANGUAGE C;

CREATE FUNCTION radians(double precision)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(radians_dbl)"
LANGUAGE C;

CREATE FUNCTION radians(decimal)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(radians_dec)"
LANGUAGE C;

CREATE FUNCTION ceil(integer)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(ceil_int)"
LANGUAGE C;

CREATE FUNCTION ceil(double precision)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(ceil_dbl)"
LANGUAGE C;

CREATE FUNCTION ceil(decimal)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(ceil_dec)"
LANGUAGE C;

CREATE FUNCTION floor(integer)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(floor_int)"
LANGUAGE C;

CREATE FUNCTION floor(double precision)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(floor_dbl)"
LANGUAGE C;

CREATE FUNCTION floor(decimal)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(floor_dec)"
LANGUAGE C;

CREATE FUNCTION sign(integer)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(sign_int)"
LANGUAGE C;

CREATE FUNCTION sign(double precision)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(sign_dbl)"
LANGUAGE C;

CREATE FUNCTION sign(decimal)
RETURNING double precision
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/IUtil/iutil.bld(sign_dec)"
LANGUAGE C;

