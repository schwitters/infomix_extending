/*
** Project include file for SqlLibC.1.0.
*/

#ifndef HDR_SqlLibC_H
#define HDR_SqlLibC_H

#include <string.h> /* contains function prototype for strstr */
#include <stdlib.h> /* contains function prototype for strstr */
#include <math.h>   /* contains function prototype for ceil */
#include <limits.h> /* defines INT_MIN and INT_MAX */
#include <ifxgls.h> /* Informix GLS Library */
#include <mi.h>     /* DataBlade API */

#define SQL_TRACE_CLASS "SqlLibC"
#define SQL_TRACE_LIGHT  10

#define SQL_TRACE_ENTER(fn) \
    GL_DPRINTF(SQL_TRACE_CLASS, SQL_TRACE_LIGHT,\
        ("SqlLibC_UDR_ENTER", "UDRNAME%s", fn, MI_LIST_END))

#define SQL_TRACE_EXIT(fn)  \
    GL_DPRINTF(SQL_TRACE_CLASS, SQL_TRACE_LIGHT,\
        ("SqlLibC_UDR_EXIT", "UDRNAME%s", fn, MI_LIST_END))
         

/*
**    UDREXPORT is normally used to export a function from the DataBlade when
**    linking on NT.  UNIX source files should maintain this define in source
**    for use when porting back to NT.
*/
#ifndef UDREXPORT
#define UDREXPORT
#endif

/* ======================= Function prototypes =============================== */

UDREXPORT mi_integer  SqlLibC_ascii 
        (mi_lvarchar *char_in, MI_FPARAM *Gen_fparam);
UDREXPORT mi_integer  SqlLibC_ceil 
        (mi_double_precision *input, MI_FPARAM *Gen_fparam);
UDREXPORT mi_lvarchar *SqlLibC_chr 
        (mi_integer ascii_input, MI_FPARAM *Gen_fparam);
UDREXPORT mi_integer  SqlLibC_sign 
        (mi_double_precision *input, MI_FPARAM *Gen_fparam);

mi_integer SqlLibC_instrb
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        mi_integer occurrence, MI_FPARAM *Gen_fparam);

mi_integer SqlLibC_instrb_back
		(mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer   start,
		mi_integer   occurrence, MI_FPARAM   *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instrb_2
        (mi_lvarchar *search_str, mi_lvarchar *search_val, 
        MI_FPARAM *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instrb_3
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        MI_FPARAM *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instrb_4
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        mi_integer occurrence, MI_FPARAM *Gen_fparam);

mi_integer SqlLibC_instr
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        mi_integer occurrence, MI_FPARAM *Gen_fparam);

mi_integer SqlLibC_instr_back
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
         mi_integer occurrence, MI_FPARAM *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instr_2
        (mi_lvarchar *search_str, mi_lvarchar *search_val, 
        MI_FPARAM *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instr_3
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        MI_FPARAM *Gen_fparam);

UDREXPORT mi_integer SqlLibC_instr_4
        (mi_lvarchar *search_str, mi_lvarchar *search_val, mi_integer start,
        mi_integer occurrence, MI_FPARAM *Gen_fparam);

void qa_instr_args (mi_string *func, mi_integer search_len, mi_integer start,
        mi_integer occurrence, MI_FPARAM *Gen_fparam);

void raise_gl_error (mi_string *func, mi_string *gl_call, mi_integer gl_errno,
        MI_FPARAM *Gen_fparam);

void raise_builtin_err (mi_integer error_num);

UDREXPORT mi_integer SqlLibC_to_integer(mi_lvarchar *input_str, MI_FPARAM *fp);
UDREXPORT mi_integer SqlLibC_to_integer_fmt(mi_lvarchar *input_str, 
                   mi_lvarchar *gls_fmt, MI_FPARAM *fp);
UDREXPORT dec_t * SqlLibC_to_decimal (mi_lvarchar *input_str, MI_FPARAM *fp);
UDREXPORT dec_t * SqlLibC_to_decimal_fmt (mi_lvarchar *input_str,
                  mi_lvarchar *gls_fmt, MI_FPARAM *fp);
UDREXPORT mi_double_precision * SqlLibC_to_float (mi_lvarchar *input_str,
                  MI_FPARAM *fp);
UDREXPORT mi_double_precision * SqlLibC_to_float_fmt (mi_lvarchar *input_str,
                  mi_lvarchar  *gls_fmt, MI_FPARAM *fp);
MI_DATUM  SqlLibC_cast_char (mi_string *func, mi_lvarchar *source_data, 
                  mi_string *target_t);

#endif

