/*
** This file, sqlfuncs.c, is not part of the University of Toronto regexp 
** source code.  sqlfuncs.c contains source code that Chris Bosch and Jean
** Anderson wrote to implement user-defined SQL routines for the Informix 
** Dynamic Server. 
*/

/*
** Title:          sqlfuncs.c
** Description:    C entrypoints for regexp SQL routines.
**
** SQL Entrypoint Functions:
**
**    regexp_match_char     Calls re_match for char/varchar/lvarchar.
**    regexp_match_clob     Calls re_match for clob.
**
**    regexp_replace_char   Matches pattern, replaces it with something else.
**    regexp_replace_char_n Matches/replaces nth occurrence.
**    regexp_replace_clob   Matches pattern, replaces it with something else.
**    regexp_replace_clob_n Matches/replaces nth occurrence.
**
**    regexp_extract        Returns substrings that match pattern.
**    regexp_extract_n      Returns up to n substrings that match pattern.
**
**    regexp_split          Splits string using pattern as delimiter.
**    regexp_split_n        Splits string into n sub-strings.
**
**    TraceSet_regexp       Turns on tracing.  
**
** Utility Functions (called by entrypoint functions):
**
**    re_match            Workhorse that tests if a string matches pattern.
**    re_replace_n        Workhorse for search and replace.
**    re_lodata_to_str    Converts CLOB contents to mi_lvarchar.
**    re_lodata_from_str  Converts mi_lvarchar to CLOB.
**    re_sblob_size       Determines length of CLOB contents.
*/

#include "sqlfuncs.h"
#include "trace.h"

/***************** Local function prototypes ***************/
static mi_boolean     re_match (char *str, char *exp);
static mi_lvarchar  * re_lodata_to_str (MI_LO_HANDLE  *sblob_src);
static MI_LO_HANDLE * re_lodata_from_str (mi_lvarchar  *text_data);
static mi_integer     re_sblob_size (MI_CONNECTION *conn, 
                      MI_LO_HANDLE *sblob_src, long *sblob_size);
static mi_lvarchar  * re_replace_n (mi_lvarchar *str_txt, 
                      mi_lvarchar *exp_txt, mi_lvarchar *rep_txt,
                      mi_integer limit);

extern regexp *regcomp(char *exp);

/* NAME
**    regexp_match_char - Calls re_match for char, varchar, lvarchar.
**    regexp_match_clob - Calls re_match for clob.
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
regexp_match_char
(
mi_lvarchar *str_txt, 
mi_lvarchar *exp_txt
)
{
    char         *str=NULL, 
                 *exp=NULL;
    mi_boolean   result;
    mi_string    *r="regexp_match_char";

    FUNC_TRACE_ENTER(r);

    /* Convert input text structures to strings */
    ALLOC_CHECK( str = mi_lvarchar_to_string(str_txt) );
    ALLOC_CHECK( exp = mi_lvarchar_to_string(exp_txt) );

    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
        ("regexp_ANY_TEXT",
         "FUNCNAME%s",       r,
         "MESSAGE%s",        str,
         MI_LIST_END
        )
    );

    result=re_match(str, exp);

    /* Free up strings converted from input text structures */
    mi_free((char *)str);
    mi_free((char *)exp);

    FUNC_TRACE_EXIT(r);

    return result;
}


