/*
** Title:          SqlLibC.c
** Author:         jean.anderson@informix.com
** Description:    C code for SqlLibC 
**
** C functions that are entrypoints for SQL routines:
**
**    SqlLibC_ascii      ascii (char) returns integer
**    SqlLibC_ceil       ceil (double precision)
**    SqlLibC_chr        chr (integer) returns char
**    SqlLibC_sign       sign (double precision) returns integer
**    TraceSet_SqlLibC   SqlLibC_Trace (lvarchar, integer)
**
**    SqlLibC_instr_2    instr(lvarchar, lvarchar)
**    SqlLibC_instr_3    instr(lvarchar, lvarchar, integer)
**    SqlLibC_instr_4    instr(lvarchar, lvarchar, integer, integer)
**
**    SqlLibC_instrb_2   instrb(lvarchar, lvarchar)
**    SqlLibC_instrb_3   instrb(lvarchar, lvarchar, integer)
**    SqlLibC_instrb_4   instrb(lvarchar, lvarchar, integer, integer)
**
**    SqlLibC_to_decimal      to_decimal(lvarchar)
**    SqlLibC_to_decimal_fmt  to_decimal(str lvarchar, gls_fmt lvarchar)
**    SqlLibC_to_float        to_float(lvarchar)
**    SqlLibC_to_float_fmt    to_float(lvarchar, gls_fmt lvarchar)
**    SqlLibC_to_integer      to_integer(lvarchar)
**    SqlLibC_to_integer_fmt  to_integer(lvarchar, gls_fmt lvarchar)
**
** Internal C functions called by other functions:
**
**    SqlLibC_instr       Called by SqlLibC_instr_* functions:  forwards search
**    SqlLibC_instr_back  Called by SqlLibC_instr_* functions:  backwards search
**    SqlLibC_instrb      Called by SqlLibC_instrb_* functions: forwards search
**    SqlLibC_instrb_back Called by SqlLibC_instrb_* functions: backwards search
**    SqlLibC_cast_char   Called by SqlLibC_to_{decimal,float} functions
**    qa_instr_args       Called by SqlLibC_instr and SqlLibC_instrb
**    raise_gl_error      Called by SqlLibC_instr to raise GLS error
**    raise_builtin_err   Called by SqlLibC_ceil to raise built-in error
*/

#include "SqlLibC.h"

/************************ ascii **************************************************
** Function name:
**
**    SqlLibC_ascii
**
** Description:
**
**    Entrypoint for the SQL routine ascii (char) returns integer.
**
**    Returns the ASCII value, in decimal, that corresponds to the
**    input argument.
** 
** Parameters:
**
**    Input character value.
**
** Return value:
**
**    mi_integer
**
********************************************************************************
*/

UDREXPORT 
mi_integer SqlLibC_ascii
(
mi_lvarchar *char_in,    /* Input character */
MI_FPARAM   *Gen_fparam  /* UDR function parameter info */
)
{
    mi_integer  Gen_RetVal;
    mi_string   *p=NULL,
                *func="ascii";

    SQL_TRACE_ENTER(func);

    p = mi_get_vardata(char_in);

    Gen_RetVal = p[0];

    /* This UDR only handles values between 0 and 127, so
    ** raise an error if the value is outside that range.
    */
    if(Gen_RetVal < 0 || Gen_RetVal > 127)
    {
        /* Extra cautious: set return value to NULL */
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        /* Raise our user-defined error */
        mi_db_error_raise(NULL, 
            MI_SQL,       "USQ01",
            "UDRNAME%s",  func,
            "VALUE%s",    p[0],
            "RANGE%s",    "0-127",
            (mi_integer) NULL);

        return MI_ERROR;    /* NOT REACHED */
    }

    SQL_TRACE_EXIT(func);

    return Gen_RetVal;
}

/************************* ceil ************************************************
**
** Function name:
**
**    SqlLibC_ceil
**
** Description:
**
**    Entrypoint for the SQL routine ceil (double precision) returns integer.
**
**    Takes a single numeric value as input and returns the smallest integer 
**    that is either equal to or greater than the input value. Calls the 'ceil'
**    function in the math library.
**
** Parameters:
**
**    Numeric value
**
** Return value:
**
**    mi_integer
**
********************************************************************************
*/

UDREXPORT 
mi_integer SqlLibC_ceil
(
mi_double_precision  *input,      /* Input float */
MI_FPARAM            *Gen_fparam  /* UDR function parameter info */
)

{
    mi_integer  Gen_RetVal,      /* The return value. */
                exceeds_limit = -1215;
    mi_string   *func="ceil";

    SQL_TRACE_ENTER(func);

    /* This routine takes a double precision value but 
    ** returns an integer. So make sure the input value 
    ** is not larger than an integer (limits.h defines 
    ** INT_MIN and INT_MAX).
    **
    ** If the value is too big, raise built-in error -1215. 
    ** However, mi_db_error_raise doesn't allow raising a 
    ** built-in error, so invoke another routine that can.
    */
    if(*input < INT_MIN || *input > INT_MAX)
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
        raise_builtin_err(exceeds_limit);
        return MI_ERROR; /* NOT REACHED */
    }

    Gen_RetVal = (mi_integer) ceil(*input);

    SQL_TRACE_EXIT(func);

    return Gen_RetVal;
}

