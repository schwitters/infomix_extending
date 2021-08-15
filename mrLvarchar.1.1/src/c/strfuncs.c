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
** Title:          strfuncs.c
** Author:         jta@informix.com
** Created:        04/16/2001 
** Description:    String manipulation UDRs for the mrLvarchar DataBlade module.
**
**  idn_mrLvarcharUpper    Converts value to upper case.
**  idn_mrLvarcharLower    Converts value to lower case.
**  idn_mrLvarchar_SubStr  Returns a substring from the original.
**  idn_mrLvarchar_SubStrb Returns a substring from the original.
**  idn_mrLvarchar_instr   Returns the index of a string within a string (GLS).
**  idn_mrLvarchar_instrb  Returns the index of a string within a string (fast).
*/

#include "mrLvarchar.h"

/* ================ Local prototypes ================ */
static
void qa_instr_args  (mi_string *func, mi_integer search_len, mi_integer start,
                     mi_integer  occurrence, MI_FPARAM *Gen_fparam);
static
mi_integer idn_mrLvarchar_instr ( mi_lvarchar *search_str,
                     mi_lvarchar *search_val, mi_integer start,
                     mi_integer occurrence, MI_FPARAM *Gen_fparam);
static
mi_integer idn_mrLvarchar_instr_back (mi_lvarchar *search_str,
                     mi_lvarchar *search_val, mi_integer start,
                     mi_integer occurrence, MI_FPARAM *Gen_fparam);
static
mi_integer idn_mrLvarchar_instrb (mi_lvarchar *search_str,
                     mi_lvarchar *search_val, mi_integer start,
                     mi_integer occurrence, MI_FPARAM *Gen_fparam);
static
mi_integer idn_mrLvarchar_instrb_back (mi_lvarchar *search_str,
                     mi_lvarchar *search_val, mi_integer start,
                     mi_integer occurrence, MI_FPARAM *Gen_fparam);