mi_boolean 
regexp_match_clob
(
MI_LO_HANDLE *src, 
mi_lvarchar  *re
)
{
    mi_boolean    result;
    mi_lvarchar   *tmp_str=NULL;
    char          *str=NULL, 
                  *exp=NULL;
    mi_string     *r="regexp_match_clob";

    FUNC_TRACE_ENTER(r);

    /* Copy the source string stored in the CLOB to a dynamically
    ** allocated mi_lvarchar.
    */
    tmp_str = re_lodata_to_str(src);

    /* Convert args expression to NULL-terminated strings. */
    ALLOC_CHECK( str = mi_lvarchar_to_string(tmp_str) );
    ALLOC_CHECK( exp = mi_lvarchar_to_string(re) );


    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
        ("regexp_ANY_TEXT",
         "FUNCNAME%s",       r,
         "MESSAGE%s",        str,
         MI_LIST_END
        )
    );

    result=re_match(str, exp);

    /* Cleanup */
    mi_var_free(tmp_str);
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
**    regexp_replace_char     char types: search/replace for all occurrences.
**    regexp_replace_char_n   char : search/replace for up to n occurrences.
**    regexp_replace_clob     clob: search/replace for all occurrences.
**    regexp_replace_clob_n   clob: search/replace for up to n occurrences.
**    re_replace_n            Workhorse that does the search/replace.
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
regexp_replace_char_n
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
mi_lvarchar *rep_txt,
mi_integer  limit
)
{
    mi_lvarchar *retval=NULL;
    mi_string   *r="regexp_replace_char_n";

    FUNC_TRACE_ENTER(r);

    retval = re_replace_n (str_txt, exp_txt, rep_txt, limit);

    FUNC_TRACE_EXIT(r);

    return retval;
}

mi_lvarchar *
regexp_replace_char
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
mi_lvarchar *rep_txt
)
{
    mi_lvarchar *result;
    result = regexp_replace_char_n(str_txt, exp_txt, rep_txt, -1);
    return(result);
}


MI_LO_HANDLE *
regexp_replace_clob_n
(
MI_LO_HANDLE *str_txt,
mi_lvarchar  *exp_txt,
mi_lvarchar  *rep_txt,
mi_integer    limit
)
{
    mi_lvarchar   *src_str=NULL,
                  *new=NULL;
    MI_LO_HANDLE  *retval=NULL;
    mi_string     *r="regexp_replace_clob_n";

    FUNC_TRACE_ENTER(r);

    /* Convert input CLOB to mi_lvarchar */
    src_str = re_lodata_to_str(str_txt);

    new = re_replace_n (src_str, exp_txt, rep_txt, limit);

    retval = re_lodata_from_str(new);

    mi_var_free(src_str);
    mi_var_free(new);

    FUNC_TRACE_EXIT(r);

    return retval;
}

