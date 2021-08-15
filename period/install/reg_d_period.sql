--
--  File:  reg_d_period.sql
--
-- About:
--
--   This is the register.sql file, extracted at great labour, from
--  the object.sql. This CREATEs the types and so on, including
--  the opclass, in keeping WITH the interdiction of the docs.
-- 
-----------------------------  Type ---------------------------------------
--
CREATE OPAQUE TYPE Period
( 	internallength = 8
);
--
GRANT USAGE ON TYPE Period TO PUBLIC; 
--
---------------- Constructor Functions -------------------------------------
--
--   The in . . . .
--
CREATE FUNCTION PeriodIn (lvarchar) 
RETURNS Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodInput)"
LANGUAGE C
DOCUMENT " Constructor function for Period. Valid external format is",
         " DATE to DATE.  The first DATE argument must precede the",
         " second. Note also that there are two special values: EPOCH",
         " and FOREVER. These indicate that the interval is open at",
         " either the beginning or at the end. ",
         " Example: PeriodIn('""1997-4-1"" to ""1997-4-1""'); ";
--
GRANT EXECUTE ON FUNCTION PeriodIn (lvarchar) TO PUBLIC;
--
CREATE IMPLICIT CAST ( lvarchar AS Period WITH  PeriodIn );
--
--   and the out.
--
CREATE FUNCTION PeriodOut (Period) 
RETURNS lvarchar 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOutput)" 
LANGUAGE C
DOCUMENT " Output function for Period. Valid external format is",
         " DATE to DATE. The first value will precede the second.",
         " Note also that there are two special values: EPOCH and",
         " FOREVER. These indicate that the interval is open at",
         " either its beginning or at its end.",
         " Example: '""1997-4-1"" to ""1997-4-1""'";
--
GRANT EXECUTE ON FUNCTION PeriodOut (Period) TO PUBLIC;
--
CREATE IMPLICIT CAST ( Period AS lvarchar WITH PeriodOut ); 
--
--  Constructed from two DATE values.
--
CREATE FUNCTION Period ( DATE, DATE )
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_D_D)"
LANGUAGE C
DOCUMENT "Constructor function that takes two DATEs. This is the ",
         "workhorse constructor because getting the public format ",
         "right is tricky. ";
--
GRANT EXECUTE ON FUNCTION Period(DATE, DATE) TO PUBLIC;
--
--  This is simply an SPL function that shims the one above.
--
CREATE FUNCTION Period ( Arg1 DATE )
RETURNING Period
	RETURN Period ( Arg1, Arg1 );
