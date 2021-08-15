/*
** Title:               Period.h
** SCCSid:              %W% %E% %U%
** CCid:                %W% %E% %U%
** Author:              
** Created:             Apr 30, 1997 04:16:35 PM
** Description:         Generated header file.
** Comments:            Generated for project Period.1.0.
*/

/*
**	Special Note: This file should not be  modified.
**	No merging is performed on this header file.  It
**	is regenerated each time the project is written.
*/

#ifndef HDR_Period_H
#define HDR_Period_H

/*
**  Configure tracing by setting TRACE_DEBUG_Period
**  to 0 to completely disable tracing or 1 to  enable
**  tracing.  This define may be set from the compiler
**  command line by using the -DTRACE_DEBUG_Period=0 flag.
*/
#ifndef TRACE_DEBUG_Period
#define TRACE_DEBUG_Period 1
#endif

#ifndef MIN
#define MIN(A,B) ((A)>(B)?(B):(A))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif


#ifndef DBDK_LOHSIZE
#define DBDK_LOHSIZE		sizeof(MI_LO_HANDLE)
#define DBDK_LOBINFNSIZE	DBDK_LOHSIZE
#endif

/* This data structure returned by LOhandles. */
typedef struct
{
	mi_integer	nlos;	/* Number of large object handles. */
	MI_LO_HANDLE	los[1];	/* Valid large object handles.     */
} MI_LO_HANDLES;

/*
**	Large object file name mask.  '?' is a wild-card that
**	is filled in when a large object is written to disk.
*/
#define LO_FN_MASK	"????????.lo"

/*	Error messages
**
**	English  versions  of  these error messages  are  automatically
**	added to the  syserrors  table as part of your DataBlade module
**	registration.  If you do  not  like the  default  messages, you
**	can create new errors  and  change  these  defines to use  your
**	new codes. You can not, however, change the text of the default
**	messages because they are shared by other DataBlade modules. 
*/
#define ERRORMESG1	"DTN01"
#define ERRORMESG2	"DTN02"
#define ERRORMESG3	"DTN03"
#define ERRORMESG4	"DTN04"
#define ERRORMESG5	"DTN05"
#define ERRORMESG6	"DTN06"
#define ERRORMESG7	"DTN07"
#define ERRORMESG8	"DTN08"
#define ERRORMESG9	"DTN09"
#define ERRORMESG10	"DTN0A"
#define ERRORMESG11	"DTN0B"
#define ERRORMESG12	"DTN0C"
#define ERRORMESG13	"DTN0D"
#define ERRORMESG14	"DTN0E"
#define ERRORMESG15	"DTN0F"
#define ERRORMESG16	"DTN0G"
#define ERRORMESG17	"DTN0H"
#define ERRORMESG18	"DTN0I"
#define ERRORMESG19	"DTN0J"

/* Use DBDK_TRACE to direct trace messages to the trace file. */
#if TRACE_DEBUG_Period
#define DBDK_TRACE		(1 << 16)
#else
#define DBDK_TRACE		0
#endif

/*
** Print a message to the trace file and for  the user.
** N.B.: This macro uses Gen_Con.  Your  function  must
**       declare Gen_Con as MI_CONNECTION * and  either
**       open the connection or set it to NULL.
*/
#define DBDK_TRACE_ERROR( Caller, ErrNo, ErrLevel )      \
                           Gen_Trace                     \
                           (                             \
                               Gen_Con,                  \
                               Caller,                   \
                               __FILE__,                 \
                               __LINE__,                 \
                               ErrNo,                    \
                               "Period",                                     \
                               ErrLevel,                 \
                               MI_SQL | DBDK_TRACE       \
                           );

/* Print a message to the trace file. */
#if TRACE_DEBUG_Period