MI_LO_HANDLE *
regexp_replace_clob
(
MI_LO_HANDLE *str_txt,
mi_lvarchar *exp_txt,
mi_lvarchar *rep_txt
)
{
    MI_LO_HANDLE *result=NULL;
    result = regexp_replace_clob_n(str_txt, exp_txt, rep_txt, -1);
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


/* NAME
**    regexp_extract   - return a set of all matching text values
**    regexp_extract_n - return a set of up to n matching text values
**
** ARGUMENTS
**    Two text values, the first a text string to match,
**    the second a regular expression pattern
**
** DESCRIPTION
**    Returns a set of text values that match the
**    input regular expression pattern
**
*/

typedef struct regexp_extract_state
{
    char *str;
    char *exp;
    char *ptr;
    regexp *prog;
    mi_integer num;
} regexp_extract_state;


mi_lvarchar *regexp_extract_n ( mi_lvarchar *str_txt, mi_lvarchar *exp_txt, mi_integer  limit, MI_FPARAM   *fParam)
{
    regexp_extract_state *state;
    char                 *str;
    char                 *exp;
    char                 *ptr;
    mi_integer            len;
    regexp               *prog;
    mi_lvarchar          *result;
    MI_MEMORY_DURATION    md;
    mi_string            *r="regexp_extract_n";

    FUNC_TRACE_ENTER(r);

    switch(mi_fp_request(fParam))
    {
        /*
        ** Allocate and initialize the structure used
        ** to maintain state for this function.
        */
        case SET_INIT:
            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM, ("regexp_ANY_TEXT", "FUNCNAME%s",       r, "MESSAGE%s",        "regexp_extract_n: SET_INIT", MI_LIST_END));

            /* Switch memory duration to PER_COMMAND */
            if ((md = mi_switch_mem_duration(PER_COMMAND)) == MI_ERROR)
				mi_db_error_raise((MI_CONNECTION *)NULL, MI_EXCEPTION, "Failed to switch memory duration" );


            /* Convert input text structures to strings */
            ALLOC_CHECK( str = mi_lvarchar_to_string(str_txt) );
            ALLOC_CHECK( exp = mi_lvarchar_to_string(exp_txt) );

            /* Compile the regular expression */
            prog = regcomp(exp);

            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM, ("regexp_ANY_TEXT", "FUNCNAME%s",       r, "MESSAGE%s",        "regexp_extract_n: Here 3", MI_LIST_END));
            /* Allocate the state structure */
            ALLOC_CHECK( state = (regexp_extract_state *) mi_alloc(sizeof(regexp_extract_state)) );
            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM, ("regexp_ANY_TEXT", "FUNCNAME%s",       r, "MESSAGE%s",        "regexp_extract_n: Here 4", MI_LIST_END));

            /* Switch memory duration back to old value */
            mi_switch_mem_duration(md);

            /* Initialize the state structure */
            state->str = str;
            state->exp = exp;
            state->ptr = str;
            state->prog = prog;
            state->num = 1;

            mi_fp_setfuncstate( fParam, (void *) state );

            break;
        /*
        ** Return the next member of the set.
        */
        case SET_RETONE:

            state = mi_fp_funcstate(fParam);
            ptr = state->ptr;
            prog = state->prog;

            if (regexec(prog, ptr) && (limit == -1 || limit >= state->num))
            {
                ptr = prog->startp[0];
                len = prog->endp[0] - prog->startp[0];

                ALLOC_CHECK( result=(mi_lvarchar *)mi_new_var(len) );
                mi_set_vardata(result, ptr);

                state->ptr = prog->endp[0];
                state->num++;

                /* We matched a 0-length something, such as
                ** begin- or end-line.
                */
                if(len == 0)
                    mi_fp_setisdone(fParam, MI_TRUE);

                FUNC_TRACE_EXIT(r);

                return(result);
            }
            else
            {
                mi_fp_setisdone(fParam, MI_TRUE);
                break;
            }
        /*
        ** Free up the memory allocated to maintain state.
        */
        case SET_END:

            state = mi_fp_funcstate(fParam);
            mi_free((char *)state->str);
            mi_free((char *)state->exp);
            mi_free((char *)state->prog);
            mi_free((void *)state);
            break;

        default:
            break;
    }

    FUNC_TRACE_EXIT(r);

    return(mi_string_to_lvarchar("IGNORED"));
}


mi_lvarchar * 
regexp_extract
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
MI_FPARAM   *fParam
)
{
    mi_lvarchar *result;
    result = regexp_extract_n(str_txt, exp_txt, -1, fParam);
    return(result);
}

/* NAME
**    regexp_split   - return a setof of all text values
**    regexp_split_n - return a setof up to n text values
**
** ARGUMENTS
**    Two text values, the first a text string to match,
**    the second a regular expression pattern
**
** DESCRIPTION
**    Returns a set of text values produced by splitting
**    the input text string at substrings that match the
**    input regular expression
*/

typedef struct regexp_split_state
{
    char *str;
    char *exp;
    char *ptr;
    regexp *prog;
    mi_integer num;
    mi_boolean done;
} regexp_split_state;