END FUNCTION;
--
GRANT EXECUTE ON FUNCTION Period ( DATE ) TO PUBLIC;
--
CREATE CAST ( DATE AS Period WITH Period );
--
-----------------------------------  Supports -------------------------------
--
--  Binary Send and Receive functions.
--
CREATE FUNCTION PeriodSend (Period) 
RETURNS sendrecv 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodSend)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodSend (Period) TO PUBLIC;
--
CREATE CAST  ( Period AS sendrecv WITH  PeriodSend );
--
CREATE FUNCTION PeriodRecv (sendrecv) 
RETURNS Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodReceive)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodRecv (sendrecv) TO PUBLIC;
-- 
CREATE IMPLICIT CAST ( sendrecv AS Period WITH PeriodRecv );
--
--  These are for the UNLOAD/RELOAD and the backup/recover stuff. 
--  Note that I re-use the Literal Expression UDFs here.
--
CREATE FUNCTION PeriodImpT (impexp) 
RETURNS Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodInput)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodImpT (impexp) TO PUBLIC;
--
CREATE IMPLICIT CAST ( impexp AS Period WITH  PeriodImpT );
--
CREATE FUNCTION PeriodExpT (Period)
RETURNS impexp
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOutput)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodExpT (Period) TO PUBLIC;
--
CREATE CAST ( Period AS impexp WITH PeriodExpT );
--
CREATE FUNCTION PeriodImpB (impexpbin)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodReceive)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodImpB (impexpbin) TO PUBLIC;
--
CREATE IMPLICIT CAST ( impexpbin AS Period WITH PeriodImpB );
--
CREATE FUNCTION PeriodExpB (Period)
RETURNS impexpbin
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodSend)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodExpB (Period) TO PUBLIC;
--
CREATE CAST ( Period AS impexpbin WITH  PeriodExpB );
--
-----------------------------------------------------------------------------
--
--      Useful Functions
--
-----------------------------------------------------------------------------
--
CREATE FUNCTION Set_Start_Epoch (Period)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_Set_Start_Epoch)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Set_Start_Epoch(Period) TO PUBLIC;
--
CREATE FUNCTION Start_Is_Epoch (Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_Start_is_Epoch)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Start_Is_Epoch(Period) TO PUBLIC;
--
CREATE FUNCTION Set_Finish_Forever (Period)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_Set_Finish_Unbound)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Set_Finish_Forever (Period) TO PUBLIC;
--
CREATE FUNCTION Finish_Is_Forever (Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_Finish_is_Unbound)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Finish_Is_Forever (Period) TO PUBLIC;
--
-------------------------------- Statistics ----------------------------------
--
CREATE FUNCTION StatCollect ( Period, double precision, double precision )
RETURNS LVARCHAR
WITH ( 
	HANDLESNULLS, NOT VARIANT 
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_StatCollect)"
LANGUAGE C
DOCUMENT "Statistics for the Period Data Type.";
--
GRANT EXECUTE ON FUNCTION StatCollect ( Period, double precision, double precision )
TO PUBLIC;
--
CREATE FUNCTION StatPrint ( Period, stat )
RETURNS LVARCHAR
WITH ( 
	HANDLESNULLS, NOT VARIANT 
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_StatPrint)"
LANGUAGE C
DOCUMENT "Print out statistics for the Period Data Type.";
--
GRANT EXECUTE ON FUNCTION StatPrint ( Period, stat ) TO PUBLIC;
--
CREATE FUNCTION Period_Equal_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_EqualSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for Equal( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_Equal_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_NotEqual_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_NotEqualSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for NotEqual( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_NotEqual_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_ContainsNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_ContainsNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for ContainsNotTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_ContainsNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_ContainsTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_ContainsTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for ContainsTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_ContainsTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_WithinTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_WithinTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for WithinTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_WithinTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_WithinNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_WithinNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for WithinNotTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_WithinNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_OverlapTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_OverlapTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for OverlapTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_OverlapTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_OverlapNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_OverlapNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for OverlapNotTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_OverlapNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_BeforeTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_BeforeTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for BeforeTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_BeforeTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_BeforeNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_BeforeNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for BeforeNotTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_BeforeNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_AfterTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_AfterTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for AfterTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_AfterTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION Period_AfterNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_AfterNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for AfterNotTouches( Period, Period ).";
--
GRANT EXECUTE ON FUNCTION Period_AfterNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
-------------------------  Operators ----------------------------------------
--
--    These are really only used internally. There is no way to 
--  sort these things in a meaningful way, but you need Compare() and
--  Equal() for things like UNION and DISTINCT.
--
CREATE FUNCTION Equal (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	NEGATOR    = NotEqual,
	COMMUTATOR = Equal,
	SELFUNC    = Period_Equal_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodEqual)"
LANGUAGE C
DOCUMENT "Equal( Period, Period ) returns true iff. the two ",
         "arguments have the same start DATE and the same end DATE.",
         "The ORDBMS can use an R-tree index to accellerate this query. ";
--
GRANT EXECUTE ON FUNCTION Equal (Period,Period) TO PUBLIC;
--
CREATE FUNCTION NotEqual (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	NEGATOR    = NotEqual,
	COMMUTATOR = Equal,
	SELFUNC    = Period_NotEqual_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodNotEqual)"
LANGUAGE C
DOCUMENT "NotEqual( Period, Period ) returns true unless both   ",
         "arguments have the same start date and the same finish date. ";
--
GRANT EXECUTE ON FUNCTION NotEqual (Period,Period) TO PUBLIC;
--
--------------------------------------------------------------------------------
--
--    These functions are used by the R-tree opclass.
--
--------------------------------------------------------------------------------
--
CREATE FUNCTION Union (Period,Period,Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodUnion)"
LANGUAGE C
DOCUMENT "Union(Period, Period, Period) places a Union ",
         "of the first two arguments into the second argument. ";
--
GRANT EXECUTE ON FUNCTION Union (Period,Period,Period) TO PUBLIC;
--
CREATE FUNCTION Size (Period, double precision)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodSize)"
LANGUAGE C
DOCUMENT "Size(Period, double) calculates the size of the first argument,",
         "and returns this in the second argument. ";
--
GRANT EXECUTE ON FUNCTION Size (Period,double precision) TO PUBLIC;
--
CREATE FUNCTION Inter (Period,Period,Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodInter)"
LANGUAGE C
DOCUMENT "Inter(Period, Period, Period) places a value ",
         "that is the intersection between the two arguments into the third.";
--
GRANT EXECUTE ON FUNCTION Inter (Period,Period,Period) TO PUBLIC;
--
------------------------------------------------------------------------------
--
--   Spatial Functions
--
------------------------------------------------------------------------------
--
CREATE FUNCTION ContainsNotTouches (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_ContainsNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodContainsNotTouches)"
LANGUAGE C
DOCUMENT " ContainsNotTouches( Period, Period ) returns true iff. the ",
         " start and finish DATEs of the second argument are within the ",
         " start and finish DATEs of the first. ContainsNotTouches(A,B) ",
         " is A Contains B completely.",
         " Note that within in this case excludes equals; in other words,",
         " ContainsNotTouches does not Equal, and that touches does not count",
	 " as contains. ";
--
GRANT EXECUTE ON FUNCTION ContainsNotTouches (Period,Period) TO PUBLIC;
--
CREATE FUNCTION Contains (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_ContainsTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodContainsTouches)"
LANGUAGE C
DOCUMENT " Contains ( Period, Period ) returns true iff. the start and",
         " finish DATE of the second argument are within the start and",
         " finish DATE of the first. i.e. Contains (A,B) is A Contains",
	 " Contains B.",
         " Note that within in this case includes equals; in other words,",
         " Equal(Period, Period ) implies Contains(Period,Period)",
	 " and that touches counts as contains.";
--
GRANT EXECUTE ON FUNCTION Contains (Period,Period) TO PUBLIC;
--
CREATE FUNCTION WithinNotTouches (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_WithinNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodWithinNotTouches)"
LANGUAGE C
DOCUMENT " WithinNotTouches( Period, Period ) returns true iff. the",
         " start and finish DATE of the first argument are within the",
	 " start and finish DATE of the second. WithinNotTouches(A,B)",
	 " is B Contains A completely. Within, in this case, excludes Equal;",
         " in other words, touches at either end do not count.";
--
GRANT EXECUTE ON FUNCTION WithinNotTouches (Period,Period) TO PUBLIC;
--
CREATE FUNCTION Within (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_WithinTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodWithinTouches)"
LANGUAGE C
DOCUMENT " Within ( Period, Period ) returns true iff. the start and",
         " finish DATE of the first argument are within the start",
         " and finish DATE of the second. i.e. Within(A,B) is B",
         " Contains A. Note that within in this case includes equals;",
         " in other words, touches at either end count.";
--
GRANT EXECUTE ON FUNCTION Within (Period,Period) TO PUBLIC;
--
CREATE FUNCTION OverlapNotTouches (Period, Period )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_OverlapNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapNotTouches)"
LANGUAGE C
DOCUMENT " OverlapNotTouches( Period, Period ) returns true iff. the",
         " start or the finish of either of argument is between the start/",
	 " finish of the other, and neither end touches.";
