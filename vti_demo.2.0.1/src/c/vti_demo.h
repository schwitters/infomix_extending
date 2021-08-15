/*
** Title:          vti_demo
** SCCSid:         %W% %E% %U%
** CCid:           %W% %E% %U%
** Author:         Informix
** Created:        08/21/1998 09:00
** Description:    This is the generated 'C' file for the vti_demo DataBlade.
** Comments:       Generated for project vti_demo.2.0
*/


/*
**	Special Note: This file should not be  modified.
**	No merging is performed on this header file.  It
**	is regenerated each time the project is written.
*/

#ifndef HDR_vti_demo_H
#define HDR_vti_demo_H

/*
**  Configure tracing by setting TRACE_DEBUG_vti_demo
**  to 0 to completely disable tracing or 1 to  enable
**  tracing.  This define can be set from the compiler
**  command line by using the -DTRACE_DEBUG_vti_demo=0 flag.
*/
#ifndef TRACE_DEBUG_vti_demo
#define TRACE_DEBUG_vti_demo 1
#endif

#ifndef DBDK_LOHSIZE
#define DBDK_LOHSIZE		sizeof(MI_LO_HANDLE)
#define DBDK_LOBINFNSIZE	DBDK_LOHSIZE
#endif

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
#define ERRORMESG1	"UGEN1"
#define ERRORMESG2	"UGEN2"
#define ERRORMESG3	"UGEN3"
#define ERRORMESG4	"UGEN4"
#define ERRORMESG5	"UGEN5"
#define ERRORMESG6	"UGEN6"
#define ERRORMESG7	"UGEN7"
#define ERRORMESG8	"UGEN8"
#define ERRORMESG9	"UGEN9"
#define ERRORMESG10	"UGENA"
#define ERRORMESG11	"UGENB"
#define ERRORMESG12	"UGENC"
#define ERRORMESG13	"UGEND"
#define ERRORMESG14	"UGENE"
#define ERRORMESG15	"UGENF"
#define ERRORMESG16	"UGENG"
#define ERRORMESG17	"UGENH"
#define ERRORMESG18	"UGENI"
#define ERRORMESG19	"UGENJ"

/* Use DBDK_TRACE to direct trace messages to the trace file. */
#if TRACE_DEBUG_vti_demo
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
#define DBDK_TRACE_ERROR( Caller, ErrNo, ErrLevel )             \
                           Gen_Trace                            \
                           (                                    \
                               Gen_Con,                         \
                               Caller,                          \
                               __FILE__,                        \
                               __LINE__,                        \
                               ErrNo,                           \
                               "vti_demo",                      \
                               ErrLevel,                        \
                               MI_SQL | DBDK_TRACE              \
                           );

/* Print a message to the trace file. */
#if TRACE_DEBUG_vti_demo

/*
** Print a message to the trace file.
** N.B.: This macro uses Gen_Con.  Your  function  must
**       declare Gen_Con as MI_CONNECTION * and  either
**       open the connection or set it to NULL.
*/
#define DBDK_TRACE_MSG( Caller, ErrNo, ErrLevel )               \
                           Gen_Trace                            \
                           (                                    \
                               Gen_Con,                         \
                               Caller,                          \
                               __FILE__,                        \
                               __LINE__,                        \
                               ErrNo,                           \
                               "vti_demo",                      \
                               ErrLevel,                        \
                               DBDK_TRACE                       \
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

/*
**	UDREXPORT is normally used to export a function from the DataBlade when
**	linking on NT.  UNIX source files should maintain this define in source
**	for use when porting back to NT.
*/
#ifndef UDREXPORT
#define UDREXPORT
#endif

/* Function prototypes. */
mi_integer Gen_nstrwords( gl_mchar_t *, mi_integer );
gl_mchar_t * Gen_sscanf
(
	MI_CONNECTION *      Gen_Con,
	char *               Gen_Caller,
	gl_mchar_t *         Gen_InData,
	mi_integer           Gen_InDataLen,
	mi_integer           Gen_Width,
	char *               Gen_Format,
	char *               Gen_Result
);
void       Gen_LoadLOFromFile
(
	MI_CONNECTION *      Gen_Con,
	char *               Gen_Caller,
	char *               Gen_LOFile,
	MI_LO_HANDLE *       Gen_pLOh
);
void Gen_StoreLOToFile
(
	MI_CONNECTION *      Gen_Con,
	char *               Gen_Caller,
	char *               Gen_LOFile,
	MI_LO_HANDLE *       Gen_pLOh
);
void Gen_Trace
(
	MI_CONNECTION *      Gen_Con,
	char *               Gen_Caller,
	char *               Gen_FileName,
	mi_integer           Gen_LineNo,
	char *               Gen_MsgNo,
	char *               Gen_Class,
	mi_integer           Gen_Threshold,
	mi_integer           Gen_MsgType
);

/*
** Demo-specific variables
*/
 
/* 
** enumerated type
*/
typedef enum demo_col_types
{
        DEMO_INT = 1,
        DEMO_CHAR
} demo_col_type;
 
/*
** structure for demo_data
*/
typedef struct demo_data
{
        MI_DATUM        *data;       /* Data for a row                       */
        mi_string	*tab_path;   /* Pathname for the external table      */
        mi_string	*fifo_path;  /* Path for the client's FIFO           */
        mi_integer      fifo_fd;     /* File descriptor for the client FIFO  */
        mi_integer      ext_fd;      /* External data server's FIFO fd       */
        mi_integer      ncols;       /* Number of columns used               */
        demo_col_type   col_type[10];
        mi_boolean      is_null[10];
} demo_data_t;

/*
** structure for the fifo record
*/
typedef struct fifo_rec
{
        mi_string	*fifo_path;  /* file path for the FIFO           */
        mi_integer      fifo_fd;     /* file descriptor for the FIFO     */
} fifo_t;

#endif
