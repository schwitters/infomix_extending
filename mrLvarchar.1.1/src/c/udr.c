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
** Title:          udr.c
** Author:         jta@informix.com
** Created:        03/03/2000 12:58
** Description:    User-defined routines for the mrLvarchar DataBlade module.
**
**
** C entrypoints for SQL UDR's:
**
**  idn_mrLvarcharEqual              Returns TRUE if arg1 == arg2
**  idn_mrLvarcharNotEqual           Returns TRUE if arg1 <> arg2
**  idn_mrLvarcharGreaterThan        Returns TRUE if arg1 > arg2
**  idn_mrLvarcharGreaterThanOrEqual Returns TRUE if arg1 >= arg2
**  idn_mrLvarcharLessThan           Returns TRUE if arg1 < arg2
**  idn_mrLvarcharLessThanOrEqual    Returns TRUE if arg1 <= arg2
**
**  idn_mrLvarcharLength             Returns size of a UDT value.
**  idn_mrLvarcharIsLarge            Returns TRUE if a UDT value is too big 
**                                   for in-row storage.
**  idn_mrLvarcharIsSmall            Returns TRUE if a UDT value fits in-row.
**  idn_mrLvarcharRefcount           Returns the refcount for a UDT object.
**  idn_mrLvarcharSbspace            Returns the name of the sbspace in 
**                                   which a large value is stored.
**  idn_mrLvarcharClip               Returns the first 255 characters of object.
**  idn_mrLvarcharConcat             Concatenates two mrLvarchar objects.
**  idn_mrLvarcharConcat_iter        Iterator for ConcatAll aggregate.
**  mrLvarcharRelease                Returns the blade version and build date.
**  TraceSet_mrLvarchar              Turns on tracing.
*/

#include "mrLvarchar.h"

/* idn_mrLvarcharEqual
**
** Entrypoint for SQL Equal() UDR.
** Returns TRUE if the two objects passed in are identical.
*/

UDREXPORT
mi_boolean
idn_mrLvarcharEqual
(
mi_lvarchar    *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar    *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM      *Gen_fparam        /* UDR function parameter info */
)
{
    mrLvarchar *object1=NULL,    /* Pointer to input data arg1.    */
               *object2=NULL;    /* Pointer to input data arg2.  */
    mi_string  *r="idn_mrLvarcharEqual",
               msg_buf[80];
    mi_integer ret_val;

    FUNC_TRACE_ENTER( r );

    object1 = (mrLvarchar *)mi_get_vardata( Gen_param1 );
    object2 = (mrLvarchar *)mi_get_vardata( Gen_param2 );

    sprintf(msg_buf, "%s: arg1 size=%d, arg2 size=%d", 
        r, object1->hdr.size, object2->hdr.size);

    GL_DPRINTF(TRACE_CLASS,
                TRACE_MEDIUM,
                (    "idn_mrLvarchar_ANY_TEXT",
                    "FUNCNAME%s",   r,
                    "MESSAGE%s",    msg_buf,
                    MI_LIST_END
                )
            );

    /* If the objects aren't the same size, don't bother doing 
    ** a possibly expensive comparison.
    */
    if(object1->hdr.size != object2->hdr.size)
        ret_val = MI_FALSE;
    else
    {
        /* Call Compare to perform the comparison. */
        ret_val = (0 == idn_mrLvarcharCompare( Gen_param1,
            Gen_param2, Gen_fparam ));
    }

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}

/* idn_mrLvarcharNotEqual
**
** Entrypoint for SQL NotEqual() UDR.
** Returns TRUE if the two objects passed in are not identical.
*/

UDREXPORT
mi_boolean
idn_mrLvarcharNotEqual
(
mi_lvarchar     *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar     *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM       *Gen_fparam     /* UDR function parameter info */
)
{
    mrLvarchar  *object1=NULL,  /* Pointer to input data arg1.    */
                *object2=NULL;  /* Pointer to input data arg2.  */
    mi_string   *r="idn_mrLvarcharNotEqual";
    mi_integer  ret_val;

    FUNC_TRACE_ENTER( r );

    object1 = (mrLvarchar *)mi_get_vardata( Gen_param1 );
    object2 = (mrLvarchar *)mi_get_vardata( Gen_param2 );

    /* If the objects aren't the same size, they can't be equal
    ** so don't bother doing a possibly expensive comparison.
    */
    if(object1->hdr.size != object2->hdr.size)
        ret_val = MI_TRUE;
    else
    {
        /* Call Compare to perform the comparison. */
        ret_val = (0 != idn_mrLvarcharCompare( Gen_param1,
            Gen_param2, Gen_fparam ));
    }

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}