--
GRANT EXECUTE ON FUNCTION OverlapNotTouches (Period,Period) TO PUBLIC;
--
CREATE FUNCTION OverlapNotTouches (Period, Period, OUT INTEGER )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_OverlapNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapNotTouchesWithLength)"
LANGUAGE C
DOCUMENT " OverlapNotTouches( Period, Period, OUT INTEGER ) ",
         " returns true iff. the start or the finish of either of arguments",
	 " is complete between the start/finish of the other, and neither end",
	 " touches. In addition, the OUT paramater returns the size of the",
	 " Overlap as some number of days.";
--
GRANT EXECUTE ON FUNCTION OverlapNotTouches(Period,Period,INTEGER)
TO PUBLIC;
--
CREATE FUNCTION Overlap(Period, Period )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_OverlapTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapTouches)"
LANGUAGE C
DOCUMENT " Overlap( Period, Period ) returns true iff. the",
         " start or the finish of either of argument is between the start/",
	 " finish of the other. Touches count. This is the most conventional",
	 " broad definition of Overlap(). This UDR is the important one used",
	 " in the R-Tree indexing data structure.";
--
GRANT EXECUTE ON FUNCTION Overlap (Period,Period) TO PUBLIC;
--
CREATE FUNCTION Overlap (Period,Period, OUT INTEGER )
RETURNS boolean
WITH (
    NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_OverlapTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapTouchesWithLength)"
