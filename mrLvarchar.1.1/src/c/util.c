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
** Title:          util.c
** Author:         jta@informix.com
** Created:        03/03/2000 12:58
** Description:    Utility functions for the mrLvarchar DataBlade module.
**
** mrLvarchar Constructor/Destructor:
**
**  mrLvarchar_from_char    Creates an mrLvarchar from a stream of bytes
**  mrLvarchar_to_char      Creates a fully expanded, NULL-terminated string 
**                          from an mrLvarchar, always allocates return result.
**  mrLvarchar_to_char_opt  Returns a pointer to mrLvarchar data, only
**                          allocates storage if value is in smart blob.
**
** Intermediate Object Management:
**
**    storage_flag_check    Verifies storage flag.
**    storage_get_mem       Gets intermediate storage flag.
**    storage_flag_to_md    Converts storage flag to MI_MEMORY_DURATION.
**    mrLvarcharTemp_get    UDR: Gets the storage flag for intermediate objects.
**    mrLvarcharTemp_set    UDR: Sets the storage flag for intermediate objects.
**
**    cache_from_char       Stores a large value in a memory cache.
**    cache_get_ptr         Returns a pointer to the cache.
**    cache_copy            Makes a copy of the cache, returns pointer.
**    cache_free            Frees the in-memory cache.
**
** Smart Large Object Management:
**
**    sblob_conn_get        Gets a connection handle for sblob routines.
**    sblob_conn_free       Frees the handle returned by sblob_conn_get.
**    sblob_from_char       Creates a smart blob from a stream of data.
**    sblob_from_tmpsblob   Creates a permanent sblob from a temp sblob.
**    sblob_to_char         Allocates an mi_lvarchar and copies data 
**                          into it from a smart blob.
**    sblob_size            Gets the size of a smart blob.
**    sbspace_name          UDR: Returns the name of the smart blob space in
**                          which a smart blob is stored.
**    sbspace_name_column   Gets the smart blob space specified for a 
**                          given table column.
**
** Misc:
**    raise_gl_error        Raises a GLS error.
*/

#include "mrLvarchar.h"

/* ================= mrLvarchar Constructor/Destructor ====================
**  mrLvarchar_from_char   Creates an mrLvarchar from a stream of bytes.
**  mrLvarchar_to_char     Creates a fully expanded, NULL-terminated string 
**                         from an mrLvarchar, always allocates return result.
**  mrLvarchar_to_char_opt Returns the pointer to mrLvarchar data, only
**                         allocates storage for and copies if smart blob
** =======================================================================*/


mi_lvarchar *           /* mrLvarchar struct packaged up in an mi_lvarchar */
mrLvarchar_from_char
(
mi_char     *data_in,   /* don't count on incoming data being null-terminated */
mi_integer  data_len,   /* size in bytes of the incoming data */
MI_FPARAM   *fp         /* UDR function parameter info */
)
{
    MI_CONNECTION   *Gen_Con=NULL;     /* The current connection. */
    mi_lvarchar     *Gen_RetVal=NULL;  /* The return value.     */
    mi_integer      Gen_UDTSize;       /* UDT size in bytes. */
    mrLvarchar      *Gen_OutData;      /* Pointer to UDT value in Gen_RetVal */
    mrLvarcharHdr   hdr;
    mi_string       *r="mrLvarchar_from_char",
                    msgbuf[80];
    MI_LO_HANDLE    *slob_p=NULL;      /* Pointer to smart blob handle. */
    mi_char         *data_p=NULL;      /* Pointer to value cached in memory */ 

    FUNC_TRACE_ENTER(r);

    /* Calculate the size needed for the UDT allocation */
    if (data_len <= MR_THRESH)
        Gen_UDTSize = sizeof(mrLvarcharHdr) + data_len;
    else
        Gen_UDTSize = sizeof(mrLvarcharHdr) + sizeof(MI_LO_HANDLE);

    /* Allocate a new UDT for the return result. */
    Gen_RetVal = mi_new_var( Gen_UDTSize );
    if( Gen_RetVal == (mi_lvarchar*)NULL )
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* Point to the output data. */
    Gen_OutData = (mrLvarchar *)mi_get_vardata( Gen_RetVal );

    /* Save the size of the text data.
    **
    ** So the end-user can more accurately anticipate disk storage,
    ** the Length() routine returns the total UDT size:
    **
    **        hdr.size + sizeof(mrLvarcharHdr)
    */
    hdr.size=data_len;

    /* Find out the storage for intermediate large (out-of-row) values. */
    if (data_len <= MR_THRESH)
        hdr.storage=MR_InRow;
    else
        hdr.storage = storage_get_mem();

    /* Save the header info in the UDT object. */
    memcpy(&Gen_OutData->hdr, &hdr, sizeof(mrLvarcharHdr));

    /* If trace is on, output data size and storage */
    if ( tf(TRACE_CLASS, TRACE_MEDIUM) )
    {
        sprintf(msgbuf, "actual text length=%d, storage=%d",
            data_len, hdr.storage);

        GL_DPRINTF(TRACE_CLASS,
            TRACE_MEDIUM,
            (    "idn_mrLvarchar_ANY_TEXT",
                "FUNCNAME%s",   r,
                "MESSAGE%s",    msgbuf,
                MI_LIST_END
            )
        );
    } /* END trace block */

    /* Deal with the input data based on its intermediate storage type */
    switch(hdr.storage)
    {
    case MR_InRow:    /* The value is small enough to store in-row */

        memcpy(&Gen_OutData->u.data, data_in, data_len);
        break;

    case MR_TmpLo: /* Store it out-of-row in a smart blob */
    case MR_Lo:

        Gen_Con = sblob_conn_get( r );

        slob_p = sblob_from_char(Gen_Con, data_in, data_len, 
            hdr.storage, fp);
        memcpy (&Gen_OutData->u.lo, (mi_char *) slob_p, sizeof(MI_LO_HANDLE));

        sblob_conn_free(Gen_Con);

        break;

    default: /* Store it out-of-row in separately allocated memory */
        if((storage_flag_check(hdr.storage)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: unhandled storage type '%d'",
                r, hdr.storage);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */
        }

        data_p=(mi_char *)cache_from_char(data_in, data_len, hdr.storage);
        Gen_OutData->u.mem=data_p;
        break;
    }

    FUNC_TRACE_EXIT(r);

    return Gen_RetVal;
}

