
--
--  File:  reg_dt_period.sql
--
-- About:
--
--   This is the register.sql file, extracted at great labour, from
--  the object.sql. This CREATEs the types and so on, including
--  the opclass, in keeping WITH the interdiction of the docs.
-- 
-----------------------------  Type ---------------------------------------
--
CREATE OPAQUE TYPE DT_Period
( 	internallength = 48, 	
	alignment = 8
);
--
GRANT USAGE ON TYPE DT_Period TO PUBLIC; 
--
---------------- Constructor Functions -------------------------------------
--
--   The in . . . .d
--
CREATE FUNCTION DT_PeriodIn (lvarchar) 
RETURNS DT_Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodInput)"
LANGUAGE C
DOCUMENT "Constructor function for DT_Period. Valid external format is ",
         "DATETIME to DATETIME or simply DATETIME DATETIME. The first ",
         " must precede the second date. Note also that there are two  ",
         " special valuesi: EPOCH and FOREVER. These indicate that the ",
         " interval is open at either the beginning or at the end.     ",
         "Example: DT_PeriodIn('1997-4-1 12:00:01 to 1997-4-1 12:05:00')";
--
GRANT EXECUTE ON FUNCTION DT_PeriodIn (lvarchar) TO PUBLIC;
--
CREATE IMPLICIT CAST ( lvarchar AS DT_Period WITH  DT_PeriodIn );
--
--   and the out.
--
CREATE FUNCTION DT_PeriodOut (DT_Period) 
RETURNS lvarchar 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOutput)" 
LANGUAGE C
DOCUMENT " Output function for DT_Period. Valid external format is     ",
         " DATETIME to DATETIME or simply DATETIME DATETIME . The first ",
         " must precede the second. Note also that there are two       ",
         " special values: EPOCH and FOREVER. These indicate that the  ",
         " interval is open at either the beginning or at the end.     ",
         " Example: '1997-4-1 12:00:01 to 1997-4-1 12:05:00'           ";
--
GRANT EXECUTE ON FUNCTION DT_PeriodOut (DT_Period) TO PUBLIC;
--
CREATE IMPLICIT CAST ( DT_Period AS lvarchar WITH DT_PeriodOut ); 
--
--  Constructed from two DATETIME YEAR TO SECOND values.
--
CREATE FUNCTION DT_Period ( DATETIME YEAR TO SECOND, DATETIME YEAR TO SECOND )
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_DT_DT)"
LANGUAGE C
DOCUMENT "Constructor function that takes two DATETIMEs. This is the  ",
         "workhorse constructor because getting the public format     ",
         "right is tricky.                                            ";
--
GRANT EXECUTE ON FUNCTION DT_Period(DATETIME YEAR TO SECOND, 
                                    DATETIME YEAR TO SECOND) 
TO PUBLIC;
--
--  This is simply an SPL function that shims the one above.
--
CREATE FUNCTION DT_Period ( Arg1 DATETIME YEAR TO SECOND )
RETURNING DT_Period
	RETURN DT_Period ( Arg1, Arg1 );