/*************************** chr ***********************************************
**
** Function name:
**
**    SqlLibC_chr
**
** Description:
**
**    Entrypoint for the SQL routine chr (integer) returns char.
**
**    Takes an integer value and returns the character that is represented 
**    by the ASCII value (in decimal) of that integer. 
**
** Parameters:
**
**    Ascii value in decimal.
**
** Return value:
**
**    mi_lvarchar
**
********************************************************************************
*/
UDREXPORT mi_lvarchar *SqlLibC_chr
(
mi_integer  ascii_input, /* Value to convert from ASCII to char */
MI_FPARAM  *Gen_fparam   /* UDR function parameter info */
)
{
    mi_lvarchar *Gen_RetVal=NULL;
    char        chr_result[2];
    mi_string   *func="chr";

    SQL_TRACE_ENTER(func);

    /* Raise an error if the value is outside the valid range. */
    if(ascii_input < 0 || ascii_input > 127)
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        mi_db_error_raise( NULL, 
            MI_SQL,       "USQ01",
            "UDRNAME%s",  func,
            "VALUE%d",    ascii_input,
            "RANGE%s",    "0-127",
            (mi_integer) NULL);

        return (mi_lvarchar *)NULL;    /* NOT REACHED */
    }

    chr_result[0] = (char) ascii_input;
    chr_result[1]='\0';

    /* Allocate the return result. */
    Gen_RetVal = mi_string_to_lvarchar(chr_result);
    if(Gen_RetVal == (mi_lvarchar *)NULL) /* memory allocation failed */
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        /* Raise the DBDK error */
        mi_db_error_raise(NULL, 
            MI_SQL,        "UGEN2",  /* Use the DBDK error message */
            "FUNCTION%s",  func, 
            (mi_integer) NULL);

        return (mi_lvarchar *)NULL; /* not reached */
    }

    SQL_TRACE_EXIT(func);

    return Gen_RetVal;
}

/************************** instrb *********************************************
**
** Function name:
**
**    SqlLibC_instrb   - workhorse routine called by SqlLibC_instrb_* functions
**    SqlLibC_instrb_2 - two arguments
**    SqlLibC_instrb_3 - three arguments
**    SqlLibC_instrb_4 - four arguments
**
** Description:
**
**    Searches for a value in a string, and returns the position where 
**    it was found.
**
**    Entrypoint for the SQL routine instrb(), which is overloaded for
**    three routine signatures.
**
**    This is identical to instr(), but assumes a single-byte character
**    set.
**
** Parameters:
**
**    search string    String we are searching through.
**    search value     Value we are searching for.
**    start position   Start position.
**    occurrence       Nth location to find.
**
** Return value:
**
**    mi_integer       Position search value was found at. 0 if not found. 
**
********************************************************************************
*/

mi_integer SqlLibC_instrb
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The Nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer position=0,  /* Position search_val was found at in search_str */
               count;       /* Loop counter for processing each occurrence */

    mi_string  *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL,
               *func="instrb";

    SQL_TRACE_ENTER(func);

    /* QA the input arguments. */
    qa_instr_args(func, mi_get_varlen(search_str), start, occurrence, 
         Gen_fparam);

    /* C string library functions operate on NULL-terminated strings,
    ** so copy each input mi_lvarchar to a NULL-terminated string.
    */
    p1=mi_lvarchar_to_string(search_str);
    p2=mi_lvarchar_to_string(search_val);

    /* Set the starting point.
    ** Most customers enter a 1-based start point, but C uses 0-based. 
    ** Some customers are C programmers, might enter 0 for position 1.
    ** Accommodate both.
    */
    if(start == 0) /* accommodate C programmers */
        p = p1;
    else
        p = p1 + (start-1);

    for (count=0, find_it=NULL; count < occurrence; count++)
    {
        /* Previous occurrence found it in position 1
        ** (C index 0), so step past.
        */
        if (find_it == p)             
            p++;

        find_it = strstr(p, p2);

        if(find_it == (mi_string *)NULL)    /* not found */
        {
            p=NULL;
            break;
        }

        else
            p = find_it; 
    } /* END: for each occurrence */

    if(p == NULL)
        position = 0;
    else
        position = (p - p1) + 1;    /* C is 0-based, customers are 1-based. */

    mi_free(p1);
    mi_free(p2);

    SQL_TRACE_EXIT(func);

    return position;
}


/* SqlLibC_instrb_back
**
** Implements backwards search for instrb. If the 'start' 
** is negative, logically this function counts and 
** searches backward from the end of the search string.
**
** This function does not use 'rindex' because rindex isn't 
** ANSI C standard and isn't implemented on all platforms.
**
** Instead, this function performs a forward search up to 
** the negative 'start' specified by the user, building an 
** array of the found positions. Then it counts backward 
** to the occurrence requested and delivers the position 
** at that occurrence.
**
** Makes perfect sense, right? whew!
**
** Here is the driving example from the Oracle8 SQL Reference:
**
**    Example 2  
**           SELECT INSTR('CORPORATE FLOOR','OR', -3, 2)
**           "Reversed Instring"
**                FROM DUAL;
**            
**           Reversed Instring
**           -----------------
**                           2
**
** Logically, the search starts on character 13 (the 
** third character from the end, and the first 'O' in
** 'FLOOR'). Moving backwards, the first occurrence of 
** 'OR' is found at character 5 and the second occurrence 
** is found at character 2.
**
** This implementation searches for all occurrences of 'OR' 
** from the start of the string, stopping the search after 
** the 'O' in 'FLOOR'. It builds a position array that records
** each position found, which looks like this for the query
** above  (remember C is 0-based):
**
**    position[1,4]
**
** Moving backwards through the array, the second occurrence
** is "1", which gets incremented to "2" for the 1-based
** customer.
*/

#define MAX_P 10