mi_char *               /* Null-terminated copy of the input value */
mrLvarchar_to_char
(
mi_lvarchar *input_val, /* UDT in its internal, binary format */
mi_integer  *data_len   /* Sets size of mi_char returned */
)
{
    MI_CONNECTION  *Gen_Con=NULL;     /* The current connection.        */
    mrLvarchar     *object=NULL;      /* Pointer to the input data.    */
    mi_char        *Gen_RetVal=NULL;  /* Return value */
    mi_string      msgbuf[80],
                   *r="mrLvarchar_to_char",
                   *p=NULL;
    mi_integer     status;

    FUNC_TRACE_ENTER( r );

    /* Point to the input data. */
    object = (mrLvarchar *)mi_get_vardata(input_val);

    /* Set the data size (not including NULL terminator) */
    *data_len = object->hdr.size;

    /* Allocate storage for the output string. 
    ** Include an additional byte for a NULL-terminator.
    */
    Gen_RetVal = mi_alloc( object->hdr.size + 1);
    if( Gen_RetVal == (mi_char *)NULL )
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                r, (mi_integer) NULL);
        return (mi_char *)NULL;/* not reached */
    }

    /* Copy the data from the C struct into the return value. */
    switch(object->hdr.storage)
    {
    case MR_InRow:
        memcpy (Gen_RetVal, (char *)&object->u.data, object->hdr.size);
        break;

    case MR_TmpLo:
    case MR_Lo:

        Gen_Con = sblob_conn_get( r );

        status = mi_lo_to_buffer(Gen_Con, &object->u.lo,
            object->hdr.size, (mi_char **) &Gen_RetVal);
        if(status == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_to_buffer failed (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_char *)NULL; /* not reached */
        }

        /* Make sure that we actually copied the number of bytes
        ** we expected to copy.
        */
        if(status != object->hdr.size)
        {
            sprintf(msgbuf,
                "%s: mi_lo_to_buffer returned %d, but the UDT had %d bytes",
                r, status, object->hdr.size);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_char *)NULL; /* not reached */
        }

        sblob_conn_free(Gen_Con);
        break;
    default: 
        /* The rest are various memory durations for data
        ** cached in separately allocated memory.
        */
        p=cache_get_ptr(object, r);
        memcpy (Gen_RetVal, p, object->hdr.size);

        break;
    }

    /* Null terminate the copy */
    Gen_RetVal[object->hdr.size] = '\0';

    FUNC_TRACE_EXIT( r );

    return Gen_RetVal;
}

mi_char *    
mrLvarchar_to_char_opt
(
mi_lvarchar *object,    /* UDT in its internal, binary format */
mi_integer  *data_len,  /* Sets size of mi_char returned */
mi_integer  *alloc      /* Indicates if return value was allocated */
)
{
    MI_CONNECTION   *Gen_Con=NULL;     /* The current connection.      */
    mrLvarchar      *obj_data=NULL;    /* Pointer to input data arg1.  */
    mi_char           *p=NULL,
                    msgbuf[80],
                    *r="mrLvarchar_to_char_opt";
    mi_integer      status;

    FUNC_TRACE_ENTER( r );

    *alloc=0; /* return result was not allocated */

    obj_data = (mrLvarchar *)mi_get_vardata( object );
    switch(obj_data->hdr.storage)
    {
    case MR_InRow:        
        p = obj_data->u.data;
        break;

    case MR_TmpLo:
    case MR_Lo:

        Gen_Con = sblob_conn_get( r );

        p = (mi_char *) mi_alloc(obj_data->hdr.size);
        if(p == (mi_char *)NULL)
        {
            mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s",
                r, (mi_integer) NULL);
            return (mi_char *)NULL;/* not reached */
        }
        *alloc=1; /* let the caller know return result was allocated */

        status = mi_lo_to_buffer(Gen_Con, &obj_data->u.lo,
            obj_data->hdr.size, (mi_char **) &p);
        if(status == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_to_buffer failed (%d)!", r, status);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_char *)NULL; /* not reached */
        }
        if(status != obj_data->hdr.size)
        {
            sprintf(msgbuf,
                "%s: mi_lo_to_buffer returned %d, but the UDT had %d bytes",
                r, status, obj_data->hdr.size);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (mi_char *)NULL; /* not reached */
        }

        sblob_conn_free(Gen_Con);
        break;
    default: /* The rest are various durations for cached memory */
        p = cache_get_ptr(obj_data, r);
        break;
    } /* END switch */

    *data_len = obj_data->hdr.size;

    FUNC_TRACE_EXIT( r ); 

    return p;
}

/* ================= Intermediate Object Management ======================= 
**    storage_flag_check   Verifies storage flag.
**    storage_get_mem      Gets intermediate storage flag.
**    storage_flag_to_md   Converts storage flag to memory duration.
**    mrLvarcharTempGet    UDR: Gets the storage for intermediate objects.
**    mrLvarcharTempSet    UDR: Sets the storage for intermediate objects.
**    cache_from_char      Stores a large value in memory.
**    cache_get_ptr        Returns a pointer to cached data.
**    cache_copy           Makes a copy of the cached data, returns pointer.
**    cache_free           Frees the in-memory cache.
** =======================================================================*/