END FUNCTION;
--
GRANT EXECUTE ON FUNCTION DT_Period ( DATETIME YEAR TO SECOND ) TO PUBLIC;
--
CREATE CAST ( DATE AS DT_Period WITH DT_Period );
--
-----------------------------------  Supports -------------------------------
--
--  Binary Send and Receive functions.
--
CREATE FUNCTION DT_PeriodSend (DT_Period) 
RETURNS sendrecv 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodSend)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodSend (DT_Period) TO PUBLIC;
--
CREATE CAST  ( DT_Period AS sendrecv WITH  DT_PeriodSend );
--
CREATE FUNCTION DT_PeriodRecv (sendrecv) 
RETURNS DT_Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodReceive)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodRecv (sendrecv) TO PUBLIC;
-- 
CREATE IMPLICIT CAST ( sendrecv AS DT_Period WITH DT_PeriodRecv );
--
--  These are for the UNLOAD/RELOAD and the backup/recover stuff. 
--  Note that I re-use the Literal Expression UDFs here.
--
CREATE FUNCTION DT_PeriodImpT (impexp) 
RETURNS DT_Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodInput)" 
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodImpT (impexp) TO PUBLIC;
--
CREATE IMPLICIT CAST ( impexp AS DT_Period WITH  DT_PeriodImpT );
--
CREATE FUNCTION DT_PeriodExpT (DT_Period)
RETURNS impexp
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOutput)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodExpT (DT_Period) TO PUBLIC;
--
CREATE CAST ( DT_Period AS impexp WITH DT_PeriodExpT );
--
CREATE FUNCTION DT_PeriodImpB (impexpbin)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodReceive)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodImpB (impexpbin) TO PUBLIC;
--
CREATE IMPLICIT CAST ( impexpbin AS DT_Period WITH DT_PeriodImpB );
--
CREATE FUNCTION DT_PeriodExpB (DT_Period)
RETURNS impexpbin
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodSend)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION DT_PeriodExpB (DT_Period) TO PUBLIC;
--
CREATE CAST ( DT_Period AS impexpbin WITH  DT_PeriodExpB );
--
-----------------------------------------------------------------------------
--
--      Useful Functions
--
-----------------------------------------------------------------------------
--
CREATE FUNCTION Set_Start_Epoch (DT_Period)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_Set_Start_Epoch)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Set_Start_Epoch(DT_Period) TO PUBLIC;
--
CREATE FUNCTION Start_Is_Epoch (DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_Start_is_Epoch)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Start_Is_Epoch(DT_Period) TO PUBLIC;
--
CREATE FUNCTION Set_Finish_Forever (DT_Period)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_Set_Finish_Unbound)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Set_Finish_Forever (DT_Period) TO PUBLIC;
--
CREATE FUNCTION Finish_Is_Forever (DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_Finish_is_Unbound)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Finish_Is_Forever (DT_Period) TO PUBLIC;
--
-------------------------------- Statistics ----------------------------------
--
CREATE FUNCTION StatCollect ( DT_Period, double precision, double precision )
RETURNS LVARCHAR
WITH ( 
	HANDLESNULLS, NOT VARIANT 
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodStatCollect)"
LANGUAGE C
DOCUMENT "Statistics for the DT_Period Data Type.";
--
GRANT EXECUTE ON FUNCTION StatCollect ( DT_Period, double precision, double precision )
TO PUBLIC;
--
CREATE FUNCTION StatPrint ( DT_Period, stat )
RETURNS LVARCHAR
WITH ( 
	HANDLESNULLS, NOT VARIANT 
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodStatPrint)"
LANGUAGE C
DOCUMENT "Print out statistics for the DT_Period Data Type.";
--
GRANT EXECUTE ON FUNCTION StatPrint ( DT_Period, stat ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodEqual_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodEqualSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for Equal( DT_Period, DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodEqual_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodNotEqual_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodNotEqualSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for NotEqual(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodNotEqual_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodContainsNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodContainsNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for ContainsNotTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodContainsNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodContainsTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodContainsTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for ContainsTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodContainsTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodWithinTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodWithinTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for WithinTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodWithinTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodWithinNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodWithinNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for WithinNotTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodWithinNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodOverlapTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for OverlapTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodOverlapTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodOverlapNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for OverlapNotTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodOverlapNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodBeforeTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodBeforeTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for BeforeTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodBeforeTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodBeforeNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodBeforeNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for BeforeNotTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodBeforeNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodAfterTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodAfterTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for AfterTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodAfterTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
CREATE FUNCTION DT_PeriodAfterNotTouches_Selectivity ( POINTER, POINTER )
RETURNS DOUBLE PRECISION
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodAfterNotTouchesSelectivity)"
LANGUAGE C
DOCUMENT "Calculate selectivity for AfterNotTouches(DT_Period,DT_Period ).";
--
GRANT EXECUTE ON FUNCTION DT_PeriodAfterNotTouches_Selectivity ( POINTER, POINTER ) TO PUBLIC;
--
-------------------------  Operators ----------------------------------------
--
--    These are really only used internally. There is no way to 
--  sort these things in a meaningful way, but you need Compare() and
--  Equal() for things like UNION and DISTINCT.
--
CREATE FUNCTION Equal (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	NEGATOR    = NotEqual,
	COMMUTATOR = Equal,
	SELFUNC    = DT_PeriodEqual_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodEqual)"
LANGUAGE C
DOCUMENT "Equal( DT_Period, DT_Period ) returns true iff. the two ",
         "arguments have the same start DATETIME and the same end DATETIME.",
         "The ORDBMS can use an R-tree index to accellerate this query. ";
--
GRANT EXECUTE ON FUNCTION Equal (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION NotEqual (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	NEGATOR    = NotEqual,
	COMMUTATOR = Equal,
	SELFUNC    = DT_PeriodNotEqual_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodNotEqual)"
LANGUAGE C
DOCUMENT "NotEqual( DT_Period, DT_Period ) returns true unless both   ",
         "arguments have the same start date and the same finish date. The  ",
         "ORDBMS can use an R-tree index to accellerate this query but      ",
         "it would be unusual for this to be efficient.                     ";
--
GRANT EXECUTE ON FUNCTION NotEqual (DT_Period,DT_Period) TO PUBLIC;
--
--------------------------------------------------------------------------------
--
--    These functions are used by the R-tree opclass.
--
--------------------------------------------------------------------------------
--
CREATE FUNCTION Union (DT_Period,DT_Period,DT_Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodUnion)"
LANGUAGE C
DOCUMENT "Union(DT_Period, DT_Period, DT_Period) places a Union ",
         "of the first two arguments into the second argument.              ";
--
GRANT EXECUTE ON FUNCTION Union (DT_Period,DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION Size (DT_Period, double precision)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodSize)"
LANGUAGE C
DOCUMENT "Size(DT_Period, double) calculates the size of the first argument,",
         "and returns this in the second argument.               ";
--
GRANT EXECUTE ON FUNCTION Size (DT_Period,double precision) TO PUBLIC;
--
CREATE FUNCTION Inter (DT_Period,DT_Period,DT_Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodInter)"
LANGUAGE C
DOCUMENT "Inter(DT_Period, DT_Period, DT_Period) places a value ",
         "that is the intersection between the two arguments into the third.";
--
GRANT EXECUTE ON FUNCTION Inter (DT_Period,DT_Period,DT_Period) TO PUBLIC;
--
------------------------------------------------------------------------------
--
--   Spatial Functions
--
------------------------------------------------------------------------------
--
CREATE FUNCTION ContainsNotTouches (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodContainsNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodContainsNotTouches)"
LANGUAGE C
DOCUMENT "ContainsNotTouches( DT_Period, DT_Period ) returns true iff. the  ",
         "start and finish DATETIMEs of the second argument are within the  ",
         "start and finish DATETIMEs of the first. ContainsNotTouches(A,B)  ",
         "is A Contains B completely.                                       ",
         "Note that within in this case excludes equals; in other words,    ",
         "ContainsNotTouches does not Equal, and that touches does not count",
	     "as contains.                                                      ";
--
GRANT EXECUTE ON FUNCTION ContainsNotTouches (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION Contains (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodContainsTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodContainsTouches)"
LANGUAGE C
DOCUMENT "Contains ( DT_Period, DT_Period ) returns true iff. the start and ",
         "finish DATETIME of the second argument are within the start and   ",
         "finish DATETIME of the first. i.e. Contains (A,B) is A Contains   ",
	 "Contains B.                                                       ",
         "Note that within in this case includes equals; in other words,    ",
         "Equal(DT_Period, DT_Period ) implies Contains(DT_Period,DT_Period)",
	 " and that touches counts as contains.                             ";
--
GRANT EXECUTE ON FUNCTION Contains (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION WithinNotTouches (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodWithinNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodWithinNotTouches)"
LANGUAGE C
DOCUMENT "WithinNotTouches( DT_Period, DT_Period ) returns true iff. the   ",
         "start and finish DATETIME of the first argument are within the  ",
	 "start and finish DATETIME of the second. WithinNotTouches(A,B)  ",
	 "is B Contains A completely. Within, in this case, excludes Equal;",
         "in other words, touches at either end do not count.              ";
--
GRANT EXECUTE ON FUNCTION WithinNotTouches (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION Within (DT_Period,DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodWithinTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodWithinTouches)"
LANGUAGE C
DOCUMENT "Within ( DT_Period, DT_Period ) returns true iff. the start and  ",
         "finish DATETIME of the first argument are within the start      ",
         "and finish DATETIME of the second. i.e. Within(A,B) is B        ",
         "Contains A. Note that within in this case includes equals;       ",
         "in other words, touches at either end count.                     ";
--
GRANT EXECUTE ON FUNCTION Within (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION OverlapNotTouches (DT_Period, DT_Period )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodOverlapNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapNotTouches)"
LANGUAGE C
DOCUMENT "OverlapNotTouches( DT_Period, DT_Period ) returns true iff. the    ",
         "start or the finish of either of argument is between the start/    ",
	 "finish of the other, and neither end touches.                      ";
--
GRANT EXECUTE ON FUNCTION OverlapNotTouches (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION OverlapNotTouches (DT_Period, DT_Period, OUT INT8 )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodOverlapNotTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapNotTouchesWithLength)"
LANGUAGE C
DOCUMENT "OverlapNotTouches( DT_Period, DT_Period, OUT INT8 )  ",
         "returns true iff. the start or the finish of either of arguments   ",
	 "is complete between the start/finish of the other, and neither end ",
	 "touches. In addition, the OUT paramater returns the size of the    ",
	 "Overlap as some number of seconds.                                 ";
--
GRANT EXECUTE ON FUNCTION OverlapNotTouches(DT_Period,DT_Period,INT8)
TO PUBLIC;
--
CREATE FUNCTION Overlap(DT_Period, DT_Period )
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodOverlapTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapTouches)"
LANGUAGE C
DOCUMENT "Overlap( DT_Period, DT_Period ) returns true iff. the ",
         "start or the finish of either of argument is between the start/ ",
	 "finish of the other. Touches count. This is the most conventional ",
	 "broad definition of Overlap(). This UDR is the important one used ",
	 "in the R-Tree indexing data structure.";
--
GRANT EXECUTE ON FUNCTION Overlap (DT_Period,DT_Period) TO PUBLIC;
--
CREATE FUNCTION Overlap (DT_Period,DT_Period, OUT INT8 )
RETURNS boolean
WITH (
    NOT VARIANT, PARALLELIZABLE,	
	SELFUNC    = DT_PeriodOverlapTouches_Selectivity
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapTouchesWithLength)"
LANGUAGE C
DOCUMENT "Overlap( DT_Period, DT_Period, OUT INT8 ) returns true iff. one ",
         "of the DATETIME in one of the arguments is between the datestimes ",
	 "of the other.The additional OUT paramater returns an INT8 for ",
         "seconds in the overlap. This is used to combine the Indexing and ",
         "the UDR.";
--
GRANT EXECUTE ON FUNCTION Overlap (DT_Period, DT_Period, INT8) TO PUBLIC;
--
--------------------------  OPCLASS -------------------------------------
--
CREATE OPCLASS DT_Period_ops FOR RTREE
STRATEGIES( Overlap(DT_Period, DT_Period), 
            Equal(DT_Period, DT_Period), 
            Contains(DT_Period, DT_Period), 
            Within(DT_Period, DT_Period), 
            Overlap(DT_Period, DT_Period, INT8 ),
	        ContainsNotTouches(DT_Period, DT_Period), 
            WithinNotTouches(DT_Period, DT_Period), 
	        OverlapNotTouches(DT_Period, DT_Period),
	        OverlapNotTouches(DT_Period, DT_Period, INT8 ),
			BeforeTouches( DT_Period, DT_Period ),
			AfterTouches ( DT_Period, DT_Period )
)
SUPPORT ( Union, Size, Inter );  
--
CREATE FUNCTION rtreeInfo(DT_Period,pointer,pointer,pointer) 
RETURNS INTEGER
WITH ( NOT VARIANT )
EXTERNAL NAME '$INFORMIXDIR/extend/Period/bin/Period.bld(D_DT_RtreeInfo)'
LANGUAGE C;
                               
--
--  Note: The way this is used is to specify the opclass as part of the
--        create index statement. For example:
--
-- CREATE INDEX Res_1 ON Reserves(Reserv DT_Period_ops) USING rtree;
--
----------------------- Non-Spatial Support Functions ---------------------
--
CREATE FUNCTION Hash(DT_Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodHash)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Hash (DT_Period) TO PUBLIC;
--
--  This is only used when you need to discard duplicates as part of a
--  UNION query.
--
CREATE FUNCTION Compare(DT_Period,DT_Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodCompare)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Compare (DT_Period, DT_Period) TO PUBLIC;
--
--------------------------------  Interface Functions ------------------------
--
CREATE FUNCTION Length(DT_Period)
RETURNS INT8
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodSizeAsSeconds)"
LANGUAGE C
DOCUMENT "Length(DT_Period) returns the INT8 seconds in the DT_Period.";
--
GRANT EXECUTE ON FUNCTION Length(DT_Period) TO PUBLIC;
--
CREATE FUNCTION Period_Interval(DT_Period)
RETURNS INTERVAL DAY(8) TO SECOND
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodSizeAsInterval)"
LANGUAGE C
DOCUMENT "Length(DT_Period) returns the INTERVAL size of the DT_Period.";
--
GRANT EXECUTE ON FUNCTION Period_Interval(DT_Period) TO PUBLIC;
--
CREATE FUNCTION Start(DT_Period)
RETURNS DATETIME YEAR TO SECOND
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodGetStart)"
LANGUAGE C
DOCUMENT "Start(DT_Period) returns the start DATETIME of the interval. If the",
         " start is epoch, then this UDF returns NULL. ";
--
GRANT EXECUTE ON FUNCTION Start(DT_Period) TO PUBLIC;
--
CREATE FUNCTION Finish(DT_Period)
RETURNS DATETIME YEAR TO SECOND
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodGetFinish)"
LANGUAGE C
DOCUMENT "Finish(DT_Period) returns the end date of the interval. If the ",
         "finish is unbound, this UDF returns NULL. ";
--
GRANT EXECUTE ON FUNCTION Finish(DT_Period) TO PUBLIC;
--
CREATE FUNCTION GetIntersect(DT_Period, DT_Period)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodGetInter)"
LANGUAGE C
DOCUMENT "GetIntersect(DT_Period, DT_Period) returns a DT_Period that is ",
         "the intersection of the two. If there is no intersection, this function ",
	     "will yield a NULL result. ";
