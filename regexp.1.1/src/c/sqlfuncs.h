/*
** This file, sqlfuncs.h, is not part of the University of Toronto regexp 
** source code.  sqlfuncs.h contains definitions and function prototypes
** for sqlfuncs.c, which implements user-defined SQL routines for the 
** Informix Dynamic Server. 
*/

#ifndef HDR_sqlfuncs_H
#define HDR_sqlfuncs_H  

#include <string.h>
#include <mi.h>
#include <ifxgls.h>
#include "regexp.h"

mi_boolean     regexp_match_char (mi_lvarchar *str_txt, mi_lvarchar *exp_txt);
mi_boolean     regexp_match_clob (MI_LO_HANDLE *src, mi_lvarchar  *re);
mi_lvarchar  * regexp_replace_char_n (mi_lvarchar *str_txt,
               mi_lvarchar *exp_txt, mi_lvarchar *rep_txt, mi_integer limit);
mi_lvarchar  * regexp_replace_char (mi_lvarchar *str_txt, mi_lvarchar *exp_txt,
               mi_lvarchar *rep_txt);
MI_LO_HANDLE * regexp_replace_clob_n (MI_LO_HANDLE *str_txt,
               mi_lvarchar *exp_txt, mi_lvarchar *rep_txt, mi_integer limit);
MI_LO_HANDLE * regexp_replace_clob (MI_LO_HANDLE *str_txt,
               mi_lvarchar *exp_txt, mi_lvarchar *rep_txt);
mi_lvarchar  * regexp_extract_n (mi_lvarchar *str_txt, mi_lvarchar *exp_txt, 
               mi_integer limit, MI_FPARAM *fParam);
mi_lvarchar  * regexp_extract (mi_lvarchar *str_txt, mi_lvarchar *exp_txt,
               MI_FPARAM *fParam); 
mi_lvarchar  * regexp_split_n (mi_lvarchar *str_txt, mi_lvarchar *exp_txt,
               mi_integer  limit, MI_FPARAM *fParam);
mi_lvarchar  * regexp_split (mi_lvarchar *str_txt, mi_lvarchar *exp_txt,
               MI_FPARAM *fParam);
void           TraceSet_regexp (mi_lvarchar *Gen_param,
               mi_integer Gen_TraceLevel, MI_FPARAM *Gen_fparam);

#endif