mi_lvarchar * 
regexp_split_n
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
mi_integer  limit,
MI_FPARAM   *fParam
)
{
    regexp_split_state *state;
    char               *str, *exp, *ptr, *start, *end;
    regexp             *prog;
    mi_integer         len,
                       ptr_len;
    mi_lvarchar        *result;
    MI_MEMORY_DURATION md;
    mi_string          buffer[100],
                       *r="regexp_split_n";

    FUNC_TRACE_ENTER(r);

    switch(MI_FP_REQUEST(fParam))
    {
        /*
        ** Allocate and initialize the structure used
        ** to maintain state for this function.
        */
        case SET_INIT:

            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                ("regexp_ANY_TEXT",
                 "FUNCNAME%s",       r,
                 "MESSAGE%s",        "regexp_split: SET_INIT",
                 MI_LIST_END
                )
            );

            /* Switch memory duration to PER_COMMAND */
            md = mi_switch_mem_duration(PER_COMMAND);

            /* Convert input text structures to strings */
            ALLOC_CHECK( str = mi_lvarchar_to_string(str_txt) );
            ALLOC_CHECK( exp = mi_lvarchar_to_string(exp_txt) );

            /* Compile the regular expression */
            prog = regcomp(exp);

            /* Allocate the state structure */
            ALLOC_CHECK( state = (regexp_split_state *) mi_alloc(sizeof(regexp_split_state)) );

            /* Switch memory duration back to old value */
            mi_switch_mem_duration(md);

            /* Initialize the state structure */
            state->str = str;
            state->exp = exp;
            state->ptr = str;
            state->prog = prog;
            state->num = 1;
            state->done = MI_FALSE;

            MI_FP_SETFUNCSTATE( fParam, (void *) state );

            break;
        /*
        ** Return the next member of the set.
        */
        case SET_RETONE:
            state = MI_FP_FUNCSTATE(fParam);
            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                ("regexp_ANY_TEXT",
                 "FUNCNAME%s",       r,
                 "MESSAGE%s",        "regexp_split_n: SET_RETONE",
                 MI_LIST_END
                ));
            if (state->done == MI_TRUE)
            {
                MI_FP_SETISDONE(fParam, MI_TRUE);
                break;
            }