--
GRANT EXECUTE ON FUNCTION GetIntersect(DT_Period, DT_Period) TO PUBLIC;
--
CREATE FUNCTION GetUnion(DT_Period, DT_Period)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodGetUnion)"
LANGUAGE C
DOCUMENT "GetUnion(DT_Period, DT_Period) returns a DT_Period that is ",
         "the union of the two arguments. ";
--
GRANT EXECUTE ON FUNCTION GetUnion(DT_Period, DT_Period) TO PUBLIC;
--
CREATE FUNCTION Before(DT_Period, DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodBeforeNotTouches)"
LANGUAGE C
DOCUMENT "Before(DT_Period, DT_Period) returns true iff. the first DT_Period is entirely ",
         "BEFORE the second. That is, iff Finish(DT_Period1) < Start(DT_Period2).";
--
GRANT EXECUTE ON FUNCTION Before(DT_Period, DT_Period) TO PUBLIC;
--
CREATE FUNCTION BeforeTouches(DT_Period, DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodBeforeTouches)"
LANGUAGE C
DOCUMENT "BeforeTouches(DT_Period, DT_Period) returns true iff. the first ",
         "DT_Period ends at the start of the second. That is, iff ",
		 "Finish(DT_Period1) = Start(DT_Period2).";
--
GRANT EXECUTE ON FUNCTION BeforeTouches(DT_Period, DT_Period) TO PUBLIC;
--
CREATE FUNCTION After(DT_Period, DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodAfterNotTouches)"
LANGUAGE C
DOCUMENT "After(DT_Period, DT_Period) returns true iff. the first DT_Period is entirely ",
         "AFTER the second. That is, iff Finish(DT_Period2) < Start(DT_Period1).";