/*  idn_mrLvarcharGreaterThan - Returns TRUE if arg1 > arg2 */
UDREXPORT
mi_boolean
idn_mrLvarcharGreaterThan
(
mi_lvarchar     *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar     *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM       *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string   *r="idn_mrLvarcharGreaterThan";
    mi_integer  ret_val;

    FUNC_TRACE_ENTER( r );

    /* Call Compare to perform the comparison. */
    ret_val = (1 == idn_mrLvarcharCompare(Gen_param1, Gen_param2, Gen_fparam));

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}

/*  idn_mrLvarcharGreaterThanOrEqual - Returns TRUE if arg1 >= arg2
*/
UDREXPORT
mi_boolean
idn_mrLvarcharGreaterThanOrEqual
(
mi_lvarchar     *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar     *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM       *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string   *r="idn_mrLvarcharGreaterThanOrEqual";
    mi_integer  ret_val;

    FUNC_TRACE_ENTER( r );

    /* Call Compare to perform the comparison. */
    ret_val = (-1 < idn_mrLvarcharCompare(Gen_param1, Gen_param2, Gen_fparam));

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}

/*  idn_mrLvarcharLessThan - Returns TRUE if arg1 < arg2 */
UDREXPORT
mi_boolean
idn_mrLvarcharLessThan
(
mi_lvarchar     *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar     *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM       *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string   *r="idn_mrLvarcharLessThan";
    mi_integer  ret_val;

    FUNC_TRACE_ENTER( r );

    /* Call Compare to perform the comparison. */
    ret_val = (-1 == idn_mrLvarcharCompare(Gen_param1, Gen_param2, Gen_fparam));

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}


/*  idn_mrLvarcharLessThanOrEqual - Returns TRUE if arg1 <= arg2
*/
UDREXPORT
mi_boolean
idn_mrLvarcharLessThanOrEqual
(
mi_lvarchar     *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar     *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM       *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string   *r="idn_mrLvarcharLessThanOrEqual";
    mi_integer  ret_val;

    FUNC_TRACE_ENTER( r );

    /* Call Compare to perform the comparison. */
    ret_val = (1 > idn_mrLvarcharCompare(Gen_param1, Gen_param2, Gen_fparam));

    FUNC_TRACE_EXIT( r );

    return (mi_boolean)ret_val;
}

/* idn_mrLvarcharLength()
**
** Entrypoint for the SQL Length() UDR.
**
** Returns size of a UDT object.
*/

UDREXPORT
mi_integer
idn_mrLvarcharLength
(
mi_lvarchar      *mr_in,       /* UDT value in its internal C binary format */
MI_FPARAM        *Gen_fparam   /* UDR function parameter info */
)
{
    mi_integer   Gen_RetVal;   /* The return value.    */
    mrLvarchar   *object;
    mi_string    *r="idn_mrLvarcharLength";

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *) mi_get_vardata (mr_in);

    Gen_RetVal = object->hdr.size + sizeof(mrLvarcharHdr);

    FUNC_TRACE_EXIT( r );

    return Gen_RetVal;
}

/* idn_mrLvarcharIsLarge()
**
** Entrypoint for the SQL IsLarge() UDR.
**
** Returns TRUE if a UDT object is too big to fit in-row.
*/

UDREXPORT
mi_integer
idn_mrLvarcharIsLarge
(
mi_lvarchar    *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM      *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string  *r="idn_mrLvarcharIsLarge";
    mi_integer ret_val;
    mrLvarchar *object=NULL;

    FUNC_TRACE_ENTER(r);

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    if(object->hdr.storage == MR_InRow)
        ret_val=MI_FALSE;
    else
        ret_val=MI_TRUE;

    FUNC_TRACE_EXIT(r);

    return ret_val;
}

