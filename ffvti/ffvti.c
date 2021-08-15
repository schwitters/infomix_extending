/*
 * Access method functions to deal with flat files (FF).
 *
 * This access method will provide the "select"
 * access to flat files.
 * The file path is provided as an argument to the access method
 * in the create table OR as an external dbspace.
 *
 * select: scancost, open, beginscan, getnext, endscan, close
 *
 * Written by: Jacques Roy 
 * Last modification 4/17/2001
 */
#include <fcntl.h>
#include <mi.h>
#include <miami.h>
#include <minmdur.h>

#include "ffvti.h"
/*----------------------------------------------------------------------*/
mi_integer ff_open(MI_AM_TABLE_DESC *ptbldesc)
{
  mi_string *pbuf;
  MI_ROW_DESC *prowdesc;
  mi_string buffer[128];
  mi_integer i;
  mi_integer col_count;
  FF_INFO *pff_info;

  DPRINTF("ffvti", 60, ("Entering ff_open()"));
  /* allocate the space for ff_info */
  pff_info = (FF_INFO *)mi_dalloc(sizeof(FF_INFO), PER_STATEMENT);

  /* open the file and save the file descriptor in the userdata area */
  if (MI_ERROR == ff_open_file(ptbldesc, pff_info)) {
    mi_db_error_raise(NULL, MI_EXCEPTION, "ffvti.c:35: Open file error");
    return(MI_ERROR);
  }
  mi_tab_setuserdata(ptbldesc, (void *)pff_info);

  /* Finish initializing the struct: delim and rows */
  prowdesc = mi_tab_rowdesc(ptbldesc);
  col_count = mi_column_count(prowdesc);
  if (col_count > 0)
  {
    pff_info->delim = ':'; /* default value */
    pbuf = ff_get_param(mi_tab_amparam(ptbldesc), "delim");
    if (pbuf != 0)
    {
      pff_info->delim = pbuf[0];
      mi_free(pbuf);
    }
    /* allocate space for return columns */
    pff_info->pdatum = (MI_DATUM *)
      mi_dalloc(col_count * sizeof(MI_DATUM), PER_STATEMENT);
    pff_info->pbool = (mi_boolean *)
      mi_dalloc(col_count * sizeof(mi_boolean), PER_STATEMENT);
  }
  /* initialize the datum to NULL */
  for (i = 0; i < col_count; i++)
    pff_info->pdatum[i] = NULL;

  /* open a connection to the server */
  pff_info->pconn = mi_open(NULL, NULL, NULL);
  
  DPRINTF("ffvti", 60, ("Exiting ff_open(): MI_OK"));
  return(MI_OK);
}
/*----------------------------------------------------------------------*/
mi_integer ff_beginscan(MI_AM_SCAN_DESC *pscandesc)
{
  MI_AM_TABLE_DESC *ptbldesc;
  MI_CALLBACK_HANDLE *pcallback;
  FF_INFO          *pff_info;
  mi_integer ret;

  DPRINTF("ffvti", 60, ("Entering ff_beginscan()"));
  /* get the file descriptor */
  ptbldesc = mi_scan_table(pscandesc);
  pff_info = (struct ff_info *)mi_tab_userdata(ptbldesc);

  /* make sure we are at the beginning of the file in case there
   * was a rescan done
   */
  ff_rewind(pff_info->fd[0]);

  DPRINTF("ffvti", 60, ("Exiting ff_beginscan(): MI_OK"));
  return(MI_OK);
}
/*----------------------------------------------------------------------*/
/* getnext is the main function that scans trhough the file
 */
