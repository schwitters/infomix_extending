/********************************************************
**  Copyright (c) 2000-2001 Informix Software Inc.
**
**  Permission is hereby granted, free of charge, 
**  to any person obtaining a copy of this software 
**  and associated documentation, to use, copy, 
**  modify, and distribute, subject to the following 
**  disclaimer:
**  
**  THIS SOFTWARE IS PROVIDED FREE, "AS IS", 
**  AND INFORMIX HEREBY DISCLAIMS ANY WARRANTY
**  OR CONDITION OF ANY KIND, EXPRESS OR IMPLIED, 
**  INCLUDING WARRANTIES OF MERCHANTABILITY, OR 
**  FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
**  SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
**  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
**  LIABILITY ARISING IN CONNECTION WITH THE 
**  SOFTWARE.
*********************************************************/

/*
** This file has been incorporated from the regexp.1.0 project that creates
** UDRs from the University of Toronto regexp library.  
**
** This file is not part of the U of Toronto distribution. It contains source 
** code that Chris Bosch and Jean Anderson wrote to implement user-defined SQL 
** routines for the Informix Dynamic Server. 
*/

/*
** Title:          re_funcs.c
** Description:    C entrypoints for regexp SQL routines.
**
** SQL Entrypoint Functions:
**
**    Regexp_Match        Calls re_match for idn_mrLvarchar
**
**    Regexp_Replace      Matches pattern, replaces it with something else.
**    Regexp_Replace      Matches/replaces nth occurrence.
**
** Utility Functions (called by entrypoint functions):
**
**    re_match            Workhorse that tests if a string matches pattern.
**    re_replace_n        Workhorse for search and replace.
*/

#include <string.h>
#include <mi.h>
#include <ifxgls.h>
#include "regexp.h"
#include "mrLvarchar.h"

/***************** Public SQL entrypoints *******************/
mi_boolean    Regexp_Match (mi_lvarchar *src, mi_lvarchar *re);
mi_lvarchar * Regexp_Replace_n (mi_lvarchar  *str_txt, mi_lvarchar *exp_txt,
              mi_lvarchar *rep_txt, mi_integer limit, MI_FPARAM *fp);
mi_lvarchar * Regexp_Replace (mi_lvarchar *str_txt, mi_lvarchar *exp_txt,
              mi_lvarchar *rep_txt, MI_FPARAM *fp);

/***************** Local function prototypes ***************/
static 
mi_boolean    re_match (char *str, char *exp);

static 
mi_lvarchar * re_replace_n (mi_lvarchar *str_txt, mi_lvarchar *exp_txt, 
              mi_lvarchar *rep_txt, mi_integer limit);


/* NAME
**    regexp_match      - Calls re_match for idn_mrLvarchar.
**    re_match          - Workhorse function that performs the match.
**
** ARGUMENTS
**    Two text values, the first a text string to match, the second a
**    regular expression pattern
**
** DESCRIPTION
**    Returns a single boolean value that is true if part of the input
**    text string matches the input regular expression pattern
*/

mi_boolean 
Regexp_Match
(
mi_lvarchar *src,  /* Contains an idn_mrLvarchar UDT */
mi_lvarchar *re    /* Text string contains regular expression */
)
{
    mi_boolean    result;
    char          *str=NULL, 
                  *exp=NULL;
    mi_integer    size;
    mi_string     msgbuf[80],
                  *r="Regexp_Match";

    FUNC_TRACE_ENTER(r);

    /* Convert args expression to NULL-terminated strings. */
    ALLOC_CHECK( exp = mi_lvarchar_to_string(re) );

    /* Copy the source string stored in the idn_mrLvarchar to a 
    ** dynamically allocated, NULL-terminated string.
    */
    str=mrLvarchar_to_char(src, &size);
    if(str == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return MI_ERROR; /* not reached */
    }

    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
        ("idn_mrLvarchar_ANY_TEXT",
         "FUNCNAME%s",       r,
         "MESSAGE%s",        str,
         MI_LIST_END
        )
    );

    result=re_match(str, exp);

    /* Cleanup */
    mi_free((char *)str);
    mi_free((char *)exp);

    FUNC_TRACE_EXIT(r);

    return result;
}

static
mi_boolean 
re_match
(
char *str, 
char *exp
)
{
    regexp       *prog=NULL;
    mi_boolean   result;
    mi_string    *r="re_match";

    FUNC_TRACE_ENTER(r);

    /* SQL implementation note (Jean Anderson): Henry Spencer says that 
    ** compiling the expression is not expensive, and tests support that.  
    ** When the compiled expression was cached in the MI_FPARAM, it saved 
    ** only 2-3 seconds on a 100,000 record operation. Since we aren't 
    ** guaranteed that the second argument will be a constant, it's 
    ** better to not cache it in the MI_FPARAM. Otherwise, we would 
    ** need to check it between invocations to see if it changed, 
    ** then recompile it. 
    */
    prog = regcomp(exp);

    /* Set result according to whether there is a regexp match */
    regexec(prog, str) ? (result=MI_TRUE) : (result=MI_FALSE);

    /* Free up strings converted from input text structures */
    mi_free((char *)prog);

    FUNC_TRACE_EXIT(r);

    return result;
}

/* NAME
**    Regexp_Replace       search/replace for all occurrences.
**    Regexp_Replace_n     search/replace for up to n occurrences.
**    re_replace_n         Workhorse that does the search/replace.
**
** ARGUMENTS
**    Three text values, the first a text string to match, the second a regular
**    expression pattern, the third a replacement pattern to be applied to the
**    matched substrings
**
** DESCRIPTION
**    Returns a single text value that is the input string with all substrings
**    matching the input regular expression pattern replaced as specified by the
**    replacement pattern.
*/

