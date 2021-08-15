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
** Title:          mrLvarchar.c
** Author:         jta@informix.com
** Created:        03/03/2000 12:58
** Description:    Opaque type support routines for the idn_mrLvarchar type.
**
** Functions:
** ---------
** idn_mrLvarcharInput        Converts external text to internal mrLvarchar.
** idn_mrLvarcharOutput       Converts internal mrLvarchar to external text.
** idn_mrLvarcharSend         Converts internal mrLvarchar to external binary.
** idn_mrLvarcharReceive      Converts external binary to internal mrLvarchar.
** idn_mrLvarcharImportText   Bulk loads from ascii data.
** idn_mrLvarcharExportText   Bulk unloads to ascii data.
** idn_mrLvarcharImportBinary Bulk load from binary data.
** idn_mrLvarcharExportBinary Bulk unloads to binary data.
** idn_mrLvarcharAssign       Creates sblob, if needed, and increments refcount.
** idn_mrLvarcharDestroy      Decrements sblob refcount.
** idn_mrLvarcharLOhandles    Returns sblob handles.
** idn_mrLvarcharDeepCopy     Copies the UDT value.
** idn_mrLvarcharTempAssign   Flushes in-memory data to temporary sblob.
** idn_mrLvarcharTempFree     Frees a temporary out-of-row (large) value.
** idn_mrLvarcharCompare      Supports ORDER BY, DISTINCT, and UNION queries.
*/

#include "mrLvarchar.h"

/* idn_mrLvarcharInput()
**
**    Entrypoint for the SQL idn_mrLvarcharIn() UDR.
**
**    The server calls this function to convert the external text representation
**    into the internal mrLvarchar C structure. Since the UDT is variable-length,
**    the mrLvarchar structure gets stored in the data portion of an mi_lvarchar.
*/

UDREXPORT
mi_lvarchar *              /* UDT in its internal, binary format */
idn_mrLvarcharInput
(
mi_lvarchar  *Gen_param1,  /* The UDT value in its external, text format */
MI_FPARAM    *Gen_fparam   /* Function parameter info */
)
{
    mi_lvarchar  *Gen_RetVal=NULL;
    mi_string    *r="idn_mrLvarcharInput",
                 msgbuf[80];

    FUNC_TRACE_ENTER(r);

    /* Initialize the return value to NULL. It gets set to not NULL at
    ** the end of this function when there is a valid value to return.
    */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    /* CURRENT LIMITATION: no GLS support (no codeset conversion) yet.  */

    /* Call the workhorse routine that creates an mrLvarchar */
    Gen_RetVal = mrLvarchar_from_char (mi_get_vardata(Gen_param1), 
        mi_get_varlen(Gen_param1), Gen_fparam);

    if(Gen_RetVal == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_from_char failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    FUNC_TRACE_EXIT(r);

    /* Return the UDT value. */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);
    return Gen_RetVal;
}

/* idn_mrLvarcharOutput()
**
**    Entrypoint for the SQL idn_mrLvarcharOut() UDR.
**
**    The server calls this function to convert the internal mrLvarchar
**    value into its external text representation.
*/

UDREXPORT
mi_lvarchar *                /* UDT value in its external text format */
idn_mrLvarcharOutput
(
mi_lvarchar  *Gen_param1,    /* The UDT value in its internal binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    mi_lvarchar  *Gen_RetVal=NULL;    /* Return value */
    char         *Gen_OutData=NULL;   /* Pointer to the output data.    */
    mi_string    *r="idn_mrLvarcharOutput";
    mi_integer   data_len;

    FUNC_TRACE_ENTER( r );

    /* Initialize the return value to NULL. It gets set to not NULL at
    ** the end of this function when there is a valid value to return.
    */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    /* Allocate space for the mi_lvarchar return value, 
    ** but don't allocate storage for the data itself.
    */
    Gen_RetVal = mi_new_var(0);
    if( Gen_RetVal == (mi_lvarchar *)NULL )
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL;/* not reached */
    }

    /* Allocate storage for the data, and get a fully
    ** expanded copy of the data.
    */
    Gen_OutData = mrLvarchar_to_char (Gen_param1, &data_len);

    /* ===========<<< Codeset conversion goes here. >>>=========== */

    /* Store the pointer to the data in the return value */
    mi_set_varptr(Gen_RetVal, Gen_OutData);

    /* Tell the server how large that data is. */
    mi_set_varlen(Gen_RetVal, data_len);

    FUNC_TRACE_EXIT( r );

    /* Return the UDT value. */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);
    return Gen_RetVal;
}

