
--
--  File:  register.sql
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
( 	internallength = 8, 	
	alignment = 8
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
DOCUMENT "Constructor function for Period. Valid external format is ",
         " date to date or simply date date. The first date must   ",
         "precede the second date.                                         ",
         " Example:  PeriodIn ( 04-01-1997 to 05-02-1997 )      ";
--
GRANT EXECUTE ON FUNCTION PeriodIn (lvarchar) TO PUBLIC;
--
CREATE IMPLICIT CAST (	lvarchar AS Period WITH  PeriodIn );
--
--   Out . . .
--
CREATE FUNCTION PeriodOut (Period) 
RETURNS lvarchar 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOutput)" 
LANGUAGE C
DOCUMENT "Output function for Period. Valid external format is      ",
         " date to date or simply date date.                       ",
         " Example:  04-01-1997 to 05-02-1997                          ";
--
GRANT EXECUTE ON FUNCTION PeriodOut (Period) TO PUBLIC;
--
CREATE IMPLICIT CAST ( Period AS lvarchar WITH PeriodOut ); 
--
--  Constructed from two integers . . . 
--
CREATE FUNCTION Period ( integer, integer )
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodIntInt)"
LANGUAGE C
DOCUMENT "Constructor function that takes two integer days. This is useful ",
         "for constructing random date ranges, because the internal format ",
         "for dates is integer days, with day 1 being 01-01-1901. The first",
         " integer must preceed (be smaller than) the second.              ";
--
GRANT EXECUTE ON FUNCTION Period(integer, integer) TO PUBLIC;
--
--  Constructed from two integers . . . 
--
CREATE FUNCTION Period ( date, date )
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodIntInt)"
LANGUAGE C
DOCUMENT "Constructor function that takes two dates. This is probably the ",
         "function used most in query processing. The first date must     ",
         "preceed the second.                                             ",
  	     " Example: Period ( 01-01-1997, 02-02-1998 )              ";
--
GRANT EXECUTE ON FUNCTION Period(date, date) TO PUBLIC;
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
CREATE FUNCTION PeriodImpT (impexp) 
RETURNS Period 
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME 
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodImportText)" 
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
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodExportText)"
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
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodImportBinary)"
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
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodExportBinary)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION PeriodExpB (Period) TO PUBLIC;
--
CREATE CAST ( Period AS impexpbin WITH  PeriodExpB );
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
	COMMUTATOR = Equal
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodEqual)"
LANGUAGE C
DOCUMENT "Equal( Period, Period ) returns true iff. the two ",
         "arguments have the same begin data and the same end date. The   ",
         "ORDBMS can use an R-tree index to accellerate this query.       ";
--
GRANT EXECUTE ON FUNCTION Equal (Period,Period) TO PUBLIC;
--
CREATE FUNCTION NotEqual (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE,
	NEGATOR = NotEqual,
	COMMUTATOR = Equal
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodNotEqual)"
LANGUAGE C
DOCUMENT "NotEqual( Period, Period ) returns true unless both ",
         "arguments have the same begin data and the same end date. The     ",
         "ORDBMS can use an R-tree index to accellerate this query but      ",
         "it would be unusual for this to be efficient.                     ";
--
GRANT EXECUTE ON FUNCTION NotEqual (Period,Period) TO PUBLIC;
--
------------------------------------------------------------------------------
--
--   Spatial Functions
--
------------------------------------------------------------------------------
--
CREATE FUNCTION Contains (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodContains)"
LANGUAGE C
DOCUMENT "Contains ( Period, Period ) returns true iff. the  ",
         "begin and end dates of the second argument are within the start  ",
         "and end dates of the first. i.e. Contains(A,B) is A Contains B.  ",
         "Note that within in this case includes equals; in other words,   ",
         "Contains includes Equal.                                         ";
--
GRANT EXECUTE ON FUNCTION Contains (Period,Period) TO PUBLIC;
--
CREATE FUNCTION Within (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodWithin)"
LANGUAGE C
DOCUMENT "Within ( Period, Period ) returns true iff. the   ",
         "begin and end dates of the first argument are within the start     ",
         "and end dates of the second. i.e. Within(A,B) is A Contained by ",
         "B. Note that within in this case includes equals; in other words,  ",
         "Within includes Equal.                                          ";
--
GRANT EXECUTE ON FUNCTION Within (Period,Period) TO PUBLIC;
--
--    These functions are used by the R-tree code.
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
         "of the first two arguments into the second argument.              ";
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
         "and returns this in the second argument.               ";
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
CREATE FUNCTION Overlap (Period,Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlap)"
LANGUAGE C
DOCUMENT "Overlap( Period, Period ) returns true iff. one of the",
         "dates in one of the arguments is between the dates of the other.   ";
