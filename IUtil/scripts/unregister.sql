-- ---------------------------------------------------------------------------
-- unregister.sql
--
-- Drop all of the IUtil functions.
-- ---------------------------------------------------------------------------

DROP FUNCTION ascii(lvarchar);
DROP FUNCTION chr(integer);
DROP FUNCTION iday(date);
DROP FUNCTION iday(datetime year to fraction);
DROP FUNCTION dayname(date);
DROP FUNCTION dayname(datetime year to fraction);
DROP FUNCTION dayofweek(date);
DROP FUNCTION dayofweek(datetime year to fraction);
DROP FUNCTION dayofyear(date);
DROP FUNCTION dayofyear(datetime year to fraction);
DROP FUNCTION degrees(double precision);
DROP FUNCTION degrees(decimal);
DROP FUNCTION radians(double precision);
DROP FUNCTION radians(decimal);
DROP FUNCTION ceil(integer);
DROP FUNCTION ceil(double precision);
DROP FUNCTION ceil(decimal);
DROP FUNCTION floor(integer);
DROP FUNCTION floor(double precision);
DROP FUNCTION floor(decimal);
DROP FUNCTION sign(integer);
DROP FUNCTION sign(double precision);
DROP FUNCTION sign(decimal);