mi_lvarchar *
Regexp_Replace_n
(
mi_lvarchar  *str_txt,   /* idn_mrLvarchar UDT value */
mi_lvarchar  *exp_txt,   /* regular expression */
mi_lvarchar  *rep_txt,   /* replacement text   */
mi_integer    limit,
MI_FPARAM    *fp
)
{
    mi_lvarchar   *src_str=NULL,
                  *new=NULL;
    mi_char       *data=NULL;
    mi_lvarchar   *retval=NULL;
    mi_integer    size;
    mi_string     msgbuf[80],
                  *r="Regexp_Replace_n";

    FUNC_TRACE_ENTER(r);

    /*
    ** The re_replace_n routine operates on a string stored in an 
    ** mi_lvarchar.  So convert from an idn_mrLvarchar UDT to a string 
    ** stored in a dynamically allocated mi_lvarchar.
    */
    data=mrLvarchar_to_char(str_txt, &size);
    if(data == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }
    src_str=mi_new_var(0);
    if(src_str == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mi_new_var failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }
    mi_set_varptr(src_str, data);
    mi_set_varlen(src_str, size);

    new = re_replace_n (src_str, exp_txt, rep_txt, limit);

    /* Convert back into an mrLvarchar UDT. */
    retval = mrLvarchar_from_char(mi_get_vardata(new), mi_get_varlen(new), fp);
    if(retval == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_from_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    mi_var_free(src_str);
    mi_var_free(new);

    FUNC_TRACE_EXIT(r);

    return retval;
}

mi_lvarchar *
Regexp_Replace
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
mi_lvarchar *rep_txt,
MI_FPARAM   *fp
)
{
    mi_lvarchar *result=NULL;
    result = Regexp_Replace_n(str_txt, exp_txt, rep_txt, -1, fp);
    return(result);
}


static
mi_lvarchar *
re_replace_n
(
mi_lvarchar *str_txt, 
mi_lvarchar *exp_txt,
mi_lvarchar *rep_txt,
mi_integer limit
)
{
    int          num = 0;
    int          old_len, unm_len, rep_len, new_len;
    char        *str, *exp, *rep, *ptr, *tmp;
    char        *old_dat, *new_dat;
    mi_lvarchar *old, *new;
    regexp      *prog;

    /* Convert input text structures to strings */
    ALLOC_CHECK( str = mi_lvarchar_to_string(str_txt) );
    ALLOC_CHECK( exp = mi_lvarchar_to_string(exp_txt) );
    ALLOC_CHECK( rep = mi_lvarchar_to_string(rep_txt) );

    /* Initialize the input string pointer */
    ptr = str;

    /* Initialize the result text structure */
    ALLOC_CHECK( new = mi_string_to_lvarchar("") );

    /* Compile the regular expression */
    prog = regcomp(exp);

    while (regexec(prog, ptr) && (limit == -1 || limit > num))
    {
        old = new;

        /* Get length of text data from old text structure */
        /* Get length of the unmatched substring */
        /* Get length of the replacement for matched substring */
        /* Calculate total length of the new text data */
        old_len = mi_get_varlen(old);
        unm_len = prog->startp[0] - ptr;
        rep_len = regsublen(prog, rep);
        new_len = old_len + unm_len + rep_len;

        /* Allocate new text structure */
        ALLOC_CHECK( new = (mi_lvarchar *)mi_new_var(new_len) );

        /* Get pointers to data in old and new text structures */
        old_dat = mi_get_vardata(old);
        new_dat = mi_get_vardata(new);

        /* Copy old text data into new text structure */
        /* Copy unmatched substring into new text structure */
        /* Copy replacement for matched substring into new text structure */
        memcpy(new_dat, old_dat, old_len);
        memcpy(&new_dat[old_len], ptr, unm_len);
        regsub(prog, rep, &new_dat[old_len + unm_len]);

        /* Free up the old text structure */
        mi_var_free((mi_lvarchar *)old);

        /* Move the input string pointer to the end of the matched substring 
         * Also, get the new start point for a sanity check.
         */
        ptr = prog->endp[0];
        tmp = prog->startp[0];

        /* A few cases result in startp and endp set to the same
        ** address, resulting in an infinite loop.
        */
        if( ptr == tmp )
            break;

        /* Increment the counter */
        num++;
    }

    old = new;

    /* Get length of text data from old text structure */
    /* Get length of the unmatched substring */
    /* Calculate total length of the new text data */
    old_len = mi_get_varlen(old);
    unm_len = strlen(ptr);
    new_len = old_len + unm_len;

    /* Allocate new text structure */
    ALLOC_CHECK( new = (mi_lvarchar *)mi_new_var(new_len) );

    /* Get pointers to data in old and new text structures */
    old_dat = mi_get_vardata(old);
    new_dat = mi_get_vardata(new);

    /* Copy old text data into new text structure */
    /* Copy unmatched substring into new text structure */
    memcpy(new_dat, old_dat, old_len);
    memcpy(&new_dat[old_len], ptr, unm_len);

    /* Free up the old text structure */
    /* Free up strings converted from input text structures */
    /* Free up the compiled regular expression */
    mi_var_free((mi_lvarchar *)old);
    mi_free((char *)str);
    mi_free((char *)exp);
    mi_free((char *)rep);
    mi_free((char *)prog);

    /* Return the new text structure */
    return(new);
}