/* idn_mrLvarcharSend()
**
**    Entrypoint for the SQL idn_mrLvarcharSend() UDR.
**
**    The server calls this function to convert the internal mrLvarchar
**    value into an external binary value. Client applications that fetch
**    data in binary mode will cause this function to be called.
*/
UDREXPORT
mi_sendrecv *
idn_mrLvarcharSend
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    /* The Send function packages up the UDT value in an mi_lvarchar
    ** data container (mi_sendrecv is an mi_lvarchar, under the hood).
    **
    ** This implementation deliberately fully expands the data--an 
    ** alternative approach send the smart blob handle, but it the
    ** client tries to insert it into a database in another server,
    ** the smart blob handle won't be valid for that other server.
    ** 
    ** Since this is text data, it can just call the opaque type
    ** output routine. Send routines are usually more complicated
    ** than this.
    */
    return (mi_sendrecv *) idn_mrLvarcharOutput((mi_lvarchar *)Gen_param1,
        Gen_fparam);
}

/* idn_mrLvarcharReceive()
**
**    Entrypoint for the SQL idn_mrLvarcharRecv() UDR.
**
**    The server calls this function to convert an external binary
**    value into the internal mrLvarchar. Client applications that
**    update or insert data  in binary mode will cause this function 
**    to be called.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharReceive
(
mi_sendrecv  *Gen_param1,    /* UDT value in its external binary format */
MI_FPARAM    *Gen_fparam        /* Function parameter info */
)
{
    /* The Receive function unpackages an idn_mrLvarchar UDT
    ** from the input mi_lvarchar (mi_sendrecv is an mi_lvarchar,
    ** under the hood) and return the opaque type.
    ** 
    ** This implementation deliberately fully expands the data
    ** in external binary format, so we just call the Input routine. 
    */
    return (mi_lvarchar *) idn_mrLvarcharInput((mi_lvarchar *)Gen_param1,
        Gen_fparam);
}

/* idn_mrLvarcharImportText
**
** Entrypoint for SQL idn_mrLvarcharImpT() UDR.
**
**    The server calls this function when the client executes the
**    LOAD command.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharImportText
(
mi_lvarchar  *Gen_param1,    /* UDT value in its external, ascii format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    /* This routine does the same thing as the Input routine.
    ** DBDK generates a separate import routine. You could also
    ** just point the external name for the SQL routine directly
    ** at the Input routine.
    */
    return idn_mrLvarcharInput (Gen_param1, Gen_fparam );
}


/* idn_mrLvarcharExportText
**
** Entrypoint for SQL idn_mrLvarcharExpT() UDR.
**
**    The server calls this function when the client executes the
**    UNLOAD command.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharExportText
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    /* This routine does the same thing as the Output routine.
    ** DBDK generates a separate export routine. You could also
    ** just point the external name for the SQL routine directly
    ** at the Output routine.
    */
    return ( idn_mrLvarcharOutput (Gen_param1, Gen_fparam) );
}

/* idn_mrLvarcharImportBinary
**
** Entrypoint for SQL idn_mrLvarcharImpB() UDR.
**
**    The server calls this function when the client executes the
**    LOAD command in binary mode.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarcharImportBinary
(
mi_lvarchar  *Gen_param1,    /* UDT value in external binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    /* This routine does the same thing as the Receive routine.
    ** DBDK generates a separate import routine. You could also
    ** just point the external name for the SQL routine directly
    ** at the Receive routine.
    */
    return idn_mrLvarcharReceive ((mi_sendrecv *)Gen_param1, Gen_fparam);

}