LANGUAGE C
DOCUMENT " Overlap( Period, Period, OUT INTEGER ) returns true iff. one",
         " of the DATE elements is between the start and finish dates",
	 " of the other. The additional OUT paramater returns an INTEGER for",
         " days in the overlap. This is used to combine the Indexing and",
         " the UDR.";
--
GRANT EXECUTE ON FUNCTION Overlap (Period, Period, INTEGER) TO PUBLIC;
--
--------------------------  OPCLASS -------------------------------------
--
CREATE OPCLASS Period_ops FOR RTREE
STRATEGIES( Overlap(Period, Period), 
            Equal(Period, Period), 
            Contains(Period, Period), 
            Within(Period, Period), 
            Overlap(Period, Period, INTEGER ),
	        ContainsNotTouches(Period, Period), 
            WithinNotTouches(Period, Period), 
	        OverlapNotTouches(Period, Period),
	        OverlapNotTouches(Period, Period, INTEGER ),
			BeforeTouches( Period, Period ),
			AfterTouches ( Period, Period )
 )    
SUPPORT ( Union, Size, Inter );  
--
CREATE FUNCTION rtreeInfo(Period,pointer,pointer,pointer) 
RETURNS INTEGER
WITH ( NOT VARIANT )
EXTERNAL NAME '$INFORMIXDIR/extend/Period/bin/Period.bld(D_DT_RtreeInfo)'
LANGUAGE C;
                               
--
--  Note: The way this is used is to specify the opclass as part of the
--        create index statement. For example:
--
-- CREATE INDEX Res_1 ON Reserves(Reserv Period_ops) USING rtree;
--
----------------------- Non-Spatial Support Functions ---------------------
--
CREATE FUNCTION Hash(Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodHash)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Hash (Period) TO PUBLIC;
--
--  This is only used when you need to discard duplicates as part of a
--  UNION query.
--
CREATE FUNCTION Compare(Period,Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodCompare)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Compare (Period, Period) TO PUBLIC;
--
--------------------------------  Interface Functions ------------------------
--
CREATE FUNCTION Length(Period)
RETURNS INTEGER
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodSizeAsDays)"
LANGUAGE C
DOCUMENT "Length(Period) returns the INTEGER days in the Period.";
--
GRANT EXECUTE ON FUNCTION Length(Period) TO PUBLIC;
--
CREATE FUNCTION Period_Interval(Period)
RETURNS INTERVAL DAY(8) TO SECOND
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodSizeAsInterval)"
LANGUAGE C
DOCUMENT "Length(Period) returns the INTERVAL size of the Period.";
--
GRANT EXECUTE ON FUNCTION Period_Interval(Period) TO PUBLIC;
--
CREATE FUNCTION Start(Period)
RETURNS DATE
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodGetStart)"
LANGUAGE C
DOCUMENT "Start(Period) returns the start DATE of the interval. If the",
         " start is epoch, then this UDF returns NULL. ";
--
GRANT EXECUTE ON FUNCTION Start(Period) TO PUBLIC;
--
CREATE FUNCTION Finish(Period)
RETURNS DATE
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodGetFinish)"
LANGUAGE C
DOCUMENT "Finish(Period) returns the end date of the interval. If the",
         "finish is unbound, this UDF returns NULL. ";
--
GRANT EXECUTE ON FUNCTION Finish(Period) TO PUBLIC;
--
CREATE FUNCTION GetIntersect(Period, Period)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodGetInter)"
LANGUAGE C
DOCUMENT "GetIntersect(Period, Period) returns a Period that is",
         "the intersection of the two. If there is no intersection, ",
         "this function returns a NULL result.";