/*
** Print a message to the trace file.
** N.B.: This macro uses Gen_Con.  Your  function  must
**       declare Gen_Con as MI_CONNECTION * and  either
**       open the connection or set it to NULL.
*/
#define DBDK_TRACE_MSG( Caller, ErrNo, ErrLevel )        \
                           Gen_Trace                     \
                           (                             \
                               Gen_Con,                  \
                               Caller,                   \
                               __FILE__,                 \
                               __LINE__,                 \
                               ErrNo,                    \
                               "Period",                                     \
                               ErrLevel,                 \
                               DBDK_TRACE                \
                           );

#else

#define DBDK_TRACE_MSG( Caller, ErrNo, ErrLevel )

#endif

/* These macros are used on entry to, and on exit from, a function. */
#define DBDK_TRACE_ENTER( Caller )  DBDK_TRACE_MSG( Caller, ERRORMESG13, 20 )
#define DBDK_TRACE_EXIT( Caller )   DBDK_TRACE_MSG( Caller, ERRORMESG14, 20 )

/*
**	Interval types.
*/
#define YEAR_TO_MONTH   1
#define DAY_TO_SECOND   2

/* Function prototypes. */
mi_integer Gen_nstrwords( gl_mchar_t *, mi_integer );
gl_mchar_t * Gen_sscanf
(
	MI_CONNECTION *                    Gen_Con,
	char *                             Gen_Caller,
	gl_mchar_t *                       Gen_InData,
	mi_integer                         Gen_InDataLen,
	mi_integer                         Gen_Width,
	char *                             Gen_Format,
	char *                             Gen_Result
);
void       Gen_LoadLOFromFile
(
	MI_CONNECTION *                    Gen_Con,
	char *                             Gen_Caller,
	char *                             Gen_LOFile,
	MI_LO_HANDLE *                     Gen_pLOh
);
void Gen_StoreLOToFile
(
	MI_CONNECTION *                    Gen_Con,
	char *                             Gen_Caller,
	char *                             Gen_LOFile,
	MI_LO_HANDLE *                     Gen_pLOh
);
void Gen_Trace
(
	MI_CONNECTION *                    Gen_Con,
	char *                             Gen_Caller,
	char *                             Gen_FileName,
	mi_integer                         Gen_LineNo,
	char *                             Gen_MsgNo,
	char *                             Gen_Class,
	mi_integer                         Gen_Threshold,
	mi_integer                         Gen_MsgType
);

/* BladeSmith 3.30.TG1 typedef Period */
/*
** Useful support functions implemented in support.c
**/
mi_integer ifx_int8mod ( ifx_int8_t * ,ifx_int8_t * ,ifx_int8_t * );
mi_integer ifx_int8toInterval ( ifx_int8_t *,mi_interval * );
mi_integer ifx_invtoInt8 ( mi_interval *, ifx_int8_t * );
/*
 * NOTE: mi_date is just int.
**/
typedef struct 
{
	mi_date                        start;
	mi_date                        finish;
} Period;

/*
**   This is the in-memory structure passed about to record the state
**  of Period Intersect aggregate.
*/
typedef struct
{
	Period		Period_Value;
	mi_integer	nStatus;
}
Period_Overlap_Aggr_State;

/*
**
**  This structure is used to store the statistics gathered to
**  record the distribution of Period data in a column. 
**  
*/
typedef struct
{
		mi_integer total;
		mi_integer null_cnt;
		mi_integer start_epoch;
		mi_integer finish_unbound;
		mi_integer closed_cnt;
		mi_date    min_start;
		mi_date    max_finish;
		mi_date    avg_start;
		mi_date    avg_finish;
} Period_Statistics;

/*
** 
*/

typedef struct
{
		mi_integer total;
		mi_integer null_cnt;
		mi_integer start_epoch;
		mi_integer finish_unbound;
		mi_integer closed_cnt;
		mi_date    min_start;
		mi_date    max_finish;
		mi_int8    sum_start;
		mi_int8    sum_finish;
} Period_Statistics_Internal;

/* Warning: Do not modify. Period checksum: 0 */

/*
** A set of utility routines for working with Period structures.
*/
Period * copy_Period ( Period *);
Period * new_Period ();

