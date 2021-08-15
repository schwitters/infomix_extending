/*
** This file, trace.h, is not part of the University of Toronto regexp 
** source code.  sqlfuncs.h contains definitions and function prototypes
** for sqlfuncs.c, which implements user-defined SQL routines for the 
** Informix Dynamic Server. 
*/

#ifndef HDR_trace_H
#define HDR_trace_H  

#define TRACE_CLASS     "regexp"
#define TRACE_MEDIUM    20                     

#define FUNC_TRACE_ENTER(r) GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM, \
        ("regexp_FUNC_ENTER", "FUNCNAME%s", r, MI_LIST_END));

#define FUNC_TRACE_EXIT(r) GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM, \
        ("regexp_FUNC_EXIT", "FUNCNAME%s", r, MI_LIST_END));  

#endif
