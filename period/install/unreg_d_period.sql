--
--  File: unreg_d_period.sql
--
-- About:
--
--  This removes the Period Stuff.
--
---------------- Constructor Functions -------------------------------------
--
DROP FUNCTION PeriodIn  ( lvarchar );
DROP FUNCTION PeriodOut ( Period );
DROP FUNCTION Period    ( DATE, DATE );
DROP FUNCTION Period    ( DATE );
--
DROP FUNCTION PeriodSend ( Period ) ;
DROP FUNCTION PeriodRecv ( sendrecv ) ;
DROP FUNCTION PeriodImpT ( impexp ) ;
DROP FUNCTION PeriodExpT ( Period );
DROP FUNCTION PeriodImpB ( impexpbin );
DROP FUNCTION PeriodExpB ( Period );
--
DROP FUNCTION Set_Start_Epoch    ( Period );
DROP FUNCTION Start_Is_Epoch     ( Period );
DROP FUNCTION Set_Finish_Unbound ( Period );
DROP FUNCTION Finish_Is_Unbound  ( Period );
--
DROP FUNCTION Equal    ( Period, Period );
DROP FUNCTION NotEqual ( Period, Period );
--
DROP FUNCTION Union ( Period, Period, Period );
DROP FUNCTION Size  ( Period, double precision );
DROP FUNCTION Inter ( Period, Period, Period );
DROP FUNCTION ContainsNotTouches ( Period, Period );
DROP FUNCTION Contains ( Period, Period );
DROP FUNCTION WithinNotTouches ( Period, Period );
DROP FUNCTION Within ( Period, Period );
DROP FUNCTION OverlapNotTouches ( Period, Period );
DROP FUNCTION OverlapNotTouches ( Period, Period, OUT INTEGER );
DROP FUNCTION Overlap ( Period, Period );
DROP FUNCTION Overlap ( Period, Period, OUT INTEGER );
--
--
--------------------- Non-Spatial Support Functions -----------------------
--
DROP FUNCTION Hash( Period );
DROP FUNCTION Compare( Period, Period );
--
-------------------------  Interface Functions ----------------------------
--
DROP FUNCTION Length        ( Period );
DROP FUNCTION Start         ( Period );
DROP FUNCTION Finish        ( Period );
DROP FUNCTION GetIntersect  ( Period, Period );
DROP FUNCTION GetUnion      ( Period, Period );
DROP FUNCTION Before        ( Period, Period );
DROP FUNCTION BeforeTouches ( Period, Period );
DROP FUNCTION After         ( Period, Period );
DROP FUNCTION AfterTouches  ( Period, Period );
--
---------------------------- Aggregate ------------------------------------
--
DROP AGGREGATE Min_Overlap;
--
DROP FUNCTION Period_Overlap_Init( Period );
DROP FUNCTION Period_Overlap_Iter( Period_Overlap_State, 
                                   Period );
DROP FUNCTION Period_Overlap_Combine( Period_Overlap_State,
                                      Period_Overlap_State );
DROP FUNCTION Period_Overlap_Final( Period_Overlap_State );
--
DROP TYPE Period_Overlap_State;
--
--------------------------- Error Messages --------------------------------
--
DROP FUNCTION CompareString( Period, Period );
--
DELETE FROM syserrors WHERE sqlstate LIKE 'DT_INT%'
--
--------------------------  OPCLASS -------------------------------------
--
DROP OPCLASS Period_ops;
--
-----------------------------  Type ---------------------------------------
--
DROP TYPE Period;