/* storage_flag_check()
**
**    Verifies the intermediate storage flag. Returns MI_OK if it is
**    valid and MI_ERROR if it is not valid.
*/
mi_integer
storage_flag_check
(
mi_integer flag
)
{
    mi_integer ret_val;

    switch(flag)
    {
    case MR_InRow:
    case MR_TmpLo:
    case MR_Lo:    
    case MR_Cached_RTN:
    case MR_Cached_CMD:
    case MR_Cached_STM:    
    case MR_Cached_TRX:    
    case MR_Cached_SES:
        ret_val=MI_OK;
        break;
    default:
        ret_val=MI_ERROR;
        break;
    }
    return ret_val;
}

/* storage_get_mem()
**
**    Returns the named memory flag that stores the storage option
**    for temporary UDT objects. If named memory has not been
**    initialized yet, calls mrLvarchar_Temp_set with the default
**    setting of "LO_TEMP".
**
**    Named memory management is not documented yet in the
**    "Informix DataBlade API Programmer's Manual".  It is documented
**    in the following IDN (http://www.informix.com/idn) DataBlade
**    Corner tech notes:
**
**  - Memory Allocation for C UDRs
**  - Semi-Public Functions for DataBlade Module Development
*/
mi_integer
storage_get_mem()
{
    mi_lvarchar   *temp=NULL;
    mi_string     msgbuf[80],
                  *r="storage_get_mem";
    mi_integer    storage_flag,
                  *mem_flag=NULL,
                  memstat;        /* mi_* func status */

    FUNC_TRACE_ENTER( r );

    memstat = mi_named_get
            (mrLvarchar_MEM_NAME, PER_SESSION, (void **) &mem_flag);

    switch(memstat)
    {
        /* We got it. */
        case MI_OK:
            storage_flag = *mem_flag;
            break;

        case MI_NO_SUCH_NAME:
            /* Hasn't been initialized yet -- set it to default */
            temp = mrLvarcharTemp_set(mi_string_to_lvarchar("LO_TEMP"));
            if(temp != (mi_lvarchar *)NULL)
                storage_flag = MR_TmpLo;
            else
            {
                sprintf(msgbuf, 
                    "%s: mrLvarcharTemp_set failed.", r);
                mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
                return MI_ERROR; /* not reached */
            }

            break;

        case MI_ERROR:
            sprintf(msgbuf, 
                "%s: mi_named_get failed.", r);
            mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
            return MI_ERROR; /* not reached */

        default:
            sprintf(msgbuf,
                "%s: unhandled mi_named memory case %d.", 
                r, memstat);
            mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
            return MI_ERROR;
    }

    FUNC_TRACE_EXIT( r );

    return storage_flag;
}

MI_MEMORY_DURATION    
storage_flag_to_md(mi_integer flag, mi_string *funcname)
{
    MI_MEMORY_DURATION md;
    mi_string          msgbuf[80];

    switch(flag)
    {
    case MR_Cached_RTN: 
        md=PER_ROUTINE;     
        break;
    case MR_Cached_CMD: 
        md=PER_COMMAND;     
        break;
    case MR_Cached_STM: 
        md=PER_STATEMENT;   
        break;
    case MR_Cached_TRX: 
        md=PER_TRANSACTION; 
        break;
    case MR_Cached_SES: 
        md=PER_SESSION;     
        break;
    default:
        sprintf(msgbuf, "%s: unhandled storage type '%d'", funcname, flag);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return PER_NONE; /* not reached */
    }

    return md;
}

/* mrLvarcharTemp_get()
**
**    UDR entrypoint for mrLvarcharTemp() SQL routine.
**    Calls storage_get_mem() to get the intermediate storage 
**    flag, then converts it to a human readable form.
*/