non_empty_val: /* we want to return just non-empty values */

            ptr = state->ptr;
            prog = state->prog;

            if( regexec(prog,ptr)  && (limit == -1 || limit > state->num) )
            {
                len = prog->startp[0] - ptr;
                if(ptr)
                    ptr_len=strlen(ptr);
                else
                    ptr_len=0;


                if ( tf(TRACE_CLASS, TRACE_MEDIUM) ) /* expensive debug block */
                {
                    mi_string  *msgbuf=NULL;

                    ALLOC_CHECK( msgbuf = (mi_string *)mi_zalloc(len + 120) );

                    sprintf(msgbuf, "SET_RETONE: len=%d, ptr_len=%d, value='", (int)len, (int)ptr_len);
                    strncat(msgbuf, ptr, len);
                    strncat(msgbuf, "'", 1);

                    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                        ("regexp_ANY_TEXT",
                         "FUNCNAME%s",       r,
                         "MESSAGE%s",        msgbuf,
                         MI_LIST_END
                        )
                    );

                    mi_free(msgbuf);
                }

                /* Boundary Check */
                start = prog->endp[0];
                end = prog->startp[0];

                /* The regular expression resulted in a 0 length match.
                ** Examples of queries that cause this block to execute:
                **
                **    execute function regexp_split ('abcdefgabcdefg', '^');
                **         (len=0, ptr_len=14)
                **
                **    execute function regexp_split ('abcdefgabcdefg', '$');
                **         (len=14, ptr_len=14)
                */
                if(start == end)
                {
                    sprintf(buffer, "Boundary check: start == end (len=%d, ptr_len=%d)", (int)len, (int)ptr_len);
                    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                        ("regexp_ANY_TEXT",
                         "FUNCNAME%s",       r,
                         "MESSAGE%s",        buffer,
                         MI_LIST_END
                        )
                    );

                    state->done = MI_TRUE;

                    if(ptr_len > 0)
                    {
                        ALLOC_CHECK( result 
                            = (mi_lvarchar *)mi_new_var(ptr_len) );
                        mi_set_vardata(result, ptr);
                        return result;
                    }
                    else
                    {
                        MI_FP_SETISDONE(fParam, MI_TRUE);
                        break;
                    }
                } /* END: if(start == end) */

                ALLOC_CHECK( result = (mi_lvarchar *)mi_new_var(len) );
                mi_set_vardata(result, ptr);

                state->ptr = prog->endp[0];
                state->num++;

                if(len == 0) /* skip empty values */
                {
                    GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                        ("regexp_ANY_TEXT",
                         "FUNCNAME%s",       r,
                         "MESSAGE%s",        "skipping 0-length value",
                         MI_LIST_END
                        )
                    );

                   goto non_empty_val;
                }

            } /* END regexec() */
            else
            {
                if(strlen(ptr) == 0)
                {
                    state->done = MI_TRUE;
                    MI_FP_SETISDONE(fParam, MI_TRUE);
                    break;
                }
                else
                {
                    if ( tf(TRACE_CLASS, TRACE_MEDIUM) ) /* expensive */
                    {
                        mi_string  *msgbuf=NULL;

                        ALLOC_CHECK( msgbuf = 
                            (mi_string *)mi_zalloc(strlen(ptr) + 100) );

                        sprintf(msgbuf, 
                           "SET_RETONE: ptr_len=%d, value='", 
                            strlen(ptr));
                        strcat(msgbuf, ptr);
                        strncat(msgbuf, "'", 1);

                        GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                            ("regexp_ANY_TEXT",
                             "FUNCNAME%s",       r,
                             "MESSAGE%s",        msgbuf,
                             MI_LIST_END
                            )
                        );

                        mi_free(msgbuf);
                    }

                    ALLOC_CHECK( result = mi_string_to_lvarchar(ptr) );
                    state->done = MI_TRUE;
                }
            }


            FUNC_TRACE_EXIT(r);
            return(result);
        /*
        ** Free up the memory allocated to maintain state.
        */
        case SET_END:

            GL_DPRINTF(TRACE_CLASS, TRACE_MEDIUM,
                ("regexp_ANY_TEXT",
                 "FUNCNAME%s",       r,
                 "MESSAGE%s",        "regexp_split_n: SET_END",
                 MI_LIST_END
                )
            );

            state = MI_FP_FUNCSTATE(fParam);
            mi_free((char *)state->str);
            mi_free((char *)state->exp);
            mi_free((char *)state->prog);
            mi_free((void *)state);
            break;

        default:
            break;
    }

    FUNC_TRACE_EXIT(r);
    return(mi_string_to_lvarchar("IGNORED"));
}


mi_lvarchar * 
regexp_split
(
mi_lvarchar *str_txt,
mi_lvarchar *exp_txt,
MI_FPARAM *fParam
)
{
    mi_lvarchar *result;
    result = regexp_split_n(str_txt, exp_txt, -1, fParam);
    return(result);
}

/* NAME
**    TraceSet_regexp
**
** ARGUMENTS
**    The first argument specifies the trace filename. 
**    The second argument specifies the trace level.
**
** DESCRIPTION
**    Entrypoint for the TraceSet_mrLvarchar(lvarchar,int) UDR.
**
**    Sets the tracefile name and sets the trace level for the
**    regexp trace class.
*/