/* idn_mrLvarcharIsSmall()
**
** Entrypoint for the SQL IsSmall() UDR.
**
** Returns TRUE if a UDT object is small enough to fit in-row.
*/
UDREXPORT
mi_integer
idn_mrLvarcharIsSmall
(
mi_lvarchar    *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM      *Gen_fparam     /* UDR function parameter info */
)
{
    mi_string  *r="idn_mrLvarcharIsSmall";
    mi_integer ret_val;
    mrLvarchar *object=NULL;

    FUNC_TRACE_ENTER(r);

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    if(object->hdr.storage == MR_InRow)
        ret_val=MI_TRUE;
    else
        ret_val=MI_FALSE;

    FUNC_TRACE_EXIT(r);

    return ret_val;
}

/* idn_mrLvarcharRefcount()
**
** Entrypoint for the SQL Refcount() UDR.
**
** Returns the refcount for a UDT object. Returns NULL if the value
** is not stored in a smart blob.
*/

UDREXPORT
mi_integer
idn_mrLvarcharRefcount
(
mi_lvarchar       *Gen_param1,  /* UDT value in its internal C binary format */
MI_FPARAM         *Gen_fparam   /* UDR function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL; /* The current connection. */
    MI_LO_FD      slob_fd;       /* smart blob descriptor  */
    MI_LO_STAT    *lostat=NULL;  /* so we can get refcount */

    mrLvarchar    *object;

    mi_string     msgbuf[80],
                  *r="idn_mrLvarcharRefcount";

    mi_integer    ret_val,
                  status;

    FUNC_TRACE_ENTER(r);

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_TmpLo:
    case MR_Lo:

        Gen_Con = sblob_conn_get( r);

        if ((status = mi_lo_validate(Gen_Con, &object->u.lo)) != 0)
        {
            sprintf(msgbuf, "%s: lo handle not valid (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return MI_ERROR; /* not reached */
        }

        /* Open the smart blob */
        if ((slob_fd = mi_lo_open(Gen_Con, &object->u.lo, MI_LO_RDONLY)) 
            == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_open failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return MI_ERROR; /* not reached */   
        }

        /* Get the refcount */
        if ((status = mi_lo_stat(Gen_Con, slob_fd, &lostat)) != MI_OK)
        {
            sprintf(msgbuf, "%s: mi_lo_stat failed (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return MI_ERROR; /* not reached */   
        }

        if ((ret_val = mi_lo_stat_refcnt(lostat)) == MI_ERROR)
        {
            (void) mi_lo_stat_free(Gen_Con, lostat);
            (void) mi_lo_close(Gen_Con, slob_fd);
            sblob_conn_free(Gen_Con);

            sprintf(msgbuf, "%s: mi_lo_stat_refcnt failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);

            return MI_ERROR; /* not reached */   
        }

        /* Cleanup */
        (void)mi_lo_stat_free(Gen_Con, lostat);
        (void)mi_lo_close(Gen_Con, slob_fd);
        sblob_conn_free(Gen_Con);

        break;

    default: /* refcount is not applicable to this UDT object */
        ret_val = -1;
        break;
    }

    /* Return NULL if refcount not applicable to UDT object. */
    if (ret_val != -1)
        mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);

    FUNC_TRACE_EXIT(r);

    return ret_val;
}

/* idn_mrLvarcharSbspace()
**
** Entrypoint for the SQL Sbspace() UDR.
**
** Returns the name of the smart blob space in which a large value
** is stored. Returns NULL if the value is not stored in a smart
** blob.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarcharSbspace
(
mi_lvarchar       *Gen_param1,   /* UDT value in its internal C binary format */
MI_FPARAM         *Gen_fparam    /* UDR function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL; /* The current connection. */
    mrLvarchar    *object;
    mi_string     *r="idn_mrLvarcharSbspace";
    mi_string     space_name[129];    /* Max of 128 in 9.2 */
    mi_integer    is_a_slob;

    FUNC_TRACE_ENTER(r);

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    memset(space_name, '\0', sizeof(space_name));

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_TmpLo:
    case MR_Lo:
        is_a_slob=1;
        Gen_Con = sblob_conn_get( r );
        sbspace_name (Gen_Con, &object->u.lo, space_name, sizeof(space_name));
        sblob_conn_free(Gen_Con);
        break;

    default: /* Not a smart blob */
        is_a_slob=0;
        /* could make the following visible by not returning NULL */
        strcpy(space_name, "not a smart blob"); 
        break;
    }

    /* Enable the return value if the object passed in
    ** is stored in a smart blob.
    */
    if (is_a_slob == 1)
        mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);

    FUNC_TRACE_EXIT(r);

    return mi_string_to_lvarchar(space_name);
}