mi_integer SqlLibC_instrb_back
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The Nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer retval=0,
               end_point,
               temp_position,
               position[MAX_P],/* Positions found */
               count;       /* Total number of occurrences found */

    mi_string  *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL,
               *func="instrb";

    SQL_TRACE_ENTER(func);

    /* QA the input arguments. */
    qa_instr_args(func, mi_get_varlen(search_str), start, occurrence, 
         Gen_fparam);

    if(occurrence > MAX_P) /* duh */
    {
        mi_string error_buf[120];

        sprintf(error_buf, 
            "%s: oops! current implementation restricts occurrence to <= %d",
            func, MAX_P);

        mi_db_error_raise(NULL, MI_EXCEPTION, error_buf);

        /* NOT REACHED */
    }
    
    for(count=0; count<MAX_P; count++)
        position[count]=0;

    /* C string library functions operate on NULL-terminated strings,
    ** so copy each input mi_lvarchar to a NULL-terminated string.
    */
    p1=mi_lvarchar_to_string(search_str);
    p2=mi_lvarchar_to_string(search_val);

    /* Calculate the end point so we don't start a search
    ** past this point.
    */
    end_point = mi_get_varlen(search_str) + start + 1;

    for (p=p1, count=0, temp_position=0; temp_position < end_point; count++)
    {
        if (find_it == p) /* previous search found it in position 1 */
            p++;

        find_it = strstr(p, p2);

        if(find_it == (mi_string *)NULL)    /* Not found */
            break;
        else
        {         
            temp_position = find_it - p1;
            if (temp_position >= end_point) /* Went too far */
                break;
            else
            {            
                position[count] = temp_position;
                p = find_it;
            }
        }
    } /* END: for each occurrence up to the end_point */

    if(count == 0)                /* No occurrences at all were found */
        retval = 0;
    else if(count < occurrence) /* Not enough occurrences were found */
        retval = 0;
    else
    {
        /* Output the position of the requested occurrence, moving backwards. */
        retval = position[count-occurrence];
        retval++;
    }

    mi_free(p1);
    mi_free(p2);

    SQL_TRACE_EXIT(func);

    return retval;
}

UDREXPORT 
mi_integer SqlLibC_instrb_2
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
MI_FPARAM   *Gen_fparam 
)
{
    return SqlLibC_instrb(search_str, search_val, 1, 1, Gen_fparam);
}

UDREXPORT 
mi_integer SqlLibC_instrb_3
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
MI_FPARAM   *Gen_fparam
)
{
    mi_integer retval;

    if(start >= 0)
        retval=SqlLibC_instrb(search_str, search_val, start, 1, Gen_fparam);
    else
        retval=SqlLibC_instrb_back(search_str, search_val, start, 1, Gen_fparam);

    return retval;
}

UDREXPORT 
mi_integer SqlLibC_instrb_4
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
mi_integer   occurrence,
MI_FPARAM   *Gen_fparam 
)
{
    mi_integer retval;

    if (start >= 0)
        retval=SqlLibC_instrb(search_str, search_val, start, 
            occurrence, Gen_fparam);
    else
        retval=SqlLibC_instrb_back(search_str, search_val, start, 
            occurrence, Gen_fparam);

    return retval;
}


/****************************** instr ******************************************
**
** Function name:
**
**    SqlLibC_instr   - workhorse routine called by SqlLibC_instr_* functions
**    SqlLibC_instr_2 - two arguments
**    SqlLibC_instr_3 - three arguments
**    SqlLibC_instr_4 - four arguments
**
** Description:
**
**    Searches for a value in a string, and returns the position where 
**    it was found.
**
**    Entrypoint for the SQL routine instr(), which is overloaded for
**    three routine signatures.
**
**    This is identical to instrb, but handles multi-byte character
**    sets. The author of this code is new to GLS, so if something
**    looks wrong here, it probably is wrong (feel free to send 
**    corrections to jta@informix.com).
**
** Parameters:
**
**    search string      String we are searching through.
**    search value       Value we are searching for.
**    start position     Where to start the search.
**    occurrence         Nth occurrence to find.
**
** Return value:
**
**    mi_integer         Position of the found value, 0 if not found.
**
********************************************************************************
*/


mi_integer SqlLibC_instr
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_string  *func="instr";

    gl_mchar_t *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL;

    mi_integer position=0,  /* Character position the search_val was found at */
               count,       /* loop counter for occurrences */
               byte_len,    /* length in bytes */
               gl_errno;    /* GLS error code */
                
    SQL_TRACE_ENTER(func);

    /* Most GLS routines take a byte size or IFX_GL_NULL, 
    ** which lets you avoid making a copy of the argument.
    **    
    ** Working with NULL-terminated strings produces 
    ** simpler code, so that's what we've done here.
    */
    p1=(gl_mchar_t *) mi_lvarchar_to_string(search_str);
    p2=(gl_mchar_t *) mi_lvarchar_to_string(search_val);

    /* QA the input arguments. */
    qa_instr_args(func, ifx_gl_mbslen(p1, IFX_GL_NULL), start, occurrence, 
        Gen_fparam);

    /* Use a GLS routine to step forward character by character to
    ** the correct starting point.
    */
    p=p1; /* accommodates C programmers that think 0-based */
    for(count=1; count < start; count++)
    {
        ifx_gl_lc_errno() = 0;

        p = ifx_gl_mbsnext(p, IFX_GL_NO_LIMIT);
        if ( (gl_errno=ifx_gl_lc_errno()) != 0)
            raise_gl_error(func, "ifx_gl_mbsnext", gl_errno, Gen_fparam);

    } /* END: step forward to starting position */

    /* C's start is 0-based */
    for (count=0, find_it=NULL; count < occurrence; count++)
    {
        if (find_it == p) /* previous search found it in position 1 */
        {
            ifx_gl_lc_errno() = 0;
            p = ifx_gl_mbsnext(p, IFX_GL_NO_LIMIT); /* step past */
            if ( (gl_errno=ifx_gl_lc_errno()) != 0)
                raise_gl_error(func, "ifx_gl_mbsnext", gl_errno, Gen_fparam);
        }

        ifx_gl_lc_errno() = 0;

        find_it = ifx_gl_mbsmbs(p, IFX_GL_NULL, p2, IFX_GL_NULL);

        if ( (gl_errno=ifx_gl_lc_errno()) != 0)
            raise_gl_error(func, "ifx_gl_mbsmbs", gl_errno, Gen_fparam);

        if(find_it == (gl_mchar_t *)NULL)    /* not found */
        {
            p=NULL;
            break;
        }

        else
            p = find_it; 
    } /* END: for each occurrence */

    if(p == NULL) /* We didn't find it */
        position = 0;
    else
    {
        /* Calculate the position where it was found in # bytes */
        byte_len = p - p1;    

        /* Convert from # bytes to # multi-byte characters */
        ifx_gl_lc_errno() = 0;

        position = ifx_gl_mbslen(p1, byte_len);
        if ( (gl_errno=ifx_gl_lc_errno()) != 0)
            raise_gl_error(func, "ifx_gl_mbslen", gl_errno, Gen_fparam);

        /* Customers are 1-based, not 0-based .... */
        position++; 
    }

    mi_free(p1);
    mi_free(p2);

    SQL_TRACE_EXIT(func);

    return position;
}

