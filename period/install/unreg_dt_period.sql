--
--  File: unreg_dt_period.sql
--
-- About:
--
--  This removes the Period Stuff.
--
--
---------------- Constructor Functions -------------------------------------
--
DROP FUNCTION DT_PeriodIn (lvarchar) ;
DROP FUNCTION DT_PeriodOut (DT_Period) ;
DROP FUNCTION DT_Period ( DATETIME YEAR TO SECOND, DATETIME YEAR TO SECOND );
DROP FUNCTION DT_Period ( Arg1 DATETIME YEAR TO SECOND );
--
DROP FUNCTION DT_PeriodSend (DT_Period) ;
DROP FUNCTION DT_PeriodRecv (sendrecv) ;
DROP FUNCTION DT_PeriodImpT (impexp) ;
DROP FUNCTION DT_PeriodExpT (DT_Period);
DROP FUNCTION DT_PeriodImpB (impexpbin);
DROP FUNCTION DT_PeriodExpB (DT_Period);
--
DROP FUNCTION Set_Start_Epoch (DT_Period);
DROP FUNCTION Start_Is_Epoch (DT_Period);
DROP FUNCTION Set_Finish_Unbound (DT_Period);
DROP FUNCTION Finish_Is_Unbound (DT_Period);
--
DROP FUNCTION Equal (DT_Period,DT_Period);
DROP FUNCTION NotEqual (DT_Period,DT_Period);
--
DROP FUNCTION Union (DT_Period,DT_Period,DT_Period);
DROP FUNCTION Size (DT_Period, double precision);
DROP FUNCTION Inter (DT_Period,DT_Period,DT_Period);
DROP FUNCTION ContainsNotTouches (DT_Period,DT_Period);
DROP FUNCTION Contains (DT_Period,DT_Period);
DROP FUNCTION WithinNotTouches (DT_Period,DT_Period);
DROP FUNCTION Within (DT_Period,DT_Period);
DROP FUNCTION OverlapNotTouches (DT_Period, DT_Period );
DROP FUNCTION OverlapNotTouches (DT_Period, DT_Period, OUT INTEGER );
DROP FUNCTION Overlap(DT_Period, DT_Period );
DROP FUNCTION Overlap (DT_Period,DT_Period, OUT INTEGER );
--
--
--------------------- Non-Spatial Support Functions -----------------------
--
DROP FUNCTION Hash(DT_Period);
DROP FUNCTION Compare(DT_Period,DT_Period);
--
-------------------------  Interface Functions ----------------------------
--
DROP FUNCTION Length(DT_Period);
DROP FUNCTION Period_Interval(DT_Period);
DROP FUNCTION Start(DT_Period);
DROP FUNCTION Finish(DT_Period);
DROP FUNCTION GetIntersect(DT_Period, DT_Period);
DROP FUNCTION GetUnion(DT_Period, DT_Period);
DROP FUNCTION Before(DT_Period, DT_Period);
DROP FUNCTION BeforeTouches(DT_Period, DT_Period);
DROP FUNCTION After(DT_Period, DT_Period);
DROP FUNCTION AfterTouches(DT_Period, DT_Period);
--
---------------------------- Aggregate ------------------------------------
--
DROP AGGREGATE Min_Overlap;
--
DROP FUNCTION DT_Period_Overlap_Init( DT_Period );
DROP FUNCTION DT_Period_Overlap_Iter( DT_Period_Overlap_State, 
                                      DT_Period );
DROP FUNCTION DT_Period_Overlap_Combine( DT_Period_Overlap_State,
                                         DT_Period_Overlap_State );
DROP FUNCTION DT_Period_Overlap_Final( DT_Period_Overlap_State );
--
DROP TYPE DT_Period_Overlap_State;
--
--------------------------- Error Messages --------------------------------
--
DROP FUNCTION CompareString(DT_Period, DT_Period);
--
DELETE FROM syserrors WHERE sqlstate LIKE 'DT_INT%'
--
--------------------------  OPCLASS -------------------------------------
--
DROP OPCLASS DT_Period_ops;
--
-----------------------------  Type ---------------------------------------
--
DROP TYPE DT_Period;