/* idn_mrLvarcharClip()
**
** Entrypoint for the SQL Clip() UDR.
**
** Returns the first 255 characters of a UDT object as a varchar(255).
*/

#define CLIP_SIZE 255

UDREXPORT
mi_lvarchar *
idn_mrLvarcharClip
(
mi_lvarchar       *Gen_param1,  /* UDT value in its internal C binary format */
MI_FPARAM         *Gen_fparam   /* UDR function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL; /* The current connection.     */
    mrLvarchar    *object=NULL;  /* Pointer to data in arg 1     */
    mi_string     msgbuf[80],
                  ret_val[CLIP_SIZE+1], /* gets converted to mi_lvarchar */
                  *r="idn_mrLvarcharClip";
    mi_char       *p=NULL;
    mi_integer    status;

    FUNC_TRACE_ENTER( r );

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_InRow:

        if(object->hdr.size <= CLIP_SIZE)
        {
            memcpy (ret_val, &object->u.data, object->hdr.size);
            ret_val[object->hdr.size]='\0';
        }
        else
        {
            memcpy (ret_val, &object->u.data, CLIP_SIZE);
            ret_val[CLIP_SIZE]='\0';
        }
        break;

    case MR_TmpLo:
    case MR_Lo:

        p = (mi_char *) mi_zalloc(CLIP_SIZE + 1);
        if(p == (mi_char *)NULL)
        {
            mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
                r, (mi_integer) NULL);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        Gen_Con = sblob_conn_get( r );

        /* A direct copy to the local ret_val variable 
        ** gets: "7420: Argument (buffer) is invalid."
        */
        status = mi_lo_to_buffer(Gen_Con, &object->u.lo, CLIP_SIZE, 
            (mi_char **) &p);
        if(status == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_to_buffer failed (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
        if(status != CLIP_SIZE)
        {
            sprintf(msgbuf, 
                "%s: mi_lo_to_buffer returned %d, but we specified %d bytes", 
                r, status, CLIP_SIZE);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        sblob_conn_free(Gen_Con);
        strncpy(ret_val, p, CLIP_SIZE);
        ret_val[CLIP_SIZE]='\0';
        mi_free(p);

        break;
    default:
        p=cache_get_ptr(object, r);
        memcpy (ret_val, p, CLIP_SIZE);
        ret_val[CLIP_SIZE]='\0';
        break;

    } /* END: copy arg 1 */

    FUNC_TRACE_EXIT( r );

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);
    return mi_string_to_lvarchar(ret_val);
}

/* idn_mrLvarcharConcat()
**
** Entrypoint for the SQL Concat() UDR.
**
** Concatenates two mrLvarchar objects.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarcharConcat
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar  *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* UDR function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL;        /* The current connection.      */
    mrLvarchar    *object1=NULL,        /* Pointer to data in arg 1     */
                  *object2=NULL;        /* Pointer to data in arg 2.    */
    mi_lvarchar   *Gen_RetVal=NULL; 
    mi_char       *temp_buf=NULL, *t_ptr=NULL;
    mi_integer    alloc_len,
                  status=0;
    mi_string     msgbuf[80],
                  *r="idn_mrLvarcharConcat";
    mi_char       *p=NULL;

    FUNC_TRACE_ENTER( r );

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    /* Point to the input data. */
    object1 = (mrLvarchar *)mi_get_vardata( Gen_param1 );
    object2 = (mrLvarchar *)mi_get_vardata( Gen_param2 );

    alloc_len = object1->hdr.size + object2->hdr.size;

    /* We'll build the concatenated data in a temp buffer,
    ** then call mrLvarchar_from_char() to convert it to an
    ** mrLvarchar object.
    **
    ** First, allocate room for the temp buffer.
    */
    temp_buf = (mi_char *)mi_zalloc(alloc_len);
    if( temp_buf == (mi_char *)NULL )
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
                r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }
    t_ptr=temp_buf;

    /* ============= #1: Copy in the data from the first argument ========== */
    switch(object1->hdr.storage)
    {
    case MR_InRow:
        /* The actual value is stored in the data portion */
        memcpy (t_ptr, &object1->u.data, object1->hdr.size);
        break;

    case MR_TmpLo:
    case MR_Lo:
        /* copy from the smart blob */
        Gen_Con = sblob_conn_get( r );

        if ((status = mi_lo_validate(Gen_Con, &object1->u.lo)) != 0)
        {
            sprintf(msgbuf, "%s: lo handle not valid (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        status = mi_lo_to_buffer(Gen_Con, &object1->u.lo, 
            object1->hdr.size, (mi_char **) &temp_buf);
        if(status == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_to_buffer failed on arg 1!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
        if(status != object1->hdr.size)
        {
            sprintf(msgbuf, 
                "%s: mi_lo_to_buffer returned %d, but arg 1 had %d bytes", 
                r, status, object1->hdr.size);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        sblob_conn_free(Gen_Con);
        break;

    default:
        /* The rest should be various durations for cached memory
        ** (storage_check_flag() verifies).
        */
        if((storage_flag_check(object1->hdr.storage)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: unhandled storage type '%d'",
                r, object1->hdr.storage);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        p=cache_get_ptr(object1, r);
        memcpy (t_ptr, p, object1->hdr.size);
        p=NULL;
        break;
    } /* END: copy arg 1 */

    /* Step past the data copied for the first argument */
    t_ptr += object1->hdr.size;

    /* ============= #2: Copy in the data from the second argument ======= */
    switch(object2->hdr.storage)
    {
    case MR_InRow:
        memcpy (t_ptr, &object2->u.data, object2->hdr.size);
        break;

    case MR_TmpLo:
    case MR_Lo:
        /* We can't do a mi_lo_to_buffer into the middle of 
        ** the target, and mi_lo_to_string isn't safe if there
        ** are any NULL's in the data.
        **
        ** So we'll allocate a temp buffer, do an mi_lo_to_buffer 
        ** into that, copy it, then free the temp buffer.
        */
        p = (mi_char *) mi_zalloc(object2->hdr.size + 1);
        if(p == (mi_char *)NULL)
        {
            mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
                r, (mi_integer) NULL);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        /* copy from smart blob into buffer 'p' */
        Gen_Con = sblob_conn_get( r);

        if ((status = mi_lo_validate(Gen_Con, &object2->u.lo)) != 0)
        {
            sprintf(msgbuf, "%s: lo handle not valid (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        status = mi_lo_to_buffer(Gen_Con, &object2->u.lo, 
            object2->hdr.size, (mi_char **) &p);
        if(status == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_to_buffer failed on arg 2 (%d)!", 
                r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
        if(status != object2->hdr.size)
        {
            sprintf(msgbuf, 
                "%s: mi_lo_to_buffer returned %d, but arg 2 had %d bytes", 
                r, status, object2->hdr.size);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        sblob_conn_free(Gen_Con);

        memcpy (t_ptr, p, object2->hdr.size);

        mi_free(p);
        p=NULL;
        break;

    default:
        /* The rest should be various durations for cached memory
        ** (storage_check_flag() verifies).
        */
        if((storage_flag_check(object2->hdr.storage)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: unhandled storage type '%d'",
                r, object2->hdr.storage);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        p=cache_get_ptr(object2, r);
        memcpy (t_ptr, p, object2->hdr.size);
        break;
    } /* END copy arg 2 */

    /* ====== #3: now convert it to our UDT by calling Input ======= */
    Gen_RetVal = mrLvarchar_from_char(temp_buf, alloc_len, Gen_fparam);

    /* Free the temporary buffer */
    mi_free(temp_buf);
    temp_buf=NULL;

    FUNC_TRACE_EXIT( r );

    /* Return the UDT value. */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);
    return Gen_RetVal;
}

/* idn_mrLvarcharConcat_iter()
**
**    Iterator routine for the ConcatAll aggregate.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarcharConcat_iter
(
mi_lvarchar  *previous,    /* UDT value in its internal C binary format */
mi_lvarchar  *next,        /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam   /* UDR function parameter info */
)
{
    mi_lvarchar *retval=NULL;
    mi_string   *r="idn_mrLvarcharConcat_iter";

    FUNC_TRACE_ENTER( r );

    /* "previous" will be NULL the first time */
    if( mi_fp_argisnull (Gen_fparam, 0) == MI_TRUE)
    {
        retval=next;
    }

    /* The "next" column value might also be NULL */
    else if( mi_fp_argisnull (Gen_fparam, 1) == MI_TRUE)
    {
        retval=previous;
    }
    else
    {
        retval = idn_mrLvarcharConcat(previous, next, Gen_fparam);

        /* "previous" is no longer needed, so free it */
        idn_mrLvarcharTempFree(previous, Gen_fparam);
    }

    FUNC_TRACE_EXIT( r );

    return retval;
}


/* mrLvarcharRelease()
**
** Entrypoint for the mrLvarcharRelease() UDR.
**
** Returns the blade version and build date.
*/
UDREXPORT
mi_lvarchar *
mrLvarcharRelease ()
{
    mi_lvarchar *ret=NULL;
    mi_string   ret_buf[100],
                *r="mrLvarcharRelease";

    FUNC_TRACE_ENTER( r );

    sprintf(ret_buf, "%s (%s)", BUILD_VER, BUILD_DATE);

    ret = mi_string_to_lvarchar(ret_buf);
    if(ret == (mi_lvarchar *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
            r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL;
    }

    FUNC_TRACE_EXIT( r );

    return ret;
}

/* TraceSet_mrLvarchar()
**
** Entrypoint for the TraceSet_mrLvarchar(lvarchar,int) UDR.
**
** Sets the tracefile name and sets the trace level for the
** mrLvarchar trace class.
*/
UDREXPORT
void TraceSet_mrLvarchar
(
mi_lvarchar  *Gen_param,     /* Trace filename */
mi_integer   Gen_TraceLevel, /* Trace level */
MI_FPARAM    *Gen_fparam     /* UDR function parameter info */
)
{
    gl_mchar_t  *Gen_TraceFile;     /* Trace file name.    */
    char        Gen_TraceStr[50];   /* Tracing level.    */ 
    mi_string   *r="TraceSet_mrLvarchar";

    FUNC_TRACE_ENTER( r );

    /*
    ** Set the name and location  of the  output
    ** trace file. If not set, the name defaults
    ** to <session id>.trc in the /tmp directory.
    */
    if( mi_get_varlen( Gen_param ) != 0 )
    {
        /* Convert input arg to a NULL-terminated input file name. */
        Gen_TraceFile = (gl_mchar_t *)mi_lvarchar_to_string( Gen_param );

        /* Set the name of the trace file. */
        if( mi_tracefile_set( (char *)Gen_TraceFile ) == -1 )
        {
            /* Raise the dbdk error in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENI", "FUNCTION%s", 
                r, (mi_integer) NULL);

            return; /* not reached */
        }

        /* Free the allocated memory. */
        (void) mi_free( Gen_TraceFile );
    }

    /*
    ** Set the tracing level. The default tracing level is 0.
    */
    if( Gen_TraceLevel >= 0 )
    {
        sprintf( Gen_TraceStr, "%s %d", TRACE_CLASS, Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            /* Raise the error that dbdk stores in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENJ", "FUNCTION%s", 
                r, (mi_integer) NULL);

            return; /* not reached */
        }

        /* Also turn on the __myErrors__ trace class */
        sprintf( Gen_TraceStr, "__myErrors__ %d", Gen_TraceLevel );
        if( mi_tracelevel_set( Gen_TraceStr ) == -1 )
        {
            /* Raise the error that dbdk stores in syserrors */
            mi_db_error_raise(NULL, MI_SQL, "UGENJ", "FUNCTION%s", 
                r, (mi_integer) NULL);

            return; /* not reached */
        }
    }

    /* The first call initializes GLS messages */
    FUNC_TRACE_EXIT( r );
}