--
GRANT EXECUTE ON FUNCTION Overlap (Period,Period) TO PUBLIC;
--
CREATE FUNCTION Overlap (Period,Period, OUT INTEGER)
RETURNS boolean
WITH (
    NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodOverlapRetVal)"
LANGUAGE C
DOCUMENT "Overlap( Period, Period, integer ) returns true iff. one of the",
         "dates in one of the arguments is between the dates of the other.  ",
         "The additional OUT paramater returns the number of days in the ",
         "overlap. This is used to combine the Indexing and the UDR.";
--
GRANT EXECUTE ON FUNCTION Overlap (Period,Period, INTEGER) TO PUBLIC;
--
--------------------------  OPCLASS -------------------------------------
--
CREATE OPCLASS Period_ops FOR RTREE
STRATEGIES( Overlap(Period, Period), 
            Equal(Period, Period), 
            Contains(Period, Period), 
            Within(Period, Period), 
            Overlap(Period, Period, INTEGER) )
SUPPORT ( Union, Size, Inter );                                 
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
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodCmp)"
LANGUAGE C;
--
GRANT EXECUTE ON FUNCTION Compare (Period, Period) TO PUBLIC;
--
--------------------------------  Interface Functions ------------------------
--
CREATE FUNCTION GetInts(Period)
RETURNS lvarchar
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodInt)"
LANGUAGE C
DOCUMENT "GetInts returns a string made up of two integers corresponding to ",
         "the integer days of the begin and end dates.                      ";
--
GRANT EXECUTE ON FUNCTION GetInts(Period) TO PUBLIC;
--
CREATE FUNCTION Length(Period)
RETURNS integer
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodLength)"
LANGUAGE C
DOCUMENT "Length(Period) returns the integer days in the Period.";
--
GRANT EXECUTE ON FUNCTION Length(Period) TO PUBLIC;
--
CREATE FUNCTION Start(Period)
RETURNS date
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodStart)"
LANGUAGE C
DOCUMENT "Start(Period) returns the start date of the interval.     ";
--
GRANT EXECUTE ON FUNCTION Start(Period) TO PUBLIC;
--
CREATE FUNCTION Finish(Period)
RETURNS date
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodFinish)"
LANGUAGE C
DOCUMENT "Finish(Period) returns the end date of the interval.     ";
--
GRANT EXECUTE ON FUNCTION Finish(Period) TO PUBLIC;
--
CREATE FUNCTION GetIntersect(Period, Period)
RETURNS Period
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodGetIntersect)"
LANGUAGE C
DOCUMENT "GetIntersect(Period, Period) returns a Period that is ",
         "the intersection of the two. If there is no intersection, this function ",
	 "will yield a NULL result. ";
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
         "the union of the two arguments. ";
--
GRANT EXECUTE ON FUNCTION GetUnion(Period, Period) TO PUBLIC;
--
CREATE FUNCTION Before(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodBefore)"
LANGUAGE C
DOCUMENT "Before(Period, Period) returns true iff. the first Period is entirely ",
         "BEFORE the second. That is, iff Finish(Period1) < Start(Period2).";
--
GRANT EXECUTE ON FUNCTION Before(Period, Period) TO PUBLIC;
--
CREATE FUNCTION After(Period, Period)
RETURNS boolean
WITH ( 
	NOT VARIANT, PARALLELIZABLE
)
EXTERNAL NAME
"$INFORMIXDIR/extend/Period/bin/Period.bld(PeriodAfter)"
LANGUAGE C
DOCUMENT "After(Period, Period) returns true iff. the first Period is entirely ",
         "AFTER the second. That is, iff Finish(Period2) < Start(Period1).";
--
GRANT EXECUTE ON FUNCTION After(Period, Period) TO PUBLIC;
--
------------------------------- Error Messages -------------------------------
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,1,'DT_INT1','en_us.1252','Connection has failed');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,2,'DT_INT2','en_us.1252','Memory allocation has failed ');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,3,'DT_INT3','en_us.1252','First date should not be later than second');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,4,'DT_INT4','en_us.1252','DT_INT4');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,5,'DT_INT5','en_us.1252','DT_INT5');
--
insert into syserrors
(level, seqno, sqlstate,  locale, message)
values
(0,6,'DT_INTB','en_us.1252','Input data format error');
--
------------------------------ Check the load --------------------------------
--
execute function Overlap('1/1/1995 to 1/1/1996'::Period, 
                         '6/1/1995 to 6/1/1996'::Period);
--