void TraceSet_regexp
(
mi_lvarchar  *Gen_param,     /* Trace filename */
mi_integer   Gen_TraceLevel, /* Trace level */
MI_FPARAM    *Gen_fparam     /* UDR function parameter info */
)
{
    gl_mchar_t  *Gen_TraceFile;     /* Trace file name.    */
    char        Gen_TraceStr[50];   /* Tracing level.    */
    mi_string   *r="TraceSet_regexp";

    FUNC_TRACE_ENTER(r);

    /*
    ** Set the name of the  output trace file. If not set, 
    ** the name defaults to <session id>.trc in the /tmp directory.
    */
    if(mi_get_varlen(Gen_param) != 0 )
    {
        /* Convert input arg to a NULL-terminated string. */
        Gen_TraceFile = (gl_mchar_t *)mi_lvarchar_to_string(Gen_param);

        /* Set the name of the trace file. */
        if(mi_tracefile_set((char *)Gen_TraceFile ) == -1 )
        {
            /* Raise the dbdk error in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENI", "FUNCTION%s",
                r, (mi_integer) NULL);
            return; /* NOT REACHED */
        }

        /* Free the allocated memory. */
        (void) mi_free(Gen_TraceFile);
    }

    /*               
    ** Set the tracing level. The default tracing level is 0.
    */
    if( Gen_TraceLevel >= 0 )
    {
        sprintf( Gen_TraceStr, "%s %d", TRACE_CLASS, (int)Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            /* Raise the error that dbdk stores in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENJ", "FUNCTION%s",
                r, (mi_integer) NULL);

            return; /* not reached */
        }

        /* Also turn on the __myErrors__ trace class */
        sprintf( Gen_TraceStr, "__myErrors__ %d", (int)Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            /* Raise the error that dbdk stores in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENJ", "FUNCTION%s",
                r, (mi_integer) NULL);

            return; /* not reached */          
        }
    }

    /* The first call initializes GLS messages */
    FUNC_TRACE_EXIT(r);

    return;
}



/****************** Smart blob utility functions ********************
**
** re_lodata_to_str      Allocates a string, copies the contents of the
**                       CLOB to that string.
** re_lodata_from_str    Allocates a CLOB, copies the contents of the
**                       string to that CLOB.
** re_sblob_size         Returns the size of the clob.
*/