/* idn_mrLvarcharExportBinary
**
** Entrypoint for SQL idn_mrLvarcharExpB() UDR.
**
**    The server calls this function when the client executes the
**    UNLOAD command in binary mode.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharExportBinary
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    /* This routine does the same thing as the Send routine.
    ** DBDK generates a separate export routine. You could also
    ** just point the external name for the SQL routine directly
    ** at the Send routine.
    */
    return (mi_lvarchar *)idn_mrLvarcharSend (Gen_param1, Gen_fparam);
}


/* idn_mrLvarcharAssign()
**
**    Entrypoint for the Assign() UDR.
**
**    The server calls this function just before a value is written to disk.
**
**    Nothing happens for a value that is small enough to fit in-row.
**    The value gets returned as is.
**
**    If an out-of-row value is cached in memory, this function writes the
**    value to a smart blob and increments the refcount for the smart blob
**    so it will persist.
**
**    If an out-of-row value is stored in a smart blob, this function copies 
**    it to the correct smart blob space if it requires relocation. Finally, 
**    the smart blob's refcount is incremented.
*/

UDREXPORT
mi_lvarchar *
idn_mrLvarcharAssign
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    MI_CONNECTION   *Gen_Con=NULL;    /* The current connection. */
    MI_LO_HANDLE    *slob_p=NULL;
    mrLvarchar      *object;
    mi_char         *p;
    mi_string       msgbuf[80],
                    sbspace1[129],    /* space name in 9.2 is 128 character */
                    sbspace2[129],
                    *trace_msg=NULL,
                    *r="idn_mrLvarcharAssign";
    mi_integer      status;

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_InRow:        /* Nothing needs to be done */
        break;

    case MR_TmpLo:        /* Create a new, permanent smart blob. */

        Gen_Con = sblob_conn_get(r);

        slob_p = sblob_from_tmpsblob( Gen_Con, &object->u.lo, Gen_fparam);

        /* Bump the refcount */
        if ((status = mi_lo_increfcount(Gen_Con, slob_p)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_increfcount() failed (%d)!",
                r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        sblob_conn_free (Gen_Con);

        /* Save the sblob handle, reset the storage flag */
        memcpy ( &object->u.lo, slob_p, sizeof(MI_LO_HANDLE) );
        object->hdr.storage=MR_Lo;

        break;

    case MR_Lo:        /* Already in a permanent sblob -- check it. */
        Gen_Con = sblob_conn_get(r);

        /* Is it in the correct smart blob space? or does it need 
        ** to be relocated to a different smart blob space?
        */
        sbspace_name (Gen_Con, &object->u.lo, sbspace1, sizeof(sbspace1));
        sbspace_name_column (Gen_Con, sbspace2, sizeof(sbspace2), Gen_fparam);

        if ( tf(TRACE_CLASS, TRACE_MEDIUM) ) /* Expensive trace message! */
        {
            trace_msg = mi_zalloc( sizeof(sbspace1) + sizeof(sbspace2) +50 );
            if( trace_msg == (mi_string *)NULL )
            {
               mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                    r, (mi_integer) NULL);
                return (mi_lvarchar *)NULL; /* not reached */
            }
            sprintf(trace_msg, "sbspace1 = '%s', sbspace2 = '%s'", 
                sbspace1, sbspace2);

            GL_DPRINTF(TRACE_CLASS,
                TRACE_MEDIUM,
                (    "idn_mrLvarchar_ANY_TEXT",
                    "FUNCNAME%s",   r,
                    "MESSAGE%s",    trace_msg,
                    MI_LIST_END
                )
            );
            (void) mi_free(trace_msg);
        } /* END trace block */

        /* If sbspace2 is not set, then the table create did not specify
        ** a smart blob space and we do not need to do anything.
        **
        ** We only need to relocate if sbspace2 is set and it does
        ** not match sbspace1.
        */
        if (strlen(sbspace2) > 0
            && strcmp(sbspace1, sbspace2) != 0
            )
        {
            slob_p = sblob_from_tmpsblob( Gen_Con, &object->u.lo, Gen_fparam);
            memcpy ( &object->u.lo, slob_p, sizeof(MI_LO_HANDLE) );
        }

        /* Bump the refcount */
        if ((status = mi_lo_increfcount(Gen_Con, &object->u.lo)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_increfcount() failed (%d) !", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL;    /* not reached */
        }

        sblob_conn_free(Gen_Con);

        break;

    default:

        /* The rest should be various durations for cached memory
        ** (storage_check_flag() verifies). We need to copy the
        ** contents of the in-memory cache to a permanent smart blob.
        */

        if((storage_flag_check(object->hdr.storage)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: unhandled storage type '%d'",
                r, object->hdr.storage);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        Gen_Con = sblob_conn_get( r );

        /* Get the pointer to the data cached in-memory,
        ** verify the data is still good, then create and
        ** populate a smart blob.
        */
        p=cache_get_ptr(object,r);

        slob_p = sblob_from_char(Gen_Con, p, object->hdr.size,
            LO_ASSIGN, Gen_fparam);

        /* Bump the refcount. */
        if ((status = mi_lo_increfcount(Gen_Con, slob_p)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_increfcount() failed (%d)!",
                r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }
                
        sblob_conn_free(Gen_Con);

        /* Copy the handle (the original UDT allocation was large
        ** enough for an MI_LO_HANDLE) and reset the storage flag.
        */
        memcpy ( &object->u.lo, (mi_char *) slob_p, sizeof(MI_LO_HANDLE) );
        object->hdr.storage=MR_Lo;

        /* Finally, free the in-memory buffer */
        cache_free(object);
        break;
    }

    if ( tf(TRACE_CLASS, TRACE_MEDIUM) )
    {
        if(object->hdr.storage == MR_InRow)
        {
            sprintf(msgbuf, "%s: Assigned value is stored in-row.", r);
        }
        else
        {
            sprintf(msgbuf,
                "Assigned value is stored in sblob.");
        }

        GL_DPRINTF(TRACE_CLASS,
            TRACE_MEDIUM,
            (   "idn_mrLvarchar_ANY_TEXT",
            "FUNCNAME%s",   r,
            "MESSAGE%s",    msgbuf,
            MI_LIST_END
            )
        );
    } /* END trace block */

    FUNC_TRACE_EXIT( r );

    return Gen_param1;
}

/* idn_mrLvarcharDestroy()
**
** Entrypoint for the Destroy() UDR.
**
** The server calls this function just before a value is deleted
** from disk.
**
** Nothing happens for a value that is small enough to fit in-row.
**
** If the value is large, the smart blob's refcount is decremented.
*/
UDREXPORT
void
idn_mrLvarcharDestroy
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL;    /* The current connection. */
    mrLvarchar    *object;
    mi_string     msgbuf[80],
                  *r="idn_mrLvarcharDestroy";
    mi_integer    status;

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *) mi_get_vardata(Gen_param1);

    switch(object->hdr.storage)
    {
    case MR_TmpLo:
    case MR_Lo:
        /* Decrement refcount. */
        if ( tf(TRACE_CLASS, TRACE_MEDIUM) )
        {
            sprintf(msgbuf, "decrementing refcount for sblob");

            GL_DPRINTF(TRACE_CLASS,
                TRACE_MEDIUM,
                (   "idn_mrLvarchar_ANY_TEXT",
                    "FUNCNAME%s",   r,
                    "MESSAGE%s",    msgbuf,
                    MI_LIST_END
                )
            );                           

        } /* END trace block */

        Gen_Con = sblob_conn_get( r );

        object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

        if ((status = mi_lo_validate(Gen_Con, &object->u.lo)) != 0)
        {
            sprintf(msgbuf, "%s: lo handle not valid (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return; /* not reached */
        }
        if ((status = mi_lo_decrefcount(Gen_Con, &object->u.lo)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_decrefcount() failed (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return; /* not reached */
        }

        /* Cleanup */
        sblob_conn_free(Gen_Con);
        break;
    default:
        break;
    }

    FUNC_TRACE_EXIT( r );

    return;
}

/* idn_mrLvarcharLOhandles
**
** Entrypoint for the SQL LOhandles UDR.
**
** Returns the smart blob handle for each smart blob in the UDT.
** Returns 0 if the value is not stored in a smart blob.
*/

/* This data structure is returned by LOhandles, wrapped in an lvarchar */
typedef struct
{
    mi_integer    nlos;    /* Number of large object handles */
    MI_LO_HANDLE  los[1];  /* Valid large object handles     */
} lo_list_struct;

UDREXPORT
mi_lvarchar *
idn_mrLvarcharLOhandles
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    mrLvarchar     *object;        /* The UDT value.          */
    mi_lvarchar    *retval=NULL;
    lo_list_struct *lo_list;
    mi_string      *r="idn_mrLvarcharLOhandles";

    FUNC_TRACE_ENTER( r );

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata( (mi_lvarchar *)Gen_param1 );

    /* Allocate an lvarchar big enough to hold an lo_list structure */
    retval = (mi_lvarchar *) mi_new_var ( sizeof(lo_list_struct) );
    if( retval == (mi_lvarchar *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    lo_list = (lo_list_struct *) mi_get_vardata ( (mi_lvarchar *) retval );

    switch(object->hdr.storage)
    {
    case MR_TmpLo:
    case MR_Lo:

        /* Fill in the lo_list data structure */
        lo_list->nlos = 1;
        memcpy ( &lo_list->los[0], &object->u.lo, sizeof(MI_LO_HANDLE) );
        break;

    default:

        /* set nlos in the lo_list to 0 */
        lo_list->nlos = 0;
        memset ( &lo_list->los[0], 0, sizeof(MI_LO_HANDLE) );
        break;
    }

    FUNC_TRACE_EXIT( r );

    return retval;
}


/* idn_mrLvarcharDeepCopy()
**
**    Entrypoint for SQL DeepCopy() UDR.
**
**    The 9.3 server will call this routine when it needs to copy a UDT value.
**    Pre-9.3 servers do not call this routine.
**    For more information, see this IDN tech note:
**
**    http://www.informix.com/idn-secure/DataBlade/Library/multirep.htm
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharDeepCopy
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    mrLvarchar  *object1=NULL,
                *object2=NULL;
    mi_char     *cache=NULL;
    mi_string   msgbuf[80],
                *r="idn_mrLvarcharDeepCopy";
    mi_lvarchar *ret_val=NULL;

    FUNC_TRACE_ENTER( r );

    /* Copy the UDT value. 
    **
    ** This first step just copies the primary mi_lvarchar, 
    ** which works for in-row values and out-of-row smart 
    ** blob handles.
    **
    ** If an out-of-row value is stored in separately allocated 
    ** memory, we copy that separately.
    */
    ret_val = mi_var_copy(Gen_param1);
    if(ret_val == (mi_lvarchar *)NULL)
    {
        sprintf(msgbuf, "%s: mi_var_copy failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    object1 = (mrLvarchar *)mi_get_vardata(Gen_param1);
    object2 = (mrLvarchar *)mi_get_vardata(ret_val);

    switch(object1->hdr.storage)
    {
    case MR_InRow: /* mi_var_copy already copied it */
    case MR_TmpLo:    
    case MR_Lo:
        break;
    default:
        cache = cache_copy(object1, r);
        object2->u.mem = cache;         /* Store the pointer */
        break;
    }

    FUNC_TRACE_EXIT( r );

    return ret_val;
}

/* idn_mrLvarcharTempAssign
**
** Entrypoint for SQL TempAssign() UDR. Does the same thing as Assign()
** but does not bump the refcount.
**
** No server calls this routine. End-users can use it to flush cached
** data to smart blobs, saving memory at the expense of disk. Also,
** at some point, DeepCopy might be recoded to call this routine when
** memory consumption has reached some limit.
*/
UDREXPORT
mi_lvarchar *
idn_mrLvarcharTempAssign
(
mi_lvarchar  *Gen_param1,
MI_FPARAM    *Gen_fparam        /* Function parameter info */
)
{
    MI_CONNECTION *Gen_Con=NULL;    /* The current connection. */
    MI_LO_HANDLE  *slob_p=NULL;
    mrLvarchar    *object;
    mi_char       *p;
    mi_string     *r="idn_mrLvarcharTempAssign";

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_InRow:  /* Nothing needs to be done. */
    case MR_TmpLo:  /* It's already a smart blob. */
    case MR_Lo:    
        break;

    default:

        /* The rest should be various durations for cached memory.
        ** Get the pointer to the data cached in-memory, then copy
        ** the contents into a temporary smart blob.
        */

        p=cache_get_ptr(object,r);

        Gen_Con = sblob_conn_get( r );

        slob_p = sblob_from_char(Gen_Con, p, object->hdr.size,
            MR_TmpLo, Gen_fparam);
            
        sblob_conn_free(Gen_Con);

        /* Copy the handle (original UDT allocation was large
        ** enough for an MI_LO_HANDLE) and reset the storage flag.
        */
        memcpy ( &object->u.lo, (mi_char *) slob_p, sizeof(MI_LO_HANDLE) );
        object->hdr.storage=MR_TmpLo;

        /* Free the in-memory buffer */
        cache_free(object);
        break;
    }

    FUNC_TRACE_EXIT( r );

    return Gen_param1;
}

/* idn_mrLvarcharTempFree
**
**    Free's a value that is known to be a temporary value. Not
**    attached to a UDR in the first release, but the symbol is
**    exported for NT.
**
**    About the only context that can know this is an aggregate iter
**    function that is passed the previous and next result. After
**    computing the return value, it is safe to free the previous
**    result (but never the "next" value that was passed in).
**
**    This routine only frees an out-of-row value stored in memory
**    or in a temporary smart blob.
**
**    It does not free an in-row value and does not free a permanent
**    smart blob. For the values it does free, it only frees the
**    data value, and does not free the mi_lvarchar for the UDT.
*/
UDREXPORT
void
idn_mrLvarcharTempFree
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    MI_CONNECTION  *Gen_Con=NULL;     /* The current connection. */
    mrLvarchar     *object;
    mi_string      msgbuf[80],
                   *r="idn_mrLvarcharTempFree";
    mi_integer     status;

    FUNC_TRACE_ENTER( r );

    object = (mrLvarchar *)mi_get_vardata( Gen_param1 );

    switch(object->hdr.storage)
    {
    case MR_InRow:    /* Don't do anything */
    case MR_Lo:       /* Don't do anything */
        break;

    case MR_TmpLo:    /* It's in a temporary smart blob */

        Gen_Con = sblob_conn_get( r );
        status = mi_lo_delete_immediate(Gen_Con, &object->u.lo);
        if(status != MI_OK)
        {
            sprintf(msgbuf, "%s: mi_lo_delete_immediate() failed (%d)!",
                r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return ; /* not reached */
        }
        sblob_conn_free (Gen_Con);

        object->hdr.storage = MR_Freed;
        object->hdr.size = 0;

        break;

    default:        /* It's in a temporary memory cache */
        /* Free the in-memory buffer.
        **
        ** If the memory duration was not high enough, the memory may
        ** have already been free'd, but not overwritten yet. So it
        ** it looks like it there, but isn't really and the mi_free
        ** can cause an assertion failure.
        **
        ** The "Memory Allocation for C UDRs" tech note on IDN
        ** suggests debug flags you can turn on to catch this sort
        ** of problem:
        **
        **    http:/www.informix.com/idn
        */
        cache_free(object);

        object->hdr.storage = MR_Freed;
        object->hdr.size = 0;

        break;
    }

    FUNC_TRACE_EXIT( r );

    return;
}


/* idn_mrLvarcharCompare()
**
** Entrypoint for SQL Compare() UDR.
**
** The server calls the compare() routine to process "order by",
** "distinct/unique", and "union" queries.
*/

UDREXPORT
mi_integer
idn_mrLvarcharCompare
(
mi_lvarchar  *Gen_param1,    /* UDT value in its internal C binary format */
mi_lvarchar  *Gen_param2,    /* UDT value in its internal C binary format */
MI_FPARAM    *Gen_fparam     /* Function parameter info */
)
{
    gl_mchar_t *p1=NULL,    /* Pointer to the data in arg 1 */
               *p2=NULL;    /* Pointer to the data in arg 2 */
    mi_string  msgbuf[80],
               *r="idn_mrLvarcharCompare";
    mi_integer arg1_size,    /* Size of data in arg 1 */
               arg2_size,    /* Size of data in arg 2 */
               arg1_alloc,    /* If p1 is allocated, this set to 1 */
               arg2_alloc,    /* If p2 is allocated, this set to 1 */
               status,
               gl_errno,
               ret_val=0;

    /*
    ** This routine must return the sort order for the two input arguments:
    **
    **   -1 if arg1 < arg2 (arg1 comes before arg2)
    **    0 if arg1 == arg2
    **    1 if arg1 > arg2 (arg1 comes after arg2)    
    */

    FUNC_TRACE_ENTER( r );

    mi_fp_setreturnisnull(Gen_fparam, 0, MI_TRUE);

    /* This code does not call mrLvarchar_to_char() for each argument
    ** because that function always allocates the return result.
    **
    ** This function does extra work (and code) so that it only allocates
    ** temp storage for large values stored in a smart large object. But 
    ** the code is more cluttered.
    */

    /* ===== Argument 1: Get the data in the first object ===== */
    p1 = (gl_mchar_t *)mrLvarchar_to_char_opt (Gen_param1, &arg1_size, &arg1_alloc);
    if(p1 ==(gl_mchar_t *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char_opt failed for arg1", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return 0; /* not reached */
    }

    /* ===== Argument 2: Get the data in the second object ===== */
    p2 = (gl_mchar_t *)mrLvarchar_to_char_opt (Gen_param2, &arg2_size, &arg2_alloc);
    if(p2 ==(gl_mchar_t *)NULL)
    {
        sprintf(msgbuf, "%s: mrLvarchar_to_char_opt failed for arg2", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return 0; /* not reached */
    }

    /* Perform the comparison using the GLS comparison routine */
    ifx_gl_lc_errno() = 0;
    status = ifx_gl_mbscoll (p1, arg1_size, p2, arg2_size);
    if ( (gl_errno=ifx_gl_lc_errno()) != 0)
        raise_gl_error(r, "ifx_gl_mbscoll", gl_errno, Gen_fparam);

    if (status > 0)
        ret_val = 1;
    else if (status < 0)
        ret_val = -1;
    else
        ret_val = 0;

    /* Cleanup */
    if(arg1_alloc == 1)
        mi_free(p1);

    if(arg2_alloc == 1)
        mi_free(p2);

    /*
    ** Write to the trace file indicating
    ** that mrLvarcharOutput has successfully exited.
    */
    FUNC_TRACE_EXIT( r );

    /* Return the value. */
    mi_fp_setreturnisnull(Gen_fparam, 0, MI_FALSE);
    return ret_val;
}