/* idn_mrLvarcharUpper()
**
** Entrypoint for the Upper(idn_mrLvarchar) UDR.
**
** Converts the input UDT to uppercase using the GLS routines.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharUpper
(
mi_lvarchar *object,   /* Source UDT                  */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    mi_lvarchar *retval=NULL;    /* Return value */

    mi_char     *source=NULL,    /* NULL-terminated copy of source */
                *target=NULL;    /* NULL-terminated copy of target */

    gl_mchar_t  *s_ptr=NULL,     /* pointer to source data */
                *t_ptr=NULL;     /* pointer to target data */

    mi_integer  s_size=0,        /* source size in bytes */
                t_size=0;        /* target size in bytes (estimated) */
                
    unsigned short status;       /* ifx_* function return code */

    mi_string   msgbuf[80],
                *r="idn_mrLvarcharUpper";

    FUNC_TRACE_ENTER( r );

    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* Get a fully expanded copy of the input value. 
    ** mrLvarchar_to_char() sets the data size of the 's_size' 
    ** arg and NULL-terminates the 'source' return value.
    */
    source=mrLvarchar_to_char(object, &s_size);
    if(source == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Calculate the size of the target */
    t_size = ifx_gl_case_conv_outbuflen(s_size);

    /* If the sizes are the same, we can modify the 'source'
    ** in place. Otherwise, we need to allocate the 'target',
    ** then convert from source into the target.
    */

    if(s_size == t_size)
    {
        target=source;
    }
    else
    {
        /* Allocate an additional byte for NULL-terminator.
        ** mi_zalloc initializes memory to NULL (like calloc).
        */
        target = (mi_char *)mi_zalloc(t_size + 1);
        if(target == (mi_char *) NULL)
        {
            sprintf(msgbuf, "%s: mi_alloc failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
    }

    s_ptr=(gl_mchar_t *)source; 
    t_ptr=(gl_mchar_t *)target;

    /* Convert the target to uppercase. */
    while (*s_ptr != '\0')
    {
        status = ifx_gl_tomupper(t_ptr, s_ptr, IFX_GL_NO_LIMIT);
        if(status == 0)
        {
            sprintf(msgbuf, "%s: ifx_gl_tomupper failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        s_ptr += IFX_GL_CASE_CONV_SRC_BYTES(status);
        t_ptr += IFX_GL_CASE_CONV_DST_BYTES(status);
    }

    /* Convert the modified copy back into an mrLvarchar. */
    retval = mrLvarchar_from_char(target, strlen(target), fp);
    if(retval == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_from_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* ======= Cleanup and return ======= */

    mi_free(source);

    if(s_size != t_size) /* we allocated target, so should free it */
        mi_free(target);

    FUNC_TRACE_EXIT( r );

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* idn_mrLvarcharLower()
**
** Entrypoint for the Lower(idn_mrLvarchar) UDR.
**
** Converts the input UDT to lowercase using the GLS routines.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharLower
(
mi_lvarchar *object,   /* Source data to be converted */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    mi_lvarchar *retval=NULL;   /* Final return value */

    mi_char     *source=NULL,   /* NULL-terminated source */
                *target=NULL;   /* NULL-terminated target */

    gl_mchar_t  *s_ptr=NULL,    /* pointer to 'source' data */
                *t_ptr=NULL;    /* pointer to 'target' data */

    mi_integer  s_size=0,       /* source string size in bytes */
                t_size=0;       /* target string size in bytes (estimated) */
                
    unsigned short status;      /* ifx_* function return code */

    mi_string   msgbuf[80],
                *r="idn_mrLvarcharLower";

    FUNC_TRACE_ENTER( r );

    mi_fp_setreturnisnull(fp, 0, MI_TRUE);

    /* Get a fully expanded copy of the input value. 
    ** mrLvarchar_to_char() sets the data size of the 's_size' 
    ** arg and NULL-terminates the 'source' return value.
    */
    source=mrLvarchar_to_char(object, &s_size);
    if(source == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Calculate the size of the target */
    t_size = ifx_gl_case_conv_outbuflen(s_size);

    /* If the sizes are the same, we can modify the 'source'
    ** in place. Otherwise, we need to allocate the 'target',
    ** then convert from source into the target.
    */

    if(s_size == t_size)
    {
        target=source;
    }
    else
    {
        /* Allocate an additional byte for NULL-terminator.
        ** mi_zalloc initializes memory allocated to NULL.
        */
        target = (mi_char *)mi_zalloc(t_size + 1);
        if(target == (mi_char *) NULL)
        {
            sprintf(msgbuf, "%s: mi_alloc failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
    }

    s_ptr=(gl_mchar_t *)source; 
    t_ptr=(gl_mchar_t *)target;

    /* Convert the target to uppercase. */
    while (*s_ptr != '\0')
    {
        status = ifx_gl_tomlower(t_ptr, s_ptr, IFX_GL_NO_LIMIT);
        if(status == 0)
        {
            sprintf(msgbuf, "%s: ifx_gl_tomlower failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        s_ptr += IFX_GL_CASE_CONV_SRC_BYTES(status);
        t_ptr += IFX_GL_CASE_CONV_DST_BYTES(status);
    }

    /* Convert the modified copy back to an mrLvarchar. */
    retval = mrLvarchar_from_char(target, strlen(target), fp);
    if(retval == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_from_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* ======= Cleanup and return ======= */

    mi_free(source);

    if(s_size != t_size) /* we allocated target, so should free it */
        mi_free(target);

    FUNC_TRACE_EXIT( r );

    mi_fp_setreturnisnull(fp, 0, MI_FALSE);
    return retval;
}

/* idn_mrLvarchar_SubStr()
**
** Entrypoint for the Snip(idn_mrLvarchar) UDR.
**
** Extracts a substring from the original; returns substring as
** idn_mrLvarchar.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarchar_SubStr_2
(
mi_lvarchar *mr_input, /* Source data to be converted */
mi_integer   start,    /* Starting character position */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    return idn_mrLvarchar_SubStr_3(mr_input, start, -1, fp);
}

UDREXPORT
mi_lvarchar *
idn_mrLvarchar_SubStr_3
(
mi_lvarchar *mr_input, /* Source data to be converted */
mi_integer   start,    /* Starting character position */
mi_integer   length,   /* Number of characters */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    mi_lvarchar *retval=NULL;
    gl_mchar_t  *p,
                *data=NULL,
                *new_data=NULL;
    mrLvarchar  *object;
    mi_integer  count,
                size,
                num_char,
                begin,
                copy_char,
                gl_errno;    /* GLS error code */
    mi_string   msgbuf[80],
                *r="SubStr";

    FUNC_TRACE_ENTER( r );

    if(  ( start == 1 || start == 0) && length == -1)
       return mr_input;

    object = (mrLvarchar *) mi_get_vardata (mr_input);

    /* Get a fully expanded copy of the data */
    data = (gl_mchar_t *)mrLvarchar_to_char(mr_input, &size);
    if(data == (gl_mchar_t *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Find out how many characters are in this string */
    num_char = ifx_gl_mbslen(data, IFX_GL_NULL);

    /* The built-in substr function returns NULL if the start point
    ** is greater than the string length.
    */
    if(start > num_char)
    {
       mi_fp_setreturnisnull(fp, 0, MI_TRUE);
       mi_free (data);
       return (mi_lvarchar *)NULL; 
    }

    /* ========= Set the beginning ========= */

    if(start < 0) /* negative start is from end-of-string */
       begin = num_char + start;
    else if ( (start == 0) || (start == 1) )
       begin = 0;
    else
       begin = start-1; /* C is 0-based */

    sprintf(msgbuf, "begin=%d", begin);
    GL_DPRINTF(TRACE_CLASS,
        TRACE_MEDIUM,
        (    "idn_mrLvarchar_ANY_TEXT",
            "FUNCNAME%s",   r,
            "MESSAGE%s",    msgbuf,
            MI_LIST_END
        )
    );

    /* ============ Set the number of characters to copy ============ */
    if(  (length < 0)
      || (begin + length > num_char)
      )
        copy_char = num_char - begin;
    else
        copy_char = length;

    sprintf(msgbuf, "copy_char=%d", copy_char);
    GL_DPRINTF(TRACE_CLASS,
        TRACE_MEDIUM,
        (    "idn_mrLvarchar_ANY_TEXT",
            "FUNCNAME%s",   r,
            "MESSAGE%s",    msgbuf,
            MI_LIST_END
        )
    );

    /* It's difficult with a multibyte string to know how many bytes
     * will be in the final result. So, initially, allocate for worst
     * case (and make sure it is NULL-terminated).
     */
    new_data = (gl_mchar_t *) mi_zalloc(object->hdr.size+1);
    if(new_data == (gl_mchar_t *)NULL)
    {
        sprintf(msgbuf, "%s: mi_alloc failed for %d bytes!", r, 
              object->hdr.size);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Use a GLS routine to step forward character by character to
    ** the correct starting point.
    */
    p = data;
    for(count=0; count < begin; count++)
    {
        ifx_gl_lc_errno() = 0;

        p = ifx_gl_mbsnext(p, IFX_GL_NO_LIMIT);
        if ( (gl_errno=ifx_gl_lc_errno()) != 0)
            raise_gl_error(r, "ifx_gl_mbsnext", gl_errno, fp);

    } /* END: step forward to starting position */

    /* Now copy the number of characters we are supposed to */
    ifx_gl_lc_errno() = 0;

    ifx_gl_mbsncpy(
        new_data,        /* Target */
        p,               /* Source */
        IFX_GL_NULL,     /* It's a NULL-terminated string */
        copy_char        /* Number of multibyte characters to read */
    );

    if ( (gl_errno=ifx_gl_lc_errno()) != 0)
        raise_gl_error(r, "ifx_gl_mbsnext", gl_errno, fp);

    /* Convert the result back into an idn_mrlvarchar 
     * ifx_gl_mbsntsbytes ignores trailing spaces. If you
     * want that behavior, uncomment the next call and comment
     * out strlen: 
     *
     *     size=ifx_gl_mbsntsbytes(new_data, IFX_GL_NULL);
     */
    size=(mi_integer)strlen((char *)new_data);
    retval = mrLvarchar_from_char((mi_char *)new_data, size, fp);

    mi_free(data);
    mi_free(new_data);

    FUNC_TRACE_EXIT( r );

    return retval;
}

/* idn_mrLvarchar_SubStrb()
**
** Entrypoint for the Snipb(idn_mrLvarchar) UDR.
**
** Extracts a substring from the original; returns substring as
** idn_mrLvarchar.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarchar_SubStrb_2
(
mi_lvarchar *mr_input, /* Source data to be converted */
mi_integer   start,    /* Start position in bytes */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    return idn_mrLvarchar_SubStrb_3(mr_input, start, -1, fp);
}

UDREXPORT
mi_lvarchar *
idn_mrLvarchar_SubStrb_3
(
mi_lvarchar *mr_input, /* Source data to be converted */
mi_integer   start,    /* Start position in bytes     */
mi_integer   length,   /* Length in bytes             */
MI_FPARAM   *fp        /* UDR function parameter info */
)
{
    mi_lvarchar *retval=NULL;
    mi_char     *p,
                *data=NULL,
                *new_data=NULL;
    mrLvarchar  *object;
    mi_integer  size,
                begin,
                num_bytes;
    mi_string   msgbuf[80],
                *r="SubStrb";

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *) mi_get_vardata (mr_input);

    /* The built-in substr function returns NULL if the start point
    ** is greater than the string length.
    */
    if(start > object->hdr.size)
    {
       mi_fp_setreturnisnull(fp, 0, MI_TRUE);
       return (mi_lvarchar *)NULL; 
    }

    if(  ( start == 1 || start == 0) && length == -1)
       return mr_input;

    /* ========= Set the beginning ========= */

    if(start < 0) /* negative start is from end-of-string */
       begin = object->hdr.size + start;
    else if ( (start == 0) || (start == 1) )
       begin = 0;
    else
       begin = start-1; /* C is 0-based */

    sprintf(msgbuf, "begin=%d", begin);
    GL_DPRINTF(TRACE_CLASS,
        TRACE_MEDIUM,
        (    "idn_mrLvarchar_ANY_TEXT",
            "FUNCNAME%s",   r,
            "MESSAGE%s",    msgbuf,
            MI_LIST_END
        )
    );

    /* ============ Set the number of bytes to copy ============ */
    if(  (length < 0)
      || (begin + length > object->hdr.size)
      )
        num_bytes = object->hdr.size - begin;
    else
        num_bytes = length;

    sprintf(msgbuf, "num_bytes=%d", num_bytes);
    GL_DPRINTF(TRACE_CLASS,
        TRACE_MEDIUM,
        (    "idn_mrLvarchar_ANY_TEXT",
            "FUNCNAME%s",   r,
            "MESSAGE%s",    msgbuf,
            MI_LIST_END
        )
    );

    new_data = (mi_char *) mi_alloc(num_bytes);
    if(new_data == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mi_alloc failed for %d bytes!", r, num_bytes);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Get a fully expanded copy of the data */
    data=mrLvarchar_to_char(mr_input, &size);
    if(data == (mi_char *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    p = data;
    p += begin;

    memcpy(new_data, p, num_bytes);

    /* Convert back into an idn_mrlvarchar */
    retval = mrLvarchar_from_char(new_data, num_bytes, fp);

    mi_free(data);
    mi_free(new_data);

    FUNC_TRACE_EXIT( r );

    return retval;
}

#define MAX_P 100

/****************************** Instr ******************************************
**
** Function name:
**
**    idn_mrLvarchar_instr   - workhorse routine called by *_instr_* functions
**    idn_mrLvarchar_instr_2 - two arguments
**    idn_mrLvarchar_instr_3 - three arguments
**    idn_mrLvarchar_instr_4 - four arguments
**
** Description:
**
**    ==> Pulled in from SqlLibC.1.1.
**
**    Searches for a literal value in a string, and returns the position where 
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

static
mi_integer idn_mrLvarchar_instr
(
mi_lvarchar *search_str,  /* idn_mrLvarchar: String we are searching through */
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
               size,        /* length in bytes */
               byte_len,    /* length in bytes */
               gl_errno;    /* GLS error code */
                
    FUNC_TRACE_ENTER( func );

    /* Most GLS routines take a byte size or IFX_GL_NULL, 
    ** which lets you avoid making a copy of the argument.
    **    
    ** Working with NULL-terminated strings produces 
    ** simpler code, so that's what we've done here.
    */
    p1=(gl_mchar_t *) mrLvarchar_to_char(search_str, &size);
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

    FUNC_TRACE_EXIT( func );

    return position;
}

/* idn_mrLvarchar_instr_back
**
** Implements backwards search for instr.
**
** See the detailed description for idn_mrLvarchar_instrb_back.
**
** The forwards and backwards search could easily be combined
** into a single function, but it it would be more difficult to
** understand what is going on.
*/
static
mi_integer idn_mrLvarchar_instr_back
(
mi_lvarchar *search_str,  /* idn_mrLvarchar: String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_string  msgbuf[80],
               *func="instr";

    gl_mchar_t *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL;

    mi_integer retval=0,
               end_point,
               temp_position=0,
               num_position,
               *position=NULL,      /* Character positions found */
               *tmp_position=NULL,  /* For re-allocating array */
               count,       /* loop counter for occurrences */
               size,    
               byte_len,    /* length in bytes */
               gl_errno;    /* GLS error code */
                
    FUNC_TRACE_ENTER( func );

    num_position = MAX_P;
    position = (mi_integer *)mi_zalloc(sizeof(mi_integer) * num_position);
    if(position == (mi_integer)NULL)
    {
        sprintf(msgbuf, "%s: mi_alloc failed!", func);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
    }

    /* Convert input args to null-terminated strings.  */
    p1=(gl_mchar_t *) mrLvarchar_to_char(search_str, &size);
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
        if(count == num_position) /* int array not big enough */
        {
            GL_DPRINTF(TRACE_CLASS,
                TRACE_MEDIUM,
                (    "idn_mrLvarchar_ANY_TEXT",
                    "FUNCNAME%s",   func,
                    "MESSAGE%s",    "reallocating position array",
                    MI_LIST_END
                )
            );
            tmp_position = 
                 (mi_integer *)mi_zalloc(sizeof(mi_integer) * num_position * 2);
            if(tmp_position == (mi_integer)NULL)
            {
                sprintf(msgbuf, "%s: mi_alloc failed!", func);
                mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            }

            memcpy(tmp_position, position, num_position);

            mi_free(position);
            num_position *= 2;
            position = tmp_position;
        }

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
    mi_free(position); /* Free the integer array */

    FUNC_TRACE_EXIT( func );

    return retval;
}


UDREXPORT 
mi_integer idn_mrLvarchar_instr_2
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
MI_FPARAM   *Gen_fparam 
)
{
    return idn_mrLvarchar_instr(search_str, search_val, 1, 1, Gen_fparam);
}

UDREXPORT 
mi_integer idn_mrLvarchar_instr_3
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
MI_FPARAM   *Gen_fparam
)
{
    mi_integer retval;

    if(start >= 0)
        retval = idn_mrLvarchar_instr(search_str, search_val, start, 1, Gen_fparam);
    else
        retval = idn_mrLvarchar_instr_back(search_str, search_val, start, 1, Gen_fparam);

    return retval;
}


UDREXPORT 
mi_integer idn_mrLvarchar_instr_4
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
        retval = idn_mrLvarchar_instr(search_str, search_val, start, 
            occurrence, Gen_fparam);
    else
        retval = idn_mrLvarchar_instr_back(search_str, search_val, start, 
            occurrence, Gen_fparam);

    return retval;
}

/************************** instrb *********************************************
**
** Function name:
**
**    idn_mrLvarchar_instrb   - workhorse routine called by idn_mrLvarchar_instrb_* functions
**    idn_mrLvarchar_instrb_2 - two arguments
**    idn_mrLvarchar_instrb_3 - three arguments
**    idn_mrLvarchar_instrb_4 - four arguments
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

static
mi_integer idn_mrLvarchar_instrb
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The Nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer position=0,  /* Position search_val was found at in search_str */
               size,
               count;       /* Loop counter for processing each occurrence */

    mi_string  *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL,
               *func="instrb";

    FUNC_TRACE_ENTER( func );

    /* C string library functions operate on NULL-terminated strings,
    ** so copy each input mi_lvarchar to a NULL-terminated string.
    */
    p1=mrLvarchar_to_char(search_str, &size);
    p2=mi_lvarchar_to_string(search_val);

    /* QA the input arguments. */
    qa_instr_args(func, size, start, occurrence, Gen_fparam);

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

    FUNC_TRACE_EXIT( func );

    return position;
}


/* idn_mrLvarchar_instrb_back
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


static
mi_integer idn_mrLvarchar_instrb_back
(
mi_lvarchar *search_str,  /* String we are searching through */
mi_lvarchar *search_val,  /* Value we are searching for */
mi_integer   start,       /* Where to begin searching */
mi_integer   occurrence,  /* The Nth occurrence for which to search */
MI_FPARAM   *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer retval=0,
               size,
               end_point,
               temp_position,
               num_position,    /* Size of integer positions found array */
               *position,       /* Positions found */
               *tmp_position,   /* For reallocating position array */
               count;       /* Total number of occurrences found */

    mi_string  msgbuf[80],
               *p=NULL,     /* Pointer that steps through p1 */
               *p1=NULL,    /* search_str converted to NULL-terminated string */
               *p2=NULL,    /* search_val converted to NULL-terminated string */
               *find_it=NULL,
               *func="instrb";

    FUNC_TRACE_ENTER( func );

    /* C string library functions operate on NULL-terminated strings,
    ** so copy each input mi_lvarchar to a NULL-terminated string.
    */
    p1=mrLvarchar_to_char(search_str, &size);
    p2=mi_lvarchar_to_string(search_val);

    /* QA the input arguments. */
    qa_instr_args(func, size, start, occurrence, Gen_fparam);

    num_position = MAX_P;
    position = (mi_integer *)mi_zalloc(sizeof(mi_integer) * num_position);
    if(position == (mi_integer)NULL)
    {
        sprintf(msgbuf, "%s: mi_alloc failed!", func);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
    }

    /* Calculate the end point so we don't start a search
    ** past this point.
    */
    end_point = size + start + 1;

    for (p=p1, count=0, temp_position=0; temp_position < end_point; count++)
    {
        if(count == num_position) /* int array not big enough */
        {
            GL_DPRINTF(TRACE_CLASS,
                TRACE_MEDIUM,
                (    "idn_mrLvarchar_ANY_TEXT",
                    "FUNCNAME%s",   func,
                    "MESSAGE%s",    "reallocating position array",
                    MI_LIST_END
                )
            );

            tmp_position = 
                 (mi_integer *)mi_zalloc(sizeof(mi_integer) * num_position * 2);
            if(tmp_position == (mi_integer)NULL)
            {
                sprintf(msgbuf, "%s: mi_alloc failed!", func);
                mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            }

            memcpy(tmp_position, position, num_position);

            mi_free(position);
            num_position *= 2;
            position = tmp_position;
        }

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
    mi_free(position); /* Free the integer array */

    FUNC_TRACE_EXIT( func );

    return retval;
}

UDREXPORT 
mi_integer idn_mrLvarchar_instrb_2
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
MI_FPARAM   *Gen_fparam 
)
{
    return idn_mrLvarchar_instrb(search_str, search_val, 1, 1, Gen_fparam);
}

UDREXPORT 
mi_integer idn_mrLvarchar_instrb_3
(
mi_lvarchar *search_str,
mi_lvarchar *search_val,
mi_integer   start,
MI_FPARAM   *Gen_fparam
)
{
    mi_integer retval;

    if(start >= 0)
        retval=idn_mrLvarchar_instrb(search_str, search_val, start, 1, Gen_fparam);
    else
        retval=idn_mrLvarchar_instrb_back(search_str, search_val, start, 1, Gen_fparam);

    return retval;
}

UDREXPORT 
mi_integer idn_mrLvarchar_instrb_4
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
        retval=idn_mrLvarchar_instrb(search_str, search_val, start, 
            occurrence, Gen_fparam);
    else
        retval=idn_mrLvarchar_instrb_back(search_str, search_val, start, 
            occurrence, Gen_fparam);

    return retval;
}
/* qa_instr_args
**
** Called by idn_mrLvarchar_instr and idn_mrLvarchar_instrb to QA the input arguments.
*/
static
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
    mi_string msgbuf[80];

    if(occurrence <= 0)
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
        sprintf(msgbuf,
            "%s: occurrence parameter is '%d', but must be > 0",
            func, occurrence);

        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* NOT REACHED */
    }

    /* Starting position is past the end of the string */
    if(start > search_len )
    {
        mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

        sprintf(msgbuf, 
             "%s, start parameter '%d' is > search string length of %d",
             func, start, search_len);

        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* NOT REACHED */
    }

    return;
}