/* SqlLibC_instr_back
**
** Implements backwards search for instr.
**
** See the detailed description for SqlLibC_instrb_back.
**
** The forwards and backwards search could easily be combined
** into a single function, but it it would be more difficult to
** understand what is going on.
*/
mi_integer SqlLibC_instr_back
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_string  *func="instr";

    gl_mchar_t *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL;

    mi_integer retval=0,
               end_point,
               temp_position=0,
               position[MAX_P],  /* Character positions found */
               count,       /* loop counter for occurrences */
               byte_len,    /* length in bytes */
               gl_errno;    /* GLS error code */
                
    SQL_TRACE_ENTER(func);

    if(occurrence > MAX_P)
    {
        mi_string error_buf[120];

        sprintf(error_buf, 
            "%s: oops! current implementation restricts occurrence to <= %d",
            func, MAX_P);

        mi_db_error_raise(NULL, MI_EXCEPTION, error_buf);

        /* NOT REACHED */
    }
    for(count=0; count<MAX_P; count++)
        position[count]=0;

    /* Convert input args to null-terminated strings.  */
    p1=(gl_mchar_t *) mi_lvarchar_to_string(search_str);
    p2=(gl_mchar_t *) mi_lvarchar_to_string(search_val);

    /* QA the input arguments. */
    qa_instr_args(func, ifx_gl_mbslen(p1, IFX_GL_NULL), start, occurrence, 
        Gen_fparam);

    /* Calculate the end point so we won't start a search beyond
    ** this point.
    */
    end_point = ifx_gl_mbslen(p1, IFX_GL_NULL) + start + 1;

    p=p1; 

    for (count=0, temp_position=0; temp_position < end_point ; count++)
    {
        if (find_it == p) /* previous search found it in position 1 */
        {
            ifx_gl_lc_errno() = 0;
            p = ifx_gl_mbsnext(p, IFX_GL_NO_LIMIT); /* step forward 1 */
            if ( (gl_errno=ifx_gl_lc_errno()) != 0)
                raise_gl_error(func, "ifx_gl_mbsnext", gl_errno, Gen_fparam);
        }

        ifx_gl_lc_errno() = 0;

        find_it = ifx_gl_mbsmbs(p, IFX_GL_NULL, p2, IFX_GL_NULL);

        if ( (gl_errno=ifx_gl_lc_errno()) != 0)
            raise_gl_error(func, "ifx_gl_mbsmbs", gl_errno, Gen_fparam);

        if(find_it == (gl_mchar_t *)NULL)    /* not found */
        {
            break;
        }
        else
        {
            /* Calculate the position where it was found in # bytes,
            ** then convert to multi-byte characters.
            */
            byte_len = find_it - p1;  
            ifx_gl_lc_errno() = 0;
            temp_position = ifx_gl_mbslen(p1, byte_len);
            if ( (gl_errno=ifx_gl_lc_errno()) != 0)
                raise_gl_error(func, "ifx_gl_mbslen", gl_errno, Gen_fparam);

            /* Did we end up going too far? */
            if (temp_position >= end_point)
                break;
            else
            {
                position[count] = temp_position;
                p = find_it;
            }
        }
    } /* END: for each occurrence up to the end point*/

    if(count == 0)              /* We didn't find any occurrences at all. */
        retval = 0;
    else if(count < occurrence) /* We didn't find enough occurrrences */
        retval=0;
    else
    {
        retval=position[count-occurrence];
        retval++;
    }

    mi_free(p1);
    mi_free(p2);

    SQL_TRACE_EXIT(func);

    return retval;
}


UDREXPORT 
mi_integer SqlLibC_instr_2
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
MI_FPARAM   *Gen_fparam 
)
{
    return SqlLibC_instr(search_str, search_val, 1, 1, Gen_fparam);
}

UDREXPORT 
mi_integer SqlLibC_instr_3
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
MI_FPARAM   *Gen_fparam
)
{
    mi_integer retval;

    if(start >= 0)
        retval = SqlLibC_instr(search_str, search_val, start, 1, Gen_fparam);
    else
        retval = SqlLibC_instr_back(search_str, search_val, start, 1, Gen_fparam);

    return retval;
}


UDREXPORT 
mi_integer SqlLibC_instr_4
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
mi_integer   occurrence,
MI_FPARAM   *Gen_fparam
)
{
    mi_integer retval;

    if(start >= 0)
        retval = SqlLibC_instr(search_str, search_val, start, 
            occurrence, Gen_fparam);
    else
        retval = SqlLibC_instr_back(search_str, search_val, start, 
            occurrence, Gen_fparam);

    return retval;
}


/************************** sign ***********************************************
**
** Function name:
**
**    SqlLibC_sign
**
** Description:
**    Takes a numeric argument and returns an integer that indicates if the 
**    input value is positive (1), negative (-1), or zero (0). 
**
**    Entrypoint for the SQL routine sign (double precision) returns integer.
**
** Parameters:
**
**    A numeric value.
**
** Return value:
**
**    mi_integer
**
********************************************************************************
*/

UDREXPORT 
mi_integer SqlLibC_sign
(
mi_double_precision  *input,
MI_FPARAM            *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer  Gen_RetVal;      /* The return value. */
    mi_string  *func="sign";

    SQL_TRACE_ENTER(func);

    if (*input < 0)
        Gen_RetVal = -1;
    else if (*input > 0)
        Gen_RetVal = 1;
    else
        Gen_RetVal = 0;

    SQL_TRACE_EXIT(func);

    return Gen_RetVal;
}