void Check_Period ( Period * );

mi_date	Period_Epoch();
mi_date Period_Forever();

mi_boolean Period_Start_is_Epoch ( Period *, MI_FPARAM * );
void Set_Period_Start_Epoch_internal ( Period *, MI_FPARAM * );
Period * Set_Period_Start_Epoch ( Period * , MI_FPARAM * );
mi_boolean Period_Finish_is_Unbound ( Period *, MI_FPARAM * );
void Set_Period_Finish_Unbound_internal ( Period *, MI_FPARAM * );
Period * Set_Period_Finish_Unbound ( Period *, MI_FPARAM * );

/*
**  This value indicates that this is not a normal datetime, but
**  that it is open ended. That is, when this value is found in
**  either start.dt_qual or finish.dt_qual it means that the 
**  value is not "real" but indicates that the Period is
**  unbounded.
**
**/
#define TU_START_EPOCH 14
#define TU_FINISH_UNBOUND 15

#define OPEN_START "EPOCH"
#define OPEN_END "FOREVER"

/*
**
**   DT_Period is the internal structure for the DT_Period OPAQUE
**  type.  
*/
typedef struct
{
       mi_datetime          start;
       mi_datetime          finish;
}
DT_Period;
/*
**   This is the in-memory structure passed about to record the state
**  of various aggregate DT_Period. 
*/
typedef struct
{
	DT_Period		DT_Period_Value;
	mi_integer		nStatus;
}
DT_Period_Overlap_Aggr_State;

/*
**   These structures are used to store the statistics gathered to
**  record the distribution of DT_Period data in a column. The
**  trick is that I need to find the average of a potentially very
**  large number of datetime values. Not easy. 
*/
typedef struct
{
		mi_integer	total;
		mi_integer  null_cnt;
		mi_integer	start_epoch;
		mi_integer	finish_unbound;
		mi_integer  closed_cnt;
		mi_datetime min_start;
		mi_int8		max_finish;
		mi_int8		avg_start;
		mi_int8		avg_finish;
} DT_Period_Statistics;
/*
**   So here's how its done. 
**
**   At every iteration, I have the min_start value, and I have the
**  total - ( start_epoch + finish_unbound ). So what I'll do is to
**  record the number of seconds between the min_start and each
**  new value. If I get a value that is smaller than min_start, I
**  will get the number of seconds between the old and new min_start,
**  multiply this by the number of counted rows (total - ( start_epoch
**  + finish_unbound ) ) and add this to sum_start and sum_finish.
*/

/*
**  DT_Period internal functions.
*/
DT_Period * copy_DT_Period ( DT_Period * );
DT_Period * new_DT_Period ();

void Check_DT_Period ( DT_Period * );

mi_boolean DT_Period_Start_is_Epoch ( DT_Period *, MI_FPARAM * );
/*
**  DT_Period internal functions.
*/
DT_Period * copy_DT_Period ( DT_Period * );
DT_Period * new_DT_Period ();

void Check_DT_Period ( DT_Period * );

mi_boolean DT_Period_Start_is_Epoch ( DT_Period *, MI_FPARAM * );
void Set_DT_Period_Start_Epoch_internal ( DT_Period *, MI_FPARAM *  );
DT_Period * Set_DT_Period_Start_Epoch ( DT_Period *, MI_FPARAM *  );


mi_boolean DT_Period_Finish_is_Unbound ( DT_Period *, MI_FPARAM *  );
void Set_DT_Period_Finish_Unbound_internal ( DT_Period *, MI_FPARAM *  );
DT_Period * Set_DT_Period_Finish_Unbound ( DT_Period *, MI_FPARAM *  );