--
GRANT EXECUTE ON FUNCTION After(DT_Period, DT_Period) TO PUBLIC;
--
CREATE FUNCTION AfterTouches(DT_Period, DT_Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodAfterTouches)"
LANGUAGE C
DOCUMENT "AfterTouches(DT_Period, DT_Period) returns true iff. the first DT_Period is entirely ",
         "AFTER the second. That is, iff Finish(DT_Period2) < Start(DT_Period1).";
--
GRANT EXECUTE ON FUNCTION AfterTouches(DT_Period, DT_Period) TO PUBLIC;
--
-------------------------------- Overlap Aggregate -----------------------------
--
CREATE OPAQUE TYPE DT_Period_Overlap_State ( internallength = 52 );
--
CREATE FUNCTION DT_Period_Overlap_Init(DT_Period)
RETURNS DT_Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE, HANDLESNULLS
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapAggrInit)"
LANGUAGE C
DOCUMENT "Initialize support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION DT_Period_Overlap_Init(DT_Period) TO PUBLIC;
--
CREATE FUNCTION DT_Period_Overlap_Iter (DT_Period_Overlap_State, DT_Period)
RETURNS DT_Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapAggrIter)"
LANGUAGE C
DOCUMENT "Iterate support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION DT_Period_Overlap_Iter(DT_Period_Overlap_State, DT_Period) TO PUBLIC;
--
CREATE FUNCTION DT_Period_Overlap_Combine ( DT_Period_Overlap_State, 
                                            DT_Period_Overlap_State)