/********************************************************************************
** Character to numeric conversion functions:
**
**    SqlLibC_to_decimal      to_decimal(lvarchar)
**    SqlLibC_to_decimal_fmt  to_decimal(str lvarchar, gls_fmt lvarchar)
**    SqlLibC_to_float        to_float(lvarchar)
**    SqlLibC_to_float_fmt    to_float(lvarchar, gls_fmt lvarchar)
**    SqlLibC_to_integer      to_integer(lvarchar)
**    SqlLibC_to_integer_fmt  to_integer(lvarchar, gls_fmt lvarchar)
********************************************************************************
*/

/* to_decimal (lvarchar)
**
** Converts a string to a decimal by calling the builtin IDS system cast
** for lvarchar to decimal. It is functionally equivalent to this SPL UDR:
**
**    create function to_number (str_in lvarchar)       
**    returning decimal with (not variant);
**        return CAST (str_in as decimal);
**    end function;     
**
** The only advantage this C routine has is it can participate in PDQ,
** whereas a SPL UDR cannot. It wouldn't surprise me if the SPL UDR
** were slightly faster than the C UDR, but I have not timed it.
*/

UDREXPORT 
mi_decimal * SqlLibC_to_decimal
(
mi_lvarchar  *input_str,  /* Decimal value stored in a character string */
MI_FPARAM    *fp          /* UDR function parameter info */
)
{
    mi_decimal *retval=NULL;        /* The return value. */
    MI_DATUM   val;                 /* Intermediate result */
    mi_string  *func="to_decimal";

    SQL_TRACE_ENTER(func);

    /* Initialize return value to NULL. */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* Allocate the return result. We could store the return value
    ** in the MI_FPARAM for a slight performance improvement when
    ** it is executed across multiple rows. The following IDN tech
    ** note explains how:
    **     http://www.informix.com/idn-secure/DataBlade/Library/named_mem.htm#per_command
    */
    retval = (mi_decimal *) mi_alloc (sizeof(mi_decimal));
    if(retval == (mi_decimal *)NULL)
    {
        /* Raise the dbdk "memory allocation failed" error */
        mi_db_error_raise(NULL, MI_SQL, "UGEN2",
            "FUNCTION%s", func, (mi_integer) NULL);

        return (mi_decimal *)NULL; /* not reached */
    }

    /* Execute the system cast. */
    val = SqlLibC_cast_char(func, input_str, "decimal");
    *retval = *(mi_decimal *) val;

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* to_decimal (lvarchar, lvarchar)
**
** Converts a string value to a decimal by calling the
** GLS ifx_gl_convert_number() function, which provides 
** flexible numeric formatting in the input string.
**
** If the gls_fmt argument is NULL, ifx_gl_convert_number()
** uses LC_NUMERIC in the client environment.
*/

UDREXPORT 
dec_t * SqlLibC_to_decimal_fmt
(
mi_lvarchar  *input_str,
mi_lvarchar  *gls_fmt,
MI_FPARAM    *fp          /* UDR function parameter info */
)
{
    dec_t      *retval=NULL;      /* The return value. */
    mi_string  *func="to_decimal";

    mi_string  *str_tmp=NULL,
               *str_fmt=NULL;
    mint       error;

    SQL_TRACE_ENTER(func);

    /* initialize return value to NULL */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* Process the input arguments and allocate
    ** the return value.
    **
    ** This UDR handles NULL arguments, but it is
    ** only valid for the second argument to be NULL.
    ** Raise an error if the first argument is NULL.
    */
    if (mi_fp_argisnull(fp,0) == MI_TRUE)  /* Arg #1 */
    {
        mi_db_error_raise(NULL, 
            MI_SQL,       "USQ05",
            "UDRNAME%s",  func,
            "ARGUMENT%d", 1,
            (mi_integer) NULL);
        return (dec_t *)NULL; /* not reached */
    }
    else
        str_tmp = mi_lvarchar_to_string(input_str);

    if (mi_fp_argisnull(fp,1) == MI_FALSE)  /* Arg #2 */
        str_fmt = mi_lvarchar_to_string(gls_fmt);

    retval = (dec_t *) mi_alloc (sizeof(dec_t));
    if(retval == (dec_t *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2",
            "FUNCTION%s", func, (mi_integer) NULL);
        return (dec_t *)NULL;         /* not reached */
    }

    /* Perform the conversion */
    ifx_gl_lc_errno() = 0;
    ifx_gl_convert_number(retval, str_tmp, str_fmt);
    if ( (error=ifx_gl_lc_errno()) != 0)
    {
        raise_gl_error(func, "ifx_gl_mbslen", error, fp);
        return (dec_t *)NULL; /* not reached */
    }

    /* clean up */
    mi_free(str_tmp);
    if (mi_fp_argisnull(fp,1) == MI_FALSE)
        mi_free(str_fmt);

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* to_float (lvarchar)
**
** Converts a string to a float by calling the builtin IDS system cast
** for lvarchar to float. It is functionally equivalent to this SPL UDR:
**
**    create function to_number (str_in lvarchar)       
**    returning float with (not variant);
**        return CAST (str_in as float);
**    end function;     
**
** The only advantage this C routine has is it can participate in PDQ,
** whereas a SPL UDR cannot.
**
** Why not just call atof()? We'd have to write our own handling of
** underflow/overflow. Also, atof() silently stops reading at the
** first invalid character. We get all that error checking for free
** with the IDS system cast.
*/

UDREXPORT 
mi_double_precision * SqlLibC_to_float
(
mi_lvarchar  *input_str,
MI_FPARAM    *fp   /* UDR function parameter info */
)
{
    mi_double_precision *retval=NULL;  /* The return value. */
    mi_string           *func="to_float";
    MI_DATUM            val;

    SQL_TRACE_ENTER(func);

    /* Initialize return value to NULL. */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* Allocate the return result */
    retval = (mi_double_precision *) mi_alloc (sizeof(mi_double_precision));
    if(retval == (mi_double_precision *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2",
            "FUNCTION%s", func, (mi_integer) NULL);

        return (mi_double_precision *)NULL; /* not reached */
    }

    val = SqlLibC_cast_char(func, input_str, "float");
    *retval = *(mi_double_precision *) val;

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* to_float (lvarchar, lvarchar)
**
** Converts a string value to a float by calling the
** GLS ifx_gl_convert_number() function, which supports
** localized floating point formats. Then converts from
** decimal to double with dectodbl().
**
** Disadvantage: converts via a decimal, which has
** less precision than a native double precision float.
**
** If the gls_fmt argument is NULL, ifx_gl_convert_number()
** uses LC_NUMERIC in the client environment.
*/

UDREXPORT 
mi_double_precision * SqlLibC_to_float_fmt
(
mi_lvarchar  *input_str,
mi_lvarchar  *gls_fmt,
MI_FPARAM    *fp          /* UDR function parameter info */
)
{
    mi_string           *func="to_float";
    mi_double_precision *retval=NULL; /* return value */
    dec_t               decval;       /* intermediate decimal value */

    mi_string  *str_tmp=NULL,
               *str_fmt=NULL;

    mint       error;

    SQL_TRACE_ENTER(func);

    /* initialize return value to NULL */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* This UDR handles NULL arguments. 
    ** Raise an error if the first argument is NULL.
    */
    if (mi_fp_argisnull(fp,0) == MI_TRUE)  /* Arg #1 */
    {
        mi_db_error_raise(NULL, 
            MI_SQL,       "USQ05",
            "UDRNAME%s",  func,
            "ARGUMENT%d", 1,
            (mi_integer) NULL);
        return (mi_double_precision *)NULL; /* not reached */
    }
    else
        str_tmp = mi_lvarchar_to_string(input_str);

    if (mi_fp_argisnull(fp,1) == MI_FALSE)  /* Arg #2 */
        str_fmt = mi_lvarchar_to_string(gls_fmt);

    retval = (mi_double_precision *) mi_alloc (sizeof(mi_double_precision));
    if(retval == (mi_double_precision *)NULL)
    {
        /* Raise the dbdk "memory allocation failed" error */
        mi_db_error_raise(NULL, MI_SQL, "UGEN2",
            "FUNCTION%s", func, (mi_integer) NULL);

        return (mi_double_precision *)NULL; /* not reached */
    }

    ifx_gl_lc_errno() = 0;
    ifx_gl_convert_number(&decval, str_tmp, str_fmt);
    if ( (error=ifx_gl_lc_errno()) != 0)
    {
        raise_gl_error(func, "ifx_gl_mbslen", error, fp);
        return (mi_double_precision *)NULL; /* not reached */
    }

    error = dectodbl(&decval, retval);
    if(error != 0)
    {
        mi_db_error_raise(NULL, MI_EXCEPTION,
           "to_float: conversion of value from dec to float failed!");
        return (mi_double_precision *)NULL; /* not reached */
    }

    /* clean up */
    mi_free(str_tmp);
    if (mi_fp_argisnull(fp,1) == MI_FALSE)
        mi_free(str_fmt);

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* to_integer (lvarchar)
**
** Converts a string to an integer by calling the builtin IDS system cast
** for lvarchar to integer. It is functionally equivalent to this SPL UDR:
**
**    create function to_number (str_in lvarchar)       
**    returning integer with (not variant);
**        return CAST (str_in as integer);
**    end function;     
**
** The only advantage this C routine has is it can participate in PDQ,
** whereas a SPL UDR cannot.
**
** Why not just call atoi()? We'd have to write our own handling for
** underflow/overflow. Also, atoi() silently stops reading at the
** first invalid character. We get all that error checking for free
** with the IDS system cast.
*/

UDREXPORT 
mi_integer SqlLibC_to_integer
(
mi_lvarchar  *input_str,
MI_FPARAM    *fp          /* UDR function parameter info */
)
{
    mi_integer retval;  /* The return value. */
    mi_string  *func="to_integer";
    MI_DATUM   val;

    SQL_TRACE_ENTER(func);

    /* Initialize return value to NULL. */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    val = SqlLibC_cast_char(func, input_str, "integer");
    retval = (mi_integer) val; 

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* to_integer (lvarchar, lvarchar)
**
** Converts a string value to a float by calling the
** GLS ifx_gl_convert_number() function, which supports
** alternate formats. Then converts from
** decimal to mi_integer with dectolong().
**
** If the gls_fmt argument is NULL, ifx_gl_convert_number()
** uses LC_NUMERIC in the client environment.
*/

UDREXPORT 
mi_integer SqlLibC_to_integer_fmt
(
mi_lvarchar  *input_str,
mi_lvarchar  *gls_fmt,
MI_FPARAM    *fp          /* UDR function parameter info */
)
{
    mi_string  *func="to_integer";
    mi_integer retval;       /* return value */
    dec_t      decval;       /* intermediate decimal value */
    mi_string  *str_tmp=NULL,
               *str_fmt=NULL;
    mint       error;

    SQL_TRACE_ENTER(func);

    /* initialize return value to NULL */
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* This UDR handles NULL arguments. 
    ** Raise an error if the first argument is NULL.
    */
    if (mi_fp_argisnull(fp,0) == MI_TRUE)  /* Arg #1 */
    {
        mi_db_error_raise(NULL, 
            MI_SQL,       "USQ05",
            "UDRNAME%s",  func,
            "ARGUMENT%d", 1,
            (mi_integer) NULL);
        return MI_ERROR; /* not reached */
    }
    else
        str_tmp = mi_lvarchar_to_string(input_str);

    if (mi_fp_argisnull(fp,1) == MI_FALSE)  /* Arg #2 */
        str_fmt = mi_lvarchar_to_string(gls_fmt);

    ifx_gl_lc_errno() = 0;
    ifx_gl_convert_number(&decval, str_tmp, str_fmt);
    if ( (error=ifx_gl_lc_errno()) != 0)
    {
        raise_gl_error(func, "ifx_gl_mbslen", error, fp);
        return MI_ERROR; /* not reached */
    }

    error = dectolong(&decval, (int4 *)&retval);
    if(error != 0)
    {
        mi_db_error_raise(NULL, MI_EXCEPTION,
           "to_integer: conversion of value from dec to integer failed!");
        return MI_ERROR; /* not reached */
    }

    /* clean up */
    mi_free(str_tmp);
    if (mi_fp_argisnull(fp,1) == MI_FALSE)
        mi_free(str_fmt);

    SQL_TRACE_EXIT(func);

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/************************** SqlLibC_Trace ***************************************
**
** Function name:
**
**    TraceSet_SqlLibC
**
** Description:
**
**   Sets the trace filename  and trace level for the SqlLibC trace class.
**   Also sets the level for the __myErrors__ trace class so that any
**   errors get interleaved with the trace output.
**
** Parameters:
**
**    lvarchar: trace filename
**    integer:  trace level
**
** Return value:
**
**    none
**
********************************************************************************
*/


UDREXPORT
void TraceSet_SqlLibC
(
mi_lvarchar *Gen_param,     /* Name of file to output trace messages to. */
mi_integer  Gen_TraceLevel, /* Trace level threshold.         */
MI_FPARAM  *Gen_fparam      /* UDR function parameter info */
)
{
    gl_mchar_t *Gen_TraceFile;     /* Output trace file name.  */
    char        Gen_TraceStr[50];  /* String to build tracelevel call. */   
    mi_string   error_buf[300];

    /*
    ** Set the name of the output trace file. If not set, 
    ** the name defaults to /tmp/<session id>.trc.
    */
    if( mi_get_varlen( Gen_param ) != 0 )
    {
        /* Convert input file name to NULL-terminated string. */
        Gen_TraceFile = (gl_mchar_t *)mi_lvarchar_to_string( Gen_param );

        /* Set the name of the trace file. */
        if( mi_tracefile_set( (char *)Gen_TraceFile ) == -1 )
        {
            sprintf(error_buf, 
                "mi_tracefile_set failed for '%s'", Gen_TraceFile);
            mi_db_error_raise(NULL, MI_EXCEPTION, error_buf);
            /* not reached */
        }
        mi_free( Gen_TraceFile );
    }

    /* Set the tracing level (default = 0). */
    if( Gen_TraceLevel >= 0 )
    {
        sprintf( Gen_TraceStr, "%s %d", SQL_TRACE_CLASS, Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            sprintf(error_buf, "mi_tracelevel_set failed for '%s'", 
               SQL_TRACE_CLASS);
            mi_db_error_raise(NULL, MI_EXCEPTION, error_buf);
            /* not reached */
        }

        /* Also set the level for the __myErrors__ trace class */
        sprintf( Gen_TraceStr, "__myErrors__ %d", Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            sprintf(error_buf, "mi_tracelevel_set failed for '__myErrors__'");
            mi_db_error_raise(NULL, MI_EXCEPTION, error_buf);
            /* not reached */
        }
    }

    /* SQL_TRACE_EXIT is our own macro that calls GL_DPRINTF.
    **
    ** The first time you call GL_DPRINTF in a client session,
    ** GLS messages get fetched from the systracemsgs system catalog
    ** with a SQL statement and cached for the rest of the client 
    ** session. We go ahead and do a trace call now to force that 
    ** initialization (and the SQL statement) to occur immediately.
    **
    ** With initialization complete, the UDRs that later call
    ** GL_DPRINTF are parallelizable.
    */

    SQL_TRACE_EXIT("TraceSet_SqlLibC" );
}

/********************************************************************
** Miscellaneous functions called internally by other functions.
**
** Normally you would declare these functions as static. We didn't do
** that here so they will show up in MS DevStudio -- in other words,
** for demo'ing.
*********************************************************************
*/

/* raise_gl_error
**
** Called by SqlLibC_instr to process GLS library error.
*/
void
raise_gl_error 
(
mi_string  *func, 
mi_string  *gl_call, 
mi_integer  gl_errno, 
MI_FPARAM  *Gen_fparam
)
{
    /* Set the UDR return value to NULL -- super cautious */

    mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

    switch (gl_errno)
    {
        case IFX_GL_PARAMERR:
            mi_db_error_raise( NULL, 
                MI_SQL,        "USQ03",
                "UDRNAME%s",   func,
                "GLS_CALL%s",  gl_call,
                "GLS_ERROR%s", "IFX_GL_PARAMERR",
                (mi_integer) NULL);

            break;    /* NOT REACHED */

        case IFX_GL_TERMMISMAT:
            mi_db_error_raise( NULL, 
                MI_SQL,        "USQ03",
                "UDRNAME%s",   func,
                "GLS_CALL%s",  gl_call,
                "GLS_ERROR%s", "IFX_GL_TERMMISMAT",
                (mi_integer) NULL);

            break;    /* NOT REACHED */

        case IFX_GL_EILSEQ:
            mi_db_error_raise( NULL, 
                MI_SQL,        "USQ03",
                "UDRNAME%s",   func,
                "GLS_CALL%s",  gl_call,
                "GLS_ERROR%s", "IFX_GL_EILSEQ",
                (mi_integer) NULL);

            break;    /* NOT REACHED */

        case IFX_GL_EINVAL:
            mi_db_error_raise( NULL, 
                MI_SQL,        "USQ03",
                "UDRNAME%s",   func,
                "GLS_CALL%s",  gl_call,
                "GLS_ERROR%s", "IFX_GL_EINVAL",
                (mi_integer) NULL);
            break;    /* NOT REACHED */

        default:
            mi_db_error_raise( NULL, 
                MI_SQL,        "USQ03",
                "UDRNAME%s",   func,
                "GLS_CALL%s",  gl_call,
                "GLS_ERROR%d", gl_errno,
                (mi_integer) NULL);

            break;    /* NOT REACHED */
    }
    return;
}

/* qa_instr_args
**
** Called by SqlLibC_instr and SqlLibC_instrb to QA the input arguments.
*/
void
qa_instr_args 
(
mi_string   *func,
mi_integer  search_len,
mi_integer  start,
mi_integer  occurrence,
MI_FPARAM   *Gen_fparam
)
{
    if(occurrence <= 0)
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        mi_db_error_raise( NULL, 
            MI_SQL,        "USQ01",
            "UDRNAME%s",   func,
            "VALUE%d",     occurrence,
            "RANGE%s",     "> 0",
            (mi_integer) NULL);

        return;    /* NOT REACHED */
    }

    /* Starting position is past the end of the string */
    if(start > search_len )
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        mi_db_error_raise( NULL, 
            MI_SQL,        "USQ01",
            "UDRNAME%s",   func,
            "VALUE%d",     start,
            "RANGE%s",     "> search string", /* oops -- English text */
            (mi_integer) NULL);

        return;    /* NOT REACHED */
    }

    return;
}

/* raise_builtin_err
**
** mi_db_error_raise lets you raise user-defined errors stored in
** the syserrors catalog, but it can't raise built-in SQL errors.
**
** But an SPL routine can raise a builtin error. So this function
** uses "fastpath" to execute an SPL procedure that raises the error:
**
**   create function sqllibc_raise_err(error_code integer)
**   returning integer;
**         raise exception error_code;
**         return error_code;
**   end function;
**
** The SqlLibC blade registration creates this SPL function.
*/

void 
raise_builtin_err 
(mi_integer error_num)
{
    MI_CONNECTION *conn=NULL;
    MI_FUNC_DESC  *funcdesc=NULL; /* returned by mi_routine_get() */
    mi_integer    funcerror;      /* mi_routine_exec error status */
    mi_string     *funcsig="sqllibc_raise_err(int)"; /* proc to execute */

    /* ===== Open a connection ======================================= */
    if ((conn = mi_open(NULL,NULL,NULL)) == (MI_CONNECTION *)NULL)
    {
        mi_db_error_raise(NULL, MI_EXCEPTION,
            "SqlLibC_raise_err: mi_open failed");
        return;
    }

    /* ===== Get the function descriptor ============================= */
    if((funcdesc=mi_routine_get(conn, 0, funcsig)) == (MI_FUNC_DESC *)NULL)
    {
        mi_db_error_raise(NULL,MI_EXCEPTION, 
            "SqlLibC_raise_err: mi_routine_get failed");
        return;
    }

    /* ===== Execute the function ==================================== */
    (void) mi_routine_exec(conn, funcdesc, &funcerror, error_num);
    if(funcerror == MI_ERROR)
    {
        mi_db_error_raise(NULL,MI_EXCEPTION, 
            "SqlLibC_raise_err: mi_routine_exec failed");
        return;
    }

    /* This point NOT REACHED because we executed an SPL proc that 
    ** raised an exception.
    */
    mi_routine_end(conn, funcdesc);
    mi_close(conn);                              

    return;
}

MI_DATUM  /* result will be decimal, float, or integer */
SqlLibC_cast_char
(
  mi_string   *func,       /* name of the function calling this one */
  mi_lvarchar *source_data, 
  mi_string *target_t      /* "decimal", "float", or "integer" */
)
{
    MI_DATUM      retval=NULL;
    MI_CONNECTION *conn=NULL;
    MI_FUNC_DESC  *funcdesc=NULL; /* returned by mi_routine_get() */
    mi_integer    funcerror;      /* mi_routine_exec error status */
    mi_string     error[80];
    MI_TYPE_DESC  *src_type=NULL,    /* Input arg to mi_td_cast_get() */
                  *dest_type=NULL;   /* Input arg to mi_td_cast_get() */
    mi_char       cast_status;       /* filled in by mi_td_cast_get() */

    /* ===== Open a connection ======================================= */
    if ((conn = mi_open(NULL,NULL,NULL)) == (MI_CONNECTION *)NULL)
    {
        sprintf(error, "%s: mi_open failed", func);
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }

    /* ===== Get the type descriptors ================================ */
    src_type=mi_typestring_to_typedesc(conn, "lvarchar");
    if(src_type == (MI_TYPE_DESC *)NULL )
    {
        sprintf(error, "%s: mi_typestring_to_typedesc failed for lvarchar", 
            func);
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }

    dest_type=mi_typestring_to_typedesc(conn, target_t);
    if(dest_type == (MI_TYPE_DESC *)NULL )
    {
        sprintf(error, "%s: mi_typestring_to_typedesc failed for %s", 
            func, target_t);
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }

    /* ===== Get the function descriptor ============================= */

    /* Get the function descriptor for the cast routine that
    ** converts lvarchar to the destination type.
    */
    funcdesc = mi_td_cast_get (conn, src_type, dest_type, &cast_status);

    if(cast_status == MI_ERROR_CAST)
    {
        sprintf(error, "%s: mi_td_cast_get failed", func);
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }
    else if(cast_status != MI_SYSTEM_CAST)
    {
        sprintf(error, "%s: oops! cast_status=%c", func, cast_status);
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }
  
    /* ===== Execute the function ==================================== */
    /* mi_routine_exec() for a system cast function takes
    ** three arguments: the value, length, and precision.
    **
    ** Get the length with mi_type_maxlength().
    ** Don't use mi_type_length()--it returns -1 for
    ** variable-length types, such as varchar/char.
    */

    retval = mi_routine_exec (conn, funcdesc, &funcerror, source_data,
        mi_type_maxlength(src_type),
        mi_type_precision(src_type)
    );

    if(funcerror != MI_OK)
    {
        sprintf(error, "%s: mi_routine_exec failed", func );
        mi_db_error_raise (conn, MI_EXCEPTION, error);
        return (MI_DATUM)NULL; /* not reached */
    }

    mi_routine_end(conn, funcdesc);
    mi_close(conn);                              

    return retval;
}