static
mi_lvarchar *
re_lodata_to_str
(
MI_LO_HANDLE  *sblob_src
)
{
    mi_lvarchar  *retval=NULL;
    mi_char      *p;
    long          sbsize;
    MI_CONNECTION *conn=NULL;
    mi_integer    status;
    mi_string     errorbuf[100],
                  *r="re_lodata_to_str";

    FUNC_TRACE_ENTER(r);

    /****************************************************************
    ** Copy the smart blob data into an allocated buffer. 
    ** Depending on the size of the data, this could be expensive.
    *****************************************************************
    */

    if ((conn = mi_open(NULL,NULL,NULL)) == (MI_CONNECTION *)NULL)
    {
        /* Raise the DBDK error */
        mi_db_error_raise(NULL, MI_SQL, "UGEN1", "FUNCTION%s",
            r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    if ((re_sblob_size(conn, sblob_src, &sbsize) ) != MI_OK)
    {
        mi_close(conn);
        sprintf(errorbuf, "%s: re_sblob_size failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    ALLOC_CHECK(retval = mi_new_var(sbsize));

    p=mi_get_vardata(retval);

    status = mi_lo_to_buffer(conn, sblob_src, sbsize, &p);
    if (status == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_to_buffer failed (%d)!", r, (int)status);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Make sure that we copied the number of bytes that we expected. */
    if(status != sbsize)
    {
        sprintf(errorbuf, "%s: mi_lo_to_buffer returned %d, but the CLOB had %ld bytes", r, (int)status, sbsize);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    (void)mi_close(conn); /* We're done with the connection handle */

    FUNC_TRACE_EXIT(r);

    return retval;
}

static
MI_LO_HANDLE *
re_lodata_from_str
(
mi_lvarchar  *text_data
)
{
    MI_CONNECTION  *conn=NULL;
    MI_LO_HANDLE   *retval=NULL;
    MI_LO_SPEC     *lo_spec_p = NULL; /* must be NULL */
    MI_LO_FD       slob_fd;
    mi_int8        int8_losize;
    mi_integer     status,
                   data_len;
    mi_string      errorbuf[80],
                   *r="re_lodata_to_str";

    FUNC_TRACE_ENTER(r);

    /*  This function does the following:
    **
    **  1. mi_open():                get a connection handle
    **  2. mi_lo_spec_init():        allocate MI_LO_SPEC structure
    **  3. mi_lo_specset_estbytes(): set the size
    **  4. mi_lo_create():           create a smart blob
    **  5. mi_lo_write():            write the data to the smart blob
    **  6. Cleanup
    */

    /* ==== 1. mi_open(): get a connection handle ===== */
    if ((conn = mi_open(NULL,NULL,NULL)) == (MI_CONNECTION *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN1", "FUNCTION%s",
            r, (mi_integer) NULL);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 2. mi_lo_spec_init(): allocate MI_LO_SPEC structure =====
    **
    ** The pointer passed to mi_lo_spec_init must be NULL for it
    ** to allocate storage for the MI_LO_SPEC structure.
    */
    if ((mi_lo_spec_init(conn, &lo_spec_p)) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_spec_init failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 3. mi_lo_specset_estbytes(): set the size ================ */
    data_len = mi_get_varlen(text_data);

    if ((ifx_int8cvlong(data_len, &int8_losize)) < 0)
    {
        sprintf(errorbuf, "%s: ifx_int8cvlong() failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    if((mi_lo_specset_estbytes(lo_spec_p, &int8_losize)) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_specset_estbytes() failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 4. mi_lo_create(): create the smart blob ================= */
    slob_fd = mi_lo_create (conn, lo_spec_p, MI_LO_WRONLY, &retval);
    if(slob_fd == MI_ERROR)
    {
        (void) mi_lo_spec_free(conn, lo_spec_p);
        sprintf(errorbuf,
            "%s: mi_lo_create failed! Are you out of smart blob space?",
            r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 5. mi_lo_write(): write the data to the smart blob ======= */
    status = mi_lo_write(conn, slob_fd, mi_get_vardata(text_data), data_len);
    if (status == MI_ERROR)
    {
        (void) mi_lo_spec_free(conn, lo_spec_p);
        (void) mi_lo_close(conn, slob_fd);
        sprintf(errorbuf,
            "%s: mi_lo_write failed! Are you out of smart blob space?",
            r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }
    if(status != data_len)
    {
        (void) mi_lo_spec_free(conn, lo_spec_p);
        (void) mi_lo_close(conn, slob_fd);
        sprintf(errorbuf, "%s: mi_lo_write wrote %d bytes, but we told it %d", r, (int)status, (int)data_len);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 6. Cleanup =============================================== */
    (void) mi_lo_spec_free (conn, lo_spec_p );
    (void) mi_lo_close(conn, slob_fd); 
    (void) mi_close(conn);

    FUNC_TRACE_EXIT(r);

    return retval;
}

/*
** Gets the size of a smart blob from the smart blob.
*/
static
mi_integer
re_sblob_size
(
MI_CONNECTION *conn,
MI_LO_HANDLE  *sblob_src, 
long          *sblob_size  /* This function fills in size */
)
{
    MI_LO_FD   sblob_fd;            /* Descriptor for accesssing sblob */
    MI_LO_STAT *lostat=NULL;        /* Lets us get size */
    mi_int8    size;  
    long       total;
    mi_integer status;
    mi_string  *r="re_sblob_size",
               errorbuf[80];

    FUNC_TRACE_ENTER(r);

    if ((sblob_fd = mi_lo_open(conn, sblob_src, MI_LO_RDONLY)) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_open failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    if ((mi_lo_stat(conn, sblob_fd, &lostat)) != MI_OK)
    {
        sprintf(errorbuf, "%s: mi_lo_stat failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    if (mi_lo_stat_size(lostat, &size) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_stat_size failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    status = (ifx_int8tolong(&size, &total));
    if (status == -1200)
    {
        sprintf(errorbuf, "%s: int8 value is too large to convert!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }
    else if (status != 0)
    {
        sprintf(errorbuf, "%s: ifx_int8tolong failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    *sblob_size = total;

    /* Cleanup */
    (void) mi_lo_stat_free(conn, lostat);
    (void) mi_lo_close(conn, sblob_fd);

    FUNC_TRACE_EXIT(r);

    return MI_OK;
}
