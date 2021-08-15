--
--  File: unregister.sql
--
-- About:
--
--  This removes the Period Stuff.
--
DROP FUNCTION Finish ( Period );
DROP FUNCTION Start ( Period );
DROP FUNCTION Length ( Period );
DROP FUNCTION GetInts ( Period );
DROP FUNCTION Hash ( Period );
--
DROP FUNCTION Compare ( Period, Period );
--
DROP OPCLASS Period_ops;
--