mi_integer ff_getnext(MI_AM_SCAN_DESC *pscandesc,
      MI_ROW **retrow, mi_integer *retrowid)
{
  MI_AM_TABLE_DESC *ptbldesc;
  MI_ROW_DESC      *prowdesc;
  MI_TYPE_DESC     *ptype;
  MI_TYPEID        *ptid_src, *ptid_dest;
  mi_lvarchar      *plvar;
  MI_FUNC_DESC     *pfuncdesc;
  MI_FPARAM        *castfp;
  MI_DATUM         *pdatum;
  FF_INFO          *pff_info;
  mi_integer    col_count;
  mi_integer    ret;
  mi_integer    i, j, k, len, prec, scale;
  mi_string buffer[BUFSIZE], logbuf[BUFSIZE];
  mi_char      status, *pstatus = &status;
  
  DPRINTF("ffvti", 60, ("Entering ff_getnext()"));
  /* get the file descriptor */
  ptbldesc = mi_scan_table(pscandesc);
  pff_info = (struct ff_info *)mi_tab_userdata(ptbldesc);

  /* Get the row descriptor to be able to build the row */
  prowdesc = mi_tab_rowdesc(ptbldesc);
  col_count = mi_column_count(prowdesc);

  /* loop until we return a row or no more rows */
    ptid_src = mi_typename_to_id(pff_info->pconn,
                     mi_string_to_lvarchar("lvarchar"));
  while(1) {
    /* read the next line */
    if (NULL == ff_fgets(buffer, BUFSIZE, pff_info->fd[0]))
      break;

    /* build the row information */
    j = 0;
    for (i = 0; i <col_count; i++)
    {
      ptype =  mi_column_typedesc(prowdesc, i);
      len = mi_type_maxlength(ptype);
      if (len == -1) len = 255;
      prec = mi_type_precision(ptype);
      scale = mi_column_scale(prowdesc, i);
      ptid_dest = mi_column_type_id(prowdesc, i);

      /* get a field */
      for (k = j; buffer[k] != pff_info->delim && buffer[k] != 0; k++)
         ;
      /* convert the input to the appropriate format */
      /* check for NULL value, get the cast function and execute */
      buffer[k] = 0;
      /* the ff_set_col function does manually what the system cast should do */
      ret = ff_set_col(pff_info, ptype, i, &buffer[j]);
      if (ret == MI_ERROR) {
        mi_db_error_raise(NULL, MI_EXCEPTION, "ffvti.c:147: Conversion error");
        return(MI_NO_MORE_RESULTS); /* should be an exception... */
      }
      j = k + 1;
    } /* End for colcount */
    /* create the row */
    DPRINTF("ffvti", 40, ("Creating row"));
    pff_info->prow = mi_row_create(pff_info->pconn, prowdesc,
            pff_info->pdatum, pff_info->pbool);
    if (pff_info->prow == NULL)
    {
      mi_db_error_raise(NULL, MI_EXCEPTION, "ffvti.c:158: Unable to create row");
      return(MI_ERROR); /* MI_ERROR */
    }
    /* ask IDS 9.x if the row complies with the where clause */
    if ( (NULL != mi_scan_quals(pscandesc)) && MI_TRUE !=
        mi_eval_am_qual(pff_info->prow, mi_scan_quals(pscandesc)) )
      continue;
    (*retrow) = pff_info->prow;
    (*retrowid) = 0;

    DPRINTF("ffvti", 60, ("Exiting ff_getnext()"));
    return(MI_ROWS); /* or MI_NO_MORE_RESULTS, or MI_ERROR */
  } /* end while */
  DPRINTF("ffvti", 60, ("Exiting ff_getnext()"));
  return(MI_NO_MORE_RESULTS);
}
/*----------------------------------------------------------------------*/
mi_integer ff_endscan(MI_AM_SCAN_DESC *pscandesc)
{
  DPRINTF("ffvti", 60, ("Entering/Exiting ff_endscan()"));
  return(MI_OK); /* or MI_ERROR */
}
/*----------------------------------------------------------------------*/
mi_integer ff_close(MI_AM_TABLE_DESC *ptbldesc)
{
  FF_INFO *pff_info;

  DPRINTF("ffvti", 60, ("Entering ff_close()"));
  /* Get the file descriptor to close the file */
  pff_info = (struct ff_info *)mi_tab_userdata(ptbldesc);

  /* Close the files */
  ff_fclose(pff_info->fd[0]); /* close the flat file */
  mi_close(pff_info->pconn);
  DPRINTF("ffvti", 60, ("Exiting ff_close()"));
  
  return(0);
}
/*----------------------------------------------------------------------*/
mi_integer ff_drop(MI_AM_TABLE_DESC *ptbldesc)
{
  DPRINTF("ffvti", 60, ("Entering/Exiting ff_drop()"));
  return(MI_OK);
}