--
GRANT EXECUTE ON FUNCTION GetIntersect(Period, Period) TO PUBLIC;
--
CREATE FUNCTION GetUnion(Period, Period)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodGetUnion)"
LANGUAGE C
DOCUMENT "GetUnion(Period, Period) returns a Period that is ",
         " the union of the two arguments. ";
--
GRANT EXECUTE ON FUNCTION GetUnion(Period, Period) TO PUBLIC;
--
CREATE FUNCTION Before(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_BeforeNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodBeforeNotTouches)"
LANGUAGE C
DOCUMENT " Before(Period, Period) returns true iff. the first Period is",
         " entirely BEFORE the second. That is, iff Finish(Period1) < ",
         " Start(Period2).";
--
GRANT EXECUTE ON FUNCTION Before(Period, Period) TO PUBLIC;
--
CREATE FUNCTION BeforeTouches(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_BeforeTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodBeforeTouches)"
LANGUAGE C
DOCUMENT " BeforeTouches(Period, Period) returns true iff. the first",
         " Period ends at the start of the second. That is, iff",
	 " Finish(Period1) = Start(Period2).";
--
GRANT EXECUTE ON FUNCTION BeforeTouches(Period, Period) TO PUBLIC;
--
CREATE FUNCTION After(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_AfterNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodAfterNotTouches)"
LANGUAGE C
DOCUMENT "After(Period, Period) returns true iff. the first Period is",
         " entirely AFTER the second. That is, iff Finish(Period2) < ",
         " Start(Period1).";
--
GRANT EXECUTE ON FUNCTION After(Period, Period) TO PUBLIC;
--
CREATE FUNCTION AfterTouches(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	SELFUNC = Period_AfterTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodAfterTouches)"
LANGUAGE C
DOCUMENT "AfterTouches(Period, Period) returns true iff. the first Period ",
         " is entirely AFTER the second. That is, iff Finish(Period2) <",
         " Start(Period1).";
--
GRANT EXECUTE ON FUNCTION AfterTouches(Period, Period) TO PUBLIC;
--
-------------------------------- Overlap Aggregate -----------------------------
--
CREATE OPAQUE TYPE Period_Overlap_State ( internallength = 12 );
--
CREATE FUNCTION Period_Overlap_Init(Period)
RETURNS Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE, HANDLESNULLS
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapAggrInit)"
LANGUAGE C
DOCUMENT "Initialize support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION Period_Overlap_Init(Period) TO PUBLIC;
--
CREATE FUNCTION Period_Overlap_Iter (Period_Overlap_State, Period)
RETURNS Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapAggrIter)"
LANGUAGE C
DOCUMENT "Iterate support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION Period_Overlap_Iter( Period_Overlap_State, 
                                               Period ) TO PUBLIC;
--
CREATE FUNCTION Period_Overlap_Combine ( Period_Overlap_State, 
                                         Period_Overlap_State)
RETURNS Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapAggrComb)"
LANGUAGE C
DOCUMENT "Combine support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION Period_Overlap_Combine (Period_Overlap_State,
                                                  Period_Overlap_State ) 
TO PUBLIC;
--
CREATE FUNCTION Period_Overlap_Final (Period_Overlap_State)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapAggrFinal)"
LANGUAGE C
DOCUMENT "Finalize support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION Period_Overlap_Final (Period_Overlap_State) TO PUBLIC;
--
CREATE AGGREGATE Min_Period_Overlap WITH (
	INIT    = Period_Overlap_Init,
	ITER    = Period_Overlap_Iter,
	COMBINE = Period_Overlap_Combine,
	FINAL   = Period_Overlap_Final
);
--
--  NOTE: As of 9.21, there appears to be no need to GRANT priveleges on the 
--        aggregate.
--

------------------------------- Error Messages -------------------------------
--
CREATE FUNCTION CompareString(Period, Period)
RETURNS lvarchar
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(Period_X_to_String)"
LANGUAGE C
DOCUMENT "Diagnostic Function for Period";
--
GRANT EXECUTE ON FUNCTION CompareString(Period, Period) TO PUBLIC;
--
------------------------------ Check the load --------------------------------
--
EXECUTE FUNCTION Period ( "10/10/2000", "11/11/2000");