UDREXPORT
mi_lvarchar *
mrLvarcharTemp_get()
{
    mi_lvarchar        *Gen_RetVal=NULL;
    mi_string        *p=NULL;
    mi_integer        flag=0;
    mi_string        *r="mrLvarcharTemp_get";

    FUNC_TRACE_ENTER(r);

    flag=storage_get_mem();

    switch(flag)
    {
    case MR_TmpLo:
        p="LO_TEMP";
        break;
    case MR_Lo:
        p="LO";
        break;
    case MR_Cached_RTN:
        p="PER_ROUTINE";
        break;
    case MR_Cached_CMD:
        p="PER_COMMAND";
        break;
    case MR_Cached_STM:
        p="PER_STATEMENT";
        break;
    case MR_Cached_TRX:
        p="PER_TRANSACTION";
        break;
    case MR_Cached_SES:
        p="PER_SESSION";
        break;
    default:
        mi_db_error_raise(NULL, MI_EXCEPTION, 
            "mrLvarcharTemp_get: unhandled flag!");
        return (mi_lvarchar *)NULL; /* not reached */
    }

    Gen_RetVal=mi_string_to_lvarchar(p);
    if(Gen_RetVal == (mi_lvarchar *)NULL) /* memory allocation failed */
    {
        /* Raise the DBDK error */
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
            r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    FUNC_TRACE_EXIT(r);

    return Gen_RetVal;
}

/* mrLvarcharTemp_set()
**
** UDR entrypoint for mrLvarcharTemp(lvarchar) SQL routine.
** Sets the intermediate storage flag. Returns the value
** that it set in its human readable form.
*/

UDREXPORT
mi_lvarchar *
mrLvarcharTemp_set
(
mi_lvarchar    *storage
)
{
    mi_string   storage_buf[80],
                msgbuf[80],
                *r="mrLvarcharTemp_set";
    mi_integer  storage_flag,
                *mem_flag=NULL,
                memstat,          /* get/allocated status */
                lockstat,         /* lock status */
                i;                /* loop counter */

    FUNC_TRACE_ENTER( r );

    /* First get the storage type */
    if( mi_get_varlen(storage) >= sizeof(storage_buf) )
    {
        sprintf(msgbuf, "%s: '%s' not a valid option", 
            r, mi_lvarchar_to_string(storage) );
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL;    /* not reached */
    }
    memset(storage_buf, '\0', sizeof(storage_buf));
    mi_var_to_buffer(storage, storage_buf);

    if(!strcmp(storage_buf, "LO_TEMP"))
        storage_flag=MR_TmpLo;
    else if(!strcmp(storage_buf, "LO"))
        storage_flag=MR_Lo;
    else if(!strcmp(storage_buf, "PER_ROUTINE"))
        storage_flag=MR_Cached_RTN;
    else if(!strcmp(storage_buf, "PER_COMMAND"))
        storage_flag=MR_Cached_CMD;
    else if(!strcmp(storage_buf, "PER_STATEMENT"))
        storage_flag=MR_Cached_STM;
    else if(!strcmp(storage_buf, "PER_TRANSACTION"))
        storage_flag=MR_Cached_TRX;
    else if(!strcmp(storage_buf, "PER_SESSION"))
        storage_flag=MR_Cached_SES;
    else
    {
        sprintf(msgbuf, "%s: '%s' not a valid option", 
            r, mi_lvarchar_to_string(storage) );
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL;    /* not reached */
    }

    /* Allocate the named memory. If it already exists, get it.  */
    memstat = mi_named_alloc (sizeof(mi_integer),
        mrLvarchar_MEM_NAME, PER_SESSION, (void**) &mem_flag);
    if(memstat == MI_NAME_ALREADY_EXISTS)
    {
        memstat = mi_named_get
            (mrLvarchar_MEM_NAME, PER_SESSION, (void **) &mem_flag);
    }

    switch(memstat)
    {
        /* We got it. */
        case MI_OK:
            break;

        case MI_ERROR:
            sprintf(msgbuf, 
                "%s: mi_named_get or mi_named_zalloc failed.", r);
            mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */

        case MI_NO_SUCH_NAME:
            sprintf(msgbuf, 
                "%s: mi_named_get said no such name.", r);
            mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL; /* not reached */

        default:
            sprintf(msgbuf,
                "%s: unhandled mi_named memory case %d.", 
                r, memstat);
            mi_db_error_raise (NULL, MI_EXCEPTION, msgbuf);
            return (mi_lvarchar *)NULL;
    }

    /*
    ** BEGIN CRITICAL SECTION.
    **
    ** Access to named memory is done inside a 
    ** lock-protected section of code.
    **
    ** If two threads in this session try to modify the 
    ** memory at the same time, the second one blocks on
    ** the mi_lock_memory call. Unlikely to occur for 
    ** this UDR, but we handle the possibility anyhow.
    */
    /* 10 tries is very pessimistic */
    for (lockstat=MI_LOCK_IS_BUSY, i=0;
        lockstat == MI_LOCK_IS_BUSY && i < 10; i++)
    {
        lockstat=mi_try_lock_memory(mrLvarchar_MEM_NAME, PER_SESSION);
        switch(lockstat)
        {
            case MI_OK:
                break;

            case MI_LOCK_IS_BUSY:
                mi_yield(); /* Yield the processor. */
                break;

            case MI_NO_SUCH_NAME:
                sprintf(msgbuf, 
                    "%s: mi_try_lock_memory returned MI_NO_SUCH_NAME.", r);
                mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
                return (mi_lvarchar *)NULL; /* not reached */

            case MI_ERROR:
                sprintf(msgbuf, 
                    "%s: mi_try_lock_memory failed.", r);
                mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
                return (mi_lvarchar *)NULL; /* not reached */

               default:
                sprintf(msgbuf, 
                    "%s: mi_try_lock unhandled status %d", r, lockstat);
                mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
                return (mi_lvarchar *)NULL; /* not reached */
        } /* END switch */
    } /* END try to get a lock */


    /* Check the status after coming out of the loop. */
    if(lockstat == MI_LOCK_IS_BUSY)
    {
        sprintf(msgbuf, "%s: failed to get lock!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    /* We have the pointer to the named memory, and it is locked. */
    *mem_flag = storage_flag;

    /* Release the lock */
    lockstat=mi_unlock_memory(mrLvarchar_MEM_NAME, PER_SESSION);

    FUNC_TRACE_EXIT( r);
    
    return storage;
}

/* cache_from_char()
**
** Stores the value in the first argument in memory that is allocated  
** with the memory duration specified by the second argument.
**
** This function prepends a check value to the in-memory value.
*/

char *          /* Pointer to the allocated memory */
cache_from_char
(
mi_char       *data_in,   /* stream of bytes */
mi_integer    data_len,   /* length of the input data */
mi_integer    flag        /* Memory duration flag */
)
{
    MI_MEMORY_DURATION  md;
    mi_string           msgbuf[80],
                        *r="cache_from_char";
    char                *data_object=NULL,
                        *p=NULL;

    FUNC_TRACE_ENTER(r);

    if (data_len == 0)
    {
        sprintf(msgbuf, "%s: object passed has no data!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (void *)NULL; /* not reached */
    }

    /* =========== get the memory duration =========== */
    md=storage_flag_to_md(flag, r);

    /* =========== Allocate the storage ============== */
    data_object = (char *) mi_dalloc(data_len + CACHE_CHECK_SIZE, md);
    if(data_object == (char *)NULL)
    {
        sprintf(msgbuf, "%s: mi_dalloc failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (void *)NULL; /* not reached */
    }

    p=data_object;

    /* Copy the check value */
    memcpy(p, CACHE_CHECK_VAL, CACHE_CHECK_SIZE);

    p += CACHE_CHECK_SIZE;

    /* Copy the large value into the buffer */
    memcpy(p, data_in, data_len);

    FUNC_TRACE_EXIT( r );

    return data_object;
}

mi_char *           /* pointer to the cached value */
cache_get_ptr
(
mrLvarchar *object,
mi_string  *funcname
)
{
    mi_char             *p=NULL;
    mi_string           msgbuf[80],
                        *r="cache_get_ptr";

    /* Verify this is a cached object */
    switch(object->hdr.storage)
    {
    case MR_InRow:  
    case MR_TmpLo: 
    case MR_Lo:                            
       sprintf(msgbuf, 
           "%s: called %s without a cached value!", funcname, r);
       mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
       return (mi_char *)NULL; /* not reached */
    default:
       break;
    }

    if((storage_flag_check(object->hdr.storage)) == MI_ERROR)
    {
       sprintf(msgbuf, "%s: unhandled storage type '%d'",
           funcname, object->hdr.storage);
       mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
       return (mi_char *)NULL; /* not reached */
    }                    

    p=object->u.mem;

    if(strncmp(p, CACHE_CHECK_VAL, CACHE_CHECK_SIZE) != 0)
    {
       sprintf(msgbuf, "%s: cache validation check failed!", funcname);
       mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
       return (mi_char *)NULL; /* not reached */
    }

    p += CACHE_CHECK_SIZE; /* step past the check portion */      

    return p;
}

char *                 /* a copy of the cached data stored in input object */
cache_copy
(
mrLvarchar *object,    /* UDT object containing cached data */
mi_string  *funcname
)
{
    mi_char    *p=NULL, 
               *retval=NULL;
    mi_string  msgbuf[80],
               *r="cache_copy";

    p = object->u.mem;

    /*
    ** Verify the check bytes.
    */
    if(strncmp(p, CACHE_CHECK_VAL, CACHE_CHECK_SIZE) != 0)
    {
        sprintf(msgbuf, 
            "%s (%s): cache validation check failed!", funcname, r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_char *)NULL; /* not reached */
    }                  

    /* Allocate storage for the copy*/
    retval = (char *) mi_alloc(object->hdr.size + CACHE_CHECK_SIZE);
    if(retval == (char *)NULL)
    {
        sprintf(msgbuf, "%s (%s): memory allocation failed for %d bytes.",
            funcname, r, object->hdr.size + CACHE_CHECK_SIZE);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_char *)NULL; /* not reached */
    }          

    /* Copy the data -- don't forget to copy the check bytes */
    memcpy (retval, p, object->hdr.size + CACHE_CHECK_SIZE);
   
    return retval;
}

void
cache_free
(
mrLvarchar *object
)
{
    /* Verify that the object is one that we can free */
    switch(object->hdr.storage)
    {
    case MR_InRow:  /* Nothing needs to be done. */
    case MR_TmpLo:  /* It's a smart blob. */
    case MR_Lo:                            
       return;
    default:
       break;
    }

    mi_free(object->u.mem);

    return;
}

/* ================== SMART BLOB MANAGEMENT ==============================
**
**    sblob_conn_get        Gets a connection handle for sblob routines.
**    sblob_conn_free       Frees the handle returned by sblob_conn_get.
**    sblob_from_char       Creates a smart blob from a stream of data.
**    sblob_from_tmpsblob   Creates a permanent sblob from a temp sblob.
**    sblob_to_char         Allocates an mi_lvarchar and copies data 
**                          into it from a smart blob.
**    sblob_size            Gets the size of a smart blob.
**    sbspace_name          Returns the name of the smart blob space in
**                          which a smart blob is stored.
**    sbspace_name_column   Gets the smart blob space specified for a 
**                          given table column.
** ======================================================================= */

/* sblob_conn_get()
**
** Gets a connection handle for smart blob operations.
*/

MI_CONNECTION *
sblob_conn_get(mi_string *routine)
{
    MI_CONNECTION *Gen_Con=NULL;

    Gen_Con = mi_open( NULL, NULL, NULL );
    if( Gen_Con == (MI_CONNECTION *)NULL )
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN1", "FUNCTION%s", 
            routine, (mi_integer) NULL);
        return (MI_CONNECTION *)NULL; /* not reached */
    }

    return Gen_Con;
}

/* sblob_conn_free()
**
** Frees the connection handle returned by sblob_conn_get.
*/
void
sblob_conn_free(MI_CONNECTION *sblob_conn)
{
    (void) mi_close(sblob_conn);
    return;
}


/* sblob_from_char()
**
** Creates a smart blob from a stream of data, which might be from
** the lvarchar passed to the Input() routine, or from the data
** cached in memory.
**
** This routine does not bump the refcount. 
**
** flag:
**   MR_TmpLo:   9.2: gets created as a regular LO without logging.
**   MR_Lo:      gets created as a regular LO in the default SBSPACENAME.
**   LO_ASSIGN:  Assign called, storage info is available in the MI_FPARAM
**
*/
MI_LO_HANDLE *
sblob_from_char
(
MI_CONNECTION    *Gen_Con,  /* Connection handle */
mi_char          *data_in,  /* Data to copy into the smart blob */
mi_integer       data_len,  /* Size of the data */
mi_integer       flag,      /* Storage flag */
MI_FPARAM        *fp        /* UDR function parameter info */
)
{
    MI_LO_SPEC   *lo_spec_p = NULL; /* must be NULL */
    MI_LO_HANDLE *slob_p = NULL;
    MI_LO_FD     slob_fd;
    mi_integer   status,
                 specset_flag,
                 slob_flags=MI_LO_WRONLY;
    mi_string    *r="slob_from_char",
                 msgbuf[80];
    mi_int8      int8_losize;

    FUNC_TRACE_ENTER(r);

    /*  This function does the following:    
    **
    **  1. mi_lo_spec_init():        allocates MI_LO_SPEC structure
    **  2. mi_lo_specset_flags():    flag==MR_TmpLo : make it a temp lo
    **  3. mi_lo_colinfo_by_ids():   flag==LO_ASSIGN: set storage
    **  4. mi_lo_specset_estbytes(): set the size
    **  5. mi_lo_create():           create a smart blob
    **  6. mi_lo_write():            write the data to the smart blob
    **  7. Cleanup
    */

    /* ==== 1. mi_lo_spec_init(): allocate MI_LO_SPEC structure ===== 
    **
    ** The pointer passed to mi_lo_spec_init must be NULL for it
    ** to allocate storage for the MI_LO_SPEC structure.
    */
    if ((mi_lo_spec_init(Gen_Con, &lo_spec_p)) == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_spec_init failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 2. mi_lo_specset_flags() ================================ 
    **
    ** If flag == MR_TmpLo
    **
    ** Next major release (post 9.21), will support LO_CREATE_TEMP
    ** flag that creates a temporary LO that won't even log the smart 
    ** blob metadata.
    */
    
    if(flag == MR_TmpLo)
    {
        specset_flag=MI_LO_ATTR_NO_LOG;
        if ((mi_lo_specset_flags(lo_spec_p, specset_flag)) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_specset_flags failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (MI_LO_HANDLE *)NULL; /* not reached */
        }
    }

    /* ==== 3. mi_lo_colinfo_by_ids() ================================
    **
    ** If flag == LO_ASSIGN, it means that the Assign() routine called
    ** this function and actual storage information is available for 
    ** the new smart blob.
    */
    if(flag == LO_ASSIGN)
    {
        if((mi_lo_colinfo_by_ids ( Gen_Con,
            mi_fp_getrow ( fp ),
            mi_fp_getcolid ( fp ),
            lo_spec_p )) == MI_ERROR)
        {
            sprintf(msgbuf, "%s: mi_lo_colinfo_by_ids failed!", r);
            mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
            return (MI_LO_HANDLE *)NULL; /* not reached */
        }
    }

    /* ==== 4. mi_lo_specset_estbytes(): set the size ================ */
    if ((ifx_int8cvlong(data_len, &int8_losize)) < 0)
    {
        sprintf(msgbuf, "%s: ifx_int8cvlong() failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    if((mi_lo_specset_estbytes(lo_spec_p, &int8_losize)) == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_specset_estbytes() failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 5. mi_lo_create(): create the smart blob ================= */
    slob_fd = mi_lo_create (Gen_Con, lo_spec_p, slob_flags, &slob_p);
    if(slob_fd == MI_ERROR)
    {
        (void) mi_lo_spec_free(Gen_Con, lo_spec_p);
        sprintf(msgbuf, 
            "%s: mi_lo_create failed! Are you out of smart blob space?",
            r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 6. mi_lo_write(): write the data to the smart blob ======= */
    status = mi_lo_write(Gen_Con, slob_fd, data_in, data_len);
    if (status == MI_ERROR)
    {
        (void) mi_lo_spec_free(Gen_Con, lo_spec_p);
        (void) mi_lo_close(Gen_Con, slob_fd); 
        sprintf(msgbuf, 
            "%s: mi_lo_write failed! Are you out of smart blob space?",
            r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }
    if(status != data_len)
    {
        (void) mi_lo_spec_free(Gen_Con, lo_spec_p);
        (void) mi_lo_close(Gen_Con, slob_fd); 
        sprintf(msgbuf, 
            "%s: mi_lo_write wrote %d bytes, but we told it %d",
            r, status, data_len);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 7. Cleanup =============================================== */
    (void) mi_lo_spec_free (Gen_Con, lo_spec_p );
    (void) mi_lo_close(Gen_Con, slob_fd); /* otherwise stays to session end! */

    FUNC_TRACE_EXIT( r );

    return slob_p;
}

/* sblob_from_tmpsblob
**
** Creates a permanent smart blob from a temporary smart blob.
*/

MI_LO_HANDLE *
sblob_from_tmpsblob
(
MI_CONNECTION    *Gen_Con,  /* Connection handle */
MI_LO_HANDLE     *tmpsblob, /* Temporary smart blob handle */
MI_FPARAM        *fp        /* UDR function info--provides sblob storage info */
)
{
    MI_LO_SPEC   *lo_spec_p = NULL; /* must be NULL */
    MI_LO_HANDLE *slob_p = NULL;
    MI_LO_FD     slob_fd;
    mi_integer   flags  = MI_LO_WRONLY;
    mi_string    *r="sblob_from_tmpsblob",
                 errorbuf[80];

    FUNC_TRACE_ENTER(r);

    /*    To create the permanent smart blob and copy the data 
    **    into it, we do the following steps:
    **
    **    1. mi_lo_spec_init():      allocate MI_LO_SPEC structure
    **    2. mi_lo_colinfo_by_ids(): set storage characteristics
    **    3. mi_lo_copy():           Copy the tmpsblob to the new sblob
    **    4. Cleanup
    */

    /* ==== 1. mi_lo_spec_init(): allocate MI_LO_SPEC structure ====== 
    **
    ** The pointer passed to mi_lo_spec_init must be NULL for it
    ** to allocate storage for the MI_LO_SPEC structure.
    */
    if ((mi_lo_spec_init(Gen_Con, &lo_spec_p)) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_spec_init failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 2. mi_lo_colinfo_by_ids(): set storage characteristics === 
    **
    ** Put the smart blob in the correct sbspace.
    */
    if((mi_lo_colinfo_by_ids ( Gen_Con,
        mi_fp_getrow ( fp ),
        mi_fp_getcolid ( fp ),
        lo_spec_p )) == MI_ERROR)
    {
        sprintf(errorbuf, "%s: mi_lo_colinfo_by_ids failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 3. mi_lo_copy(): Copy the tmpsblob to the new ======= */

    slob_fd = mi_lo_copy (Gen_Con, tmpsblob, lo_spec_p, flags, &slob_p); 
    if(slob_fd == MI_ERROR)
    {
        (void) mi_lo_spec_free(Gen_Con, lo_spec_p);
        sprintf(errorbuf, "%s: mi_lo_copy failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return (MI_LO_HANDLE *)NULL; /* not reached */
    }

    /* ==== 4. Cleanup ============================================= */
    (void) mi_lo_spec_free (Gen_Con, lo_spec_p );
    (void) mi_lo_close(Gen_Con, slob_fd); /* otherwise stays to session end! */

    FUNC_TRACE_EXIT( r );

    return slob_p;
}


/* sbspace_name()
** 
** Returns the name of the smart blob 
** space in which a smart blob is stored.
*/

void
sbspace_name
(
MI_CONNECTION   *Gen_Con,         /* Connection handle */
MI_LO_HANDLE    *sblob,           /* smart blob */
mi_string       *space_name,      /* This function stores space name here */
mi_integer      space_name_length /* Length of space_name buffer */
)
{
    MI_LO_FD    slob_fd;          /* smart blob descriptor */
    MI_LO_STAT  *lostat=NULL;     /* so we can get stat struct */
    MI_LO_SPEC  *lospec=NULL;     /* so we can get info */

    mi_string   msgbuf[80],
                *r="sbspace_name";

    FUNC_TRACE_ENTER(r);

    if ((mi_lo_validate(Gen_Con, sblob)) != 0)
    {
        (void) mi_close(Gen_Con);
        sprintf(msgbuf, "%s: lo handle not valid!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return ; /* not reached */
    }

    /* Open the smart blob */
    if ((slob_fd = mi_lo_open(Gen_Con, sblob, MI_LO_RDONLY)) == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_open failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* not reached */   
    }

    /* Get the LO_STAT structure */
    if ((mi_lo_stat(Gen_Con, slob_fd, &lostat)) != MI_OK)
    {
        (void) mi_lo_close(Gen_Con, slob_fd);
        sprintf(msgbuf, "%s: mi_lo_stat failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* not reached */   
    }

    /* Get the LO_SPEC structure */
    lospec = mi_lo_stat_cspec(lostat);
    if (lospec == NULL)
    {
        (void) mi_lo_stat_free(Gen_Con, lostat);
        (void) mi_lo_close(Gen_Con, slob_fd);
        sprintf(msgbuf, "%s: mi_lo_stat_cspec failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return ; /* not reached */   
    }

    if ((mi_lo_specget_sbspace(lospec, space_name, space_name_length)) 
        == MI_ERROR)
    {
        (void) mi_lo_stat_free(Gen_Con, lostat);
        (void) mi_lo_close(Gen_Con, slob_fd);
        sprintf(msgbuf, "%s: mi_lo_specget_sbspace failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return ; /* not reached */   
    }

    /* Cleanup */
    (void)mi_lo_stat_free(Gen_Con, lostat);
    (void)mi_lo_close(Gen_Con, slob_fd);

    FUNC_TRACE_EXIT(r);

    return ;
}

/* sbspace_name_column()
**
** Gets the smart blob space specified for a given table column.
** The Assign() routine has column information.
*/

void
sbspace_name_column
(
MI_CONNECTION    *Gen_Con,           /* Connection handle */
mi_string        *space_name,        /* This function fills in space name */
mi_integer        space_name_length, /* Length of space_name buffer */
MI_FPARAM        *fp                 /* UDR info -- provides storage info */
)
{
    MI_LO_SPEC   *lo_spec_p = NULL;  /* must be NULL */
    mi_string    *r="sbspace_name_column",
                 msgbuf[80];

    FUNC_TRACE_ENTER(r);

    /*    To get the smart blob space, we do the following steps:
    **
    **    1. mi_lo_spec_init():      allocate an MI_LO_SPEC structure
    **    2. mi_lo_colinfo_by_ids(): set storage characteristics
    **    3. mi_lo_specset_get_sbspace() : get the smart blob space
    **    4. Cleanup
    */

    /* ==== 1. mi_lo_spec_init(): allocate MI_LO_SPEC structure ====== 
    **
    ** The pointer passed to mi_lo_spec_init must be NULL for it
    ** to allocate storage for the MI_LO_SPEC structure.
    */
    if ((mi_lo_spec_init(Gen_Con, &lo_spec_p)) == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_spec_init failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* not reached */
    }

    /* ==== 2. mi_lo_colinfo_by_ids(): set storage characteristics === 
    **
    ** Put the smart blob in the correct sbspace.
    */
    if((mi_lo_colinfo_by_ids ( Gen_Con,
        mi_fp_getrow ( fp ),
        mi_fp_getcolid ( fp ),
        lo_spec_p )) == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_colinfo_by_ids failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return; /* not reached */
    }

    /* ==== 3. mi_lo_specget_sbspace ======= */
    if ((mi_lo_specget_sbspace(lo_spec_p, 
        space_name, space_name_length)) == MI_ERROR)
    {
        (void) mi_lo_spec_free (Gen_Con, lo_spec_p );
        sprintf(msgbuf, "%s: mi_lo_specget_sbspace failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return ; /* not reached */   
    }
 
    /* ==== 4. Cleanup ============================================= */
    (void) mi_lo_spec_free (Gen_Con, lo_spec_p );

    FUNC_TRACE_EXIT( r );

    return;
}

/* sblob_to_char()
**
**    Allocates an mi_lvarchar and copies data into it from a smart blob.
*/
mi_lvarchar    * 
sblob_to_char
(
MI_LO_HANDLE        *sblob  /* Smart blob to convert to an mi_lvarchar */
)
{
    MI_CONNECTION   *Gen_Con=NULL;
    mi_int8         int8_size;
    mi_integer      status,
                    long_size;
    mi_lvarchar     *ret_val=NULL;
    mi_string       *r="sblob_to_char",
                    msgbuf[80];
    mi_char         *out;

    FUNC_TRACE_ENTER(r);

    /* Get the size */
    (void) sblob_size (sblob, &int8_size);

    /* Convert it to a long so we can allocate an mi_lvarchar */
    status = (ifx_int8tolong(&int8_size, &long_size));
    if (status == -1200)
    {
        sprintf(msgbuf, "%s: int8 value too large to convert!");
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }
    else if (status != 0)
    {
        sprintf(msgbuf, "%s: ifx_int8tolong failed!");
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }    

    ret_val = mi_new_var(long_size);
    if(ret_val == (mi_lvarchar *)NULL)
    {
        mi_db_error_raise(NULL, MI_SQL, "UGEN2", "FUNCTION%s", 
            r, (mi_integer) NULL);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    out = (mi_char *)mi_get_vardata(ret_val);

    Gen_Con = sblob_conn_get(r);

    status = mi_lo_to_buffer (Gen_Con, sblob, long_size, &out);
    if (status == MI_ERROR)
    {
        sprintf(msgbuf, "%s: mi_lo_to_buffer failed (%d)!", status);
        mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
        return (mi_lvarchar *)NULL; /* not reached */
    }

    sblob_conn_free(Gen_Con);
    FUNC_TRACE_EXIT( r );

    return ret_val;
}

/*
** Gets the size of a smart blob from the smart blob.
*/
mi_integer
sblob_size
(
MI_LO_HANDLE       *sblob,              /* Smart blob we want to know about */
mi_int8            *sblob_size          /* This function fills in size */
)
{
    MI_CONNECTION  *Gen_Con=NULL;
    MI_LO_FD       slob_fd;             /* Descriptor for access sblob */
    MI_LO_STAT     *lostat=NULL;        /* Lets us get size */
    mi_integer     flags=MI_LO_RDONLY;  /* Open mode for sblob */
    mi_string      *r="sblob_size",
                   errorbuf[80];

    FUNC_TRACE_ENTER(r);

    /*    To get the size of a smart blob:
    **
    **    1. mi_open():    get a connection handle 
    **    2. mi_lo_open(): open smart blob
    **    3. mi_lo_stat(): get size
    **    4. Cleanup: free results of steps #1 and #2
    */

    /* ==== 1. mi_open(): get a connection handle ============= */
    Gen_Con = sblob_conn_get( r );

    /* ==== 2. mi_lo_open(): open smart blob ================= */
    if ((slob_fd = mi_lo_open(Gen_Con, sblob, flags)) == MI_ERROR)
    {
        sblob_conn_free (Gen_Con);
        sprintf(errorbuf, "%s: mi_lo_open failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    /* ==== 3. mi_lo_stat(): get size ======================== */
    if ((mi_lo_stat(Gen_Con, slob_fd, &lostat)) != MI_OK)
    {
        (void) mi_lo_close(Gen_Con, slob_fd);
        sblob_conn_free (Gen_Con);
        sprintf(errorbuf, "%s: mi_lo_stat failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    if (mi_lo_stat_size(lostat, sblob_size) == MI_ERROR)
    {
        (void) mi_lo_close(Gen_Con, slob_fd);
        sblob_conn_free(Gen_Con);
        sprintf(errorbuf, "%s: mi_lo_stat_size failed!", r);
        mi_db_error_raise(NULL, MI_EXCEPTION, errorbuf);
        return MI_ERROR; /* not reached */
    }

    /* ==== 4. Cleanup: free results of steps #2 and #3 ====== */
    (void) mi_lo_stat_free(Gen_Con, lostat);
    (void) mi_lo_close(Gen_Con, slob_fd);
    sblob_conn_free(Gen_Con);

    return MI_OK;
}

/* raise_gl_error
**
** Called by idn_mrLvarchar_instr and Compare to process GLS library error.
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
    mi_string  msgbuf[80];

    /* Set the caller's UDR return value to NULL -- super cautious */
    mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );

    switch (gl_errno)
    {
        case IFX_GL_PARAMERR:
            sprintf(msgbuf, "%s: %s caused GLS error '%s'",
                func, gl_call, "IFX_GL_PARAMERR");
            break;

        case IFX_GL_TERMMISMAT:
            sprintf(msgbuf, "%s: %s caused GLS error '%s'",
                func, gl_call, "IFX_GL_TERMMISMAT");
            break; 

        case IFX_GL_EILSEQ:
            sprintf(msgbuf, "%s: %s caused GLS error '%s'",
                func, gl_call, "IFX_GL_EILSEQ");
            break;

        case IFX_GL_EINVAL:
            sprintf(msgbuf, "%s: %s caused GLS error '%s'",
                func, gl_call, "IFX_GL_EINVAL");
            break;

        default:
            sprintf(msgbuf, "%s: %s caused GLS error '%d'",
                func, gl_call, gl_errno);
            break;
    }

    mi_db_error_raise(NULL, MI_EXCEPTION, msgbuf);
    return; /* not reached */
}