RETURNS DT_Period_Overlap_State
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapAggrComb)"
LANGUAGE C
DOCUMENT "Combine support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION DT_Period_Overlap_Combine (DT_Period_Overlap_State,
                                                  DT_Period_Overlap_State ) TO PUBLIC;
--
CREATE FUNCTION DT_Period_Overlap_Final (DT_Period_Overlap_State)
RETURNS DT_Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_PeriodOverlapAggrFinal)"
LANGUAGE C
DOCUMENT "Finalize support function for Overlap() aggregate.";
--
GRANT EXECUTE ON FUNCTION DT_Period_Overlap_Final (DT_Period_Overlap_State) TO PUBLIC;
--
--
CREATE AGGREGATE Min_Overlap WITH (
	INIT    = DT_Period_Overlap_Init,
	ITER    = DT_Period_Overlap_Iter,
	COMBINE = DT_Period_Overlap_Combine,
	FINAL   = DT_Period_Overlap_Final
);
--
--  NOTE: As of 9.21, there appears to be no need to GRANT priveleges on the 
--        aggregate.
--
-------------------------------- Statistics ----------------------------------
--

--
------------------------------- Error Messages -------------------------------
--
CREATE FUNCTION CompareString(DT_Period, DT_Period)
RETURNS lvarchar
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(DT_Period_X_to_String)"
LANGUAGE C
DOCUMENT "Diagnostic Function for DT_Period";
--
GRANT EXECUTE ON FUNCTION CompareString(DT_Period, DT_Period) TO PUBLIC;
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,1,'DTN01','en_us.8859','Connection has failed');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,2,'DTN02','en_us.8859','Memory allocation has failed ');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,3,'DTN03','en_us.8859','First date or datetime should not be later than second');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,4,'DTN04','en_us.8859','First does not Overlap() Second');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,5,'DTN05','en_us.8859','Error calculating Interval or Size of Period');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,6,'DTN0B','en_us.8859','Input data format error');
--
------------------------------ Check the load --------------------------------
--
EXECUTE FUNCTION DT_Period ( "2000-10-10 12:10:10", "2000-10-11 12:10:10");