mi_integer PeriodInter      ( Period *, Period *,Period *, MI_FPARAM * );
mi_integer PeriodSizeAsDays ( Period *, MI_FPARAM * );
/*
**
**   This is all a bit tricky. There are 18 possible relationships
**  between two pairs of integers, where the values in the 
**  pairs are constrained by the rule S < E. I have a marvelous
**  proof for this, but this narrow margin does not permit
**  me to list it here. (Actually, I wrote a program to figure it
**  out for me.)
**
**    These are the values returned by DT_Period_X_DT_Period 
**    
*/
#define P_CMP_ERROR 0   /* Invalid - throw exception    */
#define EQ_EQ_EQ_EQ 1   /* ( 1 -> 1 )   ( 1 -> 1 )  EQ  */
#define EQ_LT_EQ_LT 2   /* ( 1 -> 1 )   ( 1 -> 2 )  LT  */
#define LT_LT_LT_LT 3   /* ( 1 -> 1 )   ( 2 -> 2 )  LT  */
#define EQ_EQ_GT_GT 4   /* ( 1 -> 2 )   ( 1 -> 1 )  GT  */
#define EQ_LT_GT_EQ 5   /* ( 1 -> 2 )   ( 1 -> 2 )  EQ  */
#define EQ_LT_GT_LT 6   /* ( 1 -> 2 )   ( 1 -> 3 )  LT  */
#define LT_LT_EQ_EQ 7   /* ( 1 -> 2 )   ( 2 -> 2 )  LT  */
#define LT_LT_EQ_LT 8   /* ( 1 -> 2 )   ( 2 -> 3 )  LT  */
#define EQ_LT_GT_GT 9   /* ( 1 -> 3 )   ( 1 -> 2 )  GT  */
#define LT_LT_GT_GT 10  /* ( 1 -> 3 )   ( 2 -> 2 )  LT  */
#define LT_LT_GT_EQ 11  /* ( 1 -> 3 )   ( 2 -> 3 )  LT  */
#define LT_LT_GT_LT 12  /* ( 1 -> 3 )   ( 2 -> 4 )  LT  */
#define GT_GT_GT_GT 13  /* ( 2 -> 2 )   ( 1 -> 1 )  GT  */
#define GT_EQ_GT_EQ 14  /* ( 2 -> 2 )   ( 1 -> 2 )  GT  */
#define GT_LT_GT_LT 15  /* ( 2 -> 2 )   ( 1 -> 3 )  GT  */
#define GT_EQ_GT_GT 16  /* ( 2 -> 3 )   ( 1 -> 2 )  GT  */
#define GT_LT_GT_EQ 17  /* ( 2 -> 3 )   ( 1 -> 3 )  GT  */
#define GT_LT_GT_GT 18  /* ( 2 -> 4 )   ( 1 -> 3 )  GT  */     
/*
**
**  Instead of using a bunch of nasty nested thingies, I'll
** use a 4D array to look it up. Note that this little fellow
** was also generated by a program.
*/
#define RANGE_X_RET_VALS {{{{3,0,0},{8,7,0},{12,11,10}},{{0,0,0},{0,0,0},{0,0,0}},{{0,0,0},{0,0,0},{0,0,0}}},{{{0,0,0},{2,0,0},{6,5,9}},{{0,0,0},{0,1,0},{0,0,4}},{{0,0,0},{0,0,0},{0,0,0}}},{{{0,0,0},{0,0,0},{15,17,18}},{{0,0,0},{0,0,0},{0,14,16}},{{0,0,0},{0,0,0},{0,0,13}}}};

mi_integer DT_dtcmp ( mi_datetime * , mi_datetime *  );
mi_integer DT_Period_X_DT_Period ( DT_Period *, DT_Period * );
DT_Period * DT_Period_DT_DT (mi_datetime * ,mi_datetime * ,MI_FPARAM * );
mi_integer DT_PeriodInter (	DT_Period *, DT_Period *, DT_Period *,MI_FPARAM * );

ifx_int8_t * DT_PeriodSizeAsSeconds ( DT_Period *, MI_FPARAM * );
mi_interval * DT_PeriodSizeAsInterval ( DT_Period *, MI_FPARAM * );

#define NOT(X) ( ((X)==((mi_boolean)1))?((mi_boolean) 0):((mi_boolean) 1))

#endif
