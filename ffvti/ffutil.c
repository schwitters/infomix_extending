/*
 * Utility functions for the ffvti interface
 * The functions include file buffering functions:
 * ff_fopen(), ff_fclose(), ff_rewind(), ff_gets(), ff_puts()
 *
 * It also includes other support functions:
 * ff_open_files(): Find the file path and open it
 * ff_get_param(): Extract the parameter from the table definition
 * ff_set_col():  convert from character to the proper SQL type
 * set_tracing(): Turn on/off tracing
 *
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
mi_integer ff_open_file(MI_AM_TABLE_DESC *ptbldesc, FF_INFO *pff_info)
{
  mi_string *pamparam;
  mi_string *pdbspath;
  mi_string *ppath;
  mi_unsigned_integer mode;
  mi_integer flags;

  DPRINTF("ffvti", 80, ("Entering ff_open_files()"));
  pamparam = mi_tab_amparam(ptbldesc); /* access method parameters */
  pdbspath = mi_tab_spaceloc(ptbldesc); /* path of the external dbspace */

  /* if the table is in an external dbs, use that */
  if (pdbspath != NULL)
  {
    ppath = (mi_string *)mi_dalloc(strlen(pdbspath), PER_STATEMENT);
    strcpy(ppath, pdbspath);
  }
  else
  {
    /* use the path AM parameter */
    ppath = ff_get_param(pamparam, "path");
    if (ppath == NULL) {
      mi_db_error_raise(NULL, MI_EXCEPTION, "ffutil.c:45: No path parameter");
      return(MI_ERROR);
    }
  }

  /* Get the open mode for the table fragment */
  mode = mi_tab_mode(ptbldesc);
  if (MI_TRUE == MI_INPUT(mode))
    flags = O_RDONLY;
  if (MI_TRUE == MI_OUTPUT(mode))
    flags = O_WRONLY;
  if (MI_TRUE == MI_INOUT(mode))
    flags = O_RDWR;
  pff_info->fd[0] = ff_fopen(ppath, flags);
  if (pff_info->fd[0] == NULL) {
    mi_db_error_raise(NULL, MI_EXCEPTION,
               "ffutil.c:61: Unable to open the external file");
    return(MI_ERROR);
  }
  mi_free(ppath);

  DPRINTF("ffvti", 80, ("Exiting ff_open_files()"));
  return(MI_OK);
}
/*----------------------------------------------------------------------*/
/* find the value of the access method keyword argument */
mi_string *ff_get_param(mi_string *pamparam, mi_string *pname)
{
  mi_integer i, j, state;
  mi_string *pbuf;

  DPRINTF("ffvti", 80, ("Entering ff_get_param()"));
  state = 0;
  i = 0;
  while (1)
  {
    switch(state)
    {
    case 0 : /* find keyword */
      for (; pamparam[i] == ' '; i++); /* skip spaces */
      if (pamparam[i] == 0) { /* end of string! */
        DPRINTF("ffvti", 80, ("Exiting ff_get_param(): not found!"));
        return((mi_string *)NULL);
      }
      for (j = i; pamparam[j] != '='; j++)
        ; /* find keyword */
      pamparam[j] = 0;
      if (0 == strcmp(&pamparam[i], pname) )
        state = 1;
      else
        state = 2;
      pamparam[j] = '=';
      break;
    case 1: /* found the path keyword, extract string */
      for (j = i; pamparam[j] != ',' && pamparam[j] != 0; j++)
        ;
      pbuf = (mi_string *)mi_dalloc(2 + j - i, PER_STATEMENT);
      strncpy(pbuf, &pamparam[i], j - i);
      pbuf[j - i] = 0;
      DPRINTF("ffvti", 80, ("Exiting ff_get_param()"));
      return(pbuf);
      break;
    case 2: /* not found, skip string */
      for (j = i; pamparam[j] != ',' && pamparam[j] != 0; j++)
        ;
      if (pamparam[j] == 0)
        j--;
      state = 0;
      break;
    } /* end switch */
    i = j + 1;
  } /* end while */
}
/*----------------------------------------------------------------------*/
/* set the DATUM value after converting the input to the appropriate
 * type.  It should handle the distinct types properly.
 */
mi_integer ff_set_col(FF_INFO *pff_info, MI_TYPE_DESC *ptype,
        mi_integer i, mi_string *buf)
{
  MI_TYPE_DESC *pbasetype;
  dec_t     mydec;
  mi_double_precision mydouble;
  dtime_t   *pdt;
  intrvl_t  *pintr;
  mi_string *ptname;
  mi_integer cnt, ret, jdate;
  mi_string logbuf[128];
  mi_lvarchar *plvar;
  union u {
    mi_integer i;
    mi_real  *flt;
    mi_double_precision *flt8;
    mi_string *pchar;
  } retval, retval2;

  FF_TYPE_VAL types[] = {
    { "boolean", 1 }, { "byte", 2 },     { "char", 3 },
    { "date", 4},     { "datetime", 5 }, { "decimal", 6 },
    { "float", 7 },   { "int8", 8 },     { "integer",  9 },
    { "interval",10}, { "lvarchar", 3 }, { "money", 12 },
    { "nchar", 3 },   { "nvarchar", 3 }, { "smallfloat", 15 },
    { "smallint", 9}, { "text",  17 },   { "varchar", 3 },
    { "", -1}
  };

  DPRINTF("ffvti", 80, ("Entering ff_set_col()"));
  /* Get the real base type so we can use distinct types */
  if (MI_TRUE == mi_typeid_is_distinct(mi_typedesc_typeid(ptype)) )
    pbasetype = mi_get_type_source_type(ptype);
  else
    pbasetype = ptype;
  ptname = mi_type_typename(pbasetype);

  /* display the type information */
  sprintf(logbuf, "type: %s, len: %d, maxlen: %d, byvalue: %d, buf: <%s>",
    ptname, mi_type_length(pbasetype), mi_type_maxlength(pbasetype),
    mi_type_byvalue(pbasetype), buf);
  DPRINTF("ffvti", 80, (logbuf));
  sprintf(logbuf, "precision: %d, qualifier: %d, scale: %d",
    mi_type_precision(pbasetype), mi_type_qualifier(pbasetype),
    mi_type_scale(pbasetype) );
  DPRINTF("ffvti", 80, (logbuf));
  /* the type could have the form char(1), stop at the parenthesis */
  for (cnt = 0;
    ptname[cnt] != '(' && ptname[cnt] != 0 && ptname[cnt] != ' '; cnt++)
    ;
  ptname[cnt] = 0;

  /* find the type of the column */
  for (cnt = 0; cnt < 19; cnt++)
    if (0 == strcmp(ptname, types[cnt].type))
      break;
  if (types[cnt].val == -1) {
    mi_db_error_raise(NULL, MI_EXCEPTION, "ffutil.c:176: unknown column type");
    return(MI_ERROR);
  }
  /* if the datum was allocated memory, free it */
  if (pff_info->pdatum[i] != NULL && MI_TRUE == mi_type_byvalue(pbasetype))
  {
    mi_free(pff_info->pdatum[i]);
    pff_info->pdatum[i] = 0;
  }
  /* if the input string is empty, return a NULL datum */
  if (0 == strlen(buf))
  {
    pff_info->pdatum[i] = (MI_DATUM)0;
    pff_info->pbool[i] = MI_TRUE;
  DPRINTF("ffvti", 80, ("Exiting ff_set_col()"));
    return(MI_OK);
  }

  switch(types[cnt].val)
  {
  case 8: /* int8 */
    pff_info->pdatum[i] = (MI_DATUM)mi_alloc(sizeof(ifx_int8_t));
    ifx_int8cvasc(buf, strlen(buf), pff_info->pdatum[i]);
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 9: /* integer, smallint */
    sscanf(buf, "%d", &retval.i);
    pff_info->pdatum[i] = (MI_DATUM)retval.i;
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 3: /* char, nchar, varchar, nvarchar, lvarchar */
    pff_info->pdatum[i] = (MI_DATUM)mi_string_to_lvarchar(buf);
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 7: /* float */
    pff_info->pdatum[i] = (MI_DATUM)
      mi_alloc(sizeof(mi_double_precision));
    deccvasc(buf, strlen(buf), &mydec);
    dectodbl(&mydec, pff_info->pdatum[i]);
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 15: /* smallfloat */
    pff_info->pdatum[i] = (MI_DATUM)
      mi_dalloc(sizeof(mi_real), PER_FUNCTION);
    deccvasc(buf, strlen(buf), &mydec);
    dectodbl(&mydec, &mydouble);
    (*(mi_real *)pff_info->pdatum[i]) = (mi_real)mydouble;
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 6: /* decimal */
  case 12: /* money */
    plvar = mi_string_to_lvarchar(buf);
    pff_info->pdatum[i] = (MI_DATUM)mi_alloc(sizeof(mi_decimal));
    deccvasc(buf, strlen(buf), pff_info->pdatum[i]);
    pff_info->pbool[i] = MI_FALSE;
    break;
  case 4: /* date */
#if 0 
    plvar = mi_string_to_lvarchar(buf);
    pff_info->pdatum[i] = (MI_DATUM)mi_date_to_binary(plvar);
    pff_info->pbool[i] = MI_FALSE;
#endif
    ret = rstrdate(buf, &jdate);
    DPRINTF("ffvti", 40, ("rstrdate returned: %d", ret));
    pff_info->pdatum[i] = (MI_DATUM)jdate;
    pff_info->pbool[i] = MI_FALSE;

    break;
  case 10: /* interval */
    pintr = (intrvl_t *)mi_alloc(sizeof(intrvl_t));
    pintr->in_qual = mi_type_qualifier(pbasetype);
    pff_info->pdatum[i] = (MI_DATUM)pintr;
    ret = incvasc(buf, pintr);
    DPRINTF("ffvti", 40, ("incvasc returned: %d", ret));
    if (ret < 0)
      pff_info->pbool[i] = MI_TRUE;
    else
      pff_info->pbool[i] = MI_FALSE;
    break;
  case 5: /* datetime */
    pdt = (dtime_t *)mi_alloc(sizeof(dtime_t));
    pdt->dt_qual = mi_type_qualifier(pbasetype);
    pff_info->pdatum[i] = (MI_DATUM)pdt;
    ret = dtcvasc(buf, pdt);
    DPRINTF("ffvti", 40, ("dtcvasc returned: %d", ret));
    if (ret < 0)
      pff_info->pbool[i] = MI_TRUE;
    else
      pff_info->pbool[i] = MI_FALSE;
    break;
  case 1: /* boolean */
    if (buf[0] == 'f')
      pff_info->pdatum[i] = (MI_DATUM)0;
    else
      pff_info->pdatum[i] = (MI_DATUM)1;
    pff_info->pbool[i] = MI_FALSE;
    break;
  default: /* byte, text */
    mi_db_error_raise(NULL, MI_EXCEPTION,"ffutil.c:253: unsuppported type");
    return(MI_ERROR);
  } /* end switch */

  DPRINTF("ffvti", 80, ("Exiting ff_set_col()"));
  return(MI_OK);
}
/*----------------------------------------------------------------------*/
/* function similar to the standard fopen().  I felt I needed to 
 * provide buffering for the mi_file_read()
 */
FF_FILE *ff_fopen(mi_string *ppath, mi_char mode)
{
  FF_FILE *fd;

  DPRINTF("ffvti", 80, ("Entering ff_fopen()"));
  fd = (FF_FILE *)mi_dalloc(sizeof(FF_FILE), PER_STATEMENT);
  fd->_file = mi_file_open(ppath, mode, 0);
  if (fd->_file == MI_ERROR)
  {
    mi_free(fd);
  DPRINTF("ffvti", 80, ("Exiting ff_fopen() with NULL value"));
    return(NULL);
  }
  /* initialize the FF_FILE structure */
  fd->_base = mi_dalloc(BUFSIZE, PER_STATEMENT); /* buffer */
  fd->_cnt = 0; /* number of chars in buffer */
  fd->_flag = 0;

  DPRINTF("ffvti", 80, ("Exiting ff_fopen()"));
  return(fd);
}
/*----------------------------------------------------------------------*/
/* close a file and release the memory */
mi_integer ff_fclose(FF_FILE *fd)
{
  DPRINTF("ffvti", 80, ("Entering ff_fclose()"));
  mi_file_close(fd->_file);
  mi_free(fd->_base);
  mi_free(fd);
  DPRINTF("ffvti", 80, ("Exiting ff_fclose()"));
  return(0);
}
/*----------------------------------------------------------------------*/
/* set the file pointer to the beginning of the file */
mi_integer ff_rewind(FF_FILE *fd)
{
  mi_file_seek(fd->_file, 0, MI_LO_SEEK_SET);
  fd->_cnt = 0;
  return(0);
}
/*----------------------------------------------------------------------*/
/* function similar to the standard fgets().  This provides buffering
 * to the mi_file_read() function
 */
mi_string *ff_fgets(mi_string *s, mi_integer n, FF_FILE *stream)
{
  mi_integer ret, nbmv;

  DPRINTF("ffvti", 80, ("Entering ff_fgets()"));
  nbmv = 0; /* number of character moved to the buffer */

  while (1)
  {
    if ((stream->_cnt) <= 0)
    {
      /* read a buffer */
      if (stream->_flag == 0)
      {
        ret = mi_file_read(stream->_file, stream->_base, BUFSIZE - 1);
        if (ret == MI_ERROR)
          stream->_flag = 1;
          stream->_cnt = ret;
          stream->_ptr = stream->_base;
      }
    }
    if (stream->_cnt <= 0)
    {
      if ( nbmv == 0)
      {
        DPRINTF("ffvti", 80, ("Exiting ff_fgets() with NULL value"));
        return(NULL); /* nothing transferred */
      } else
      {
        s[nbmv] = 0;
        DPRINTF("ffvti", 80, ("Exiting ff_fgets()"));
        return(s);
      }
    }
    while ((stream->_cnt) > 0)
    {
      s[nbmv] = *stream->_ptr;
      stream->_ptr++;
      stream->_cnt--;
      if (s[nbmv] == '\n')
      {
        s[nbmv] = 0;
        DPRINTF("ffvti", 80, ("Exiting ff_fgets()"));
        return(s);
      }
      nbmv++;
      if (nbmv >= (n - 1))
      {
        s[nbmv] = 0;
        DPRINTF("ffvti", 80, ("Exiting ff_fgets()"));
        return(s);
      }
    }
  } /* end while(1) */
}
/*----------------------------------------------------------------------*/
/* this function just writes the string, no buffering */
mi_integer ff_fputs(const char *s, FF_FILE *stream)
{
  DPRINTF("ffvti", 80, ("Entering/Exiting ff_fputs()"));
  return(mi_file_write(stream->_file, s, strlen(s)));
}
/*----------------------------------------------------------------------*/
mi_integer set_tracing(mi_lvarchar *class, mi_integer lvl,
                       mi_lvarchar *tfile, MI_FPARAM *fparam)
{
  mi_integer   ret;
  mi_string *str, buffer[80];

  /* if there is a trace file provided */
  if (mi_fp_argisnull(fparam, 2) != MI_TRUE) {
    str = mi_lvarchar_to_string(tfile);
    ret = mi_tracefile_set(str);
  }
  /* if both the class and level are not NULL */
  if (mi_fp_argisnull(fparam, 0) != MI_TRUE &&
     (mi_fp_argisnull(fparam, 1)) != MI_TRUE) {
    str = mi_lvarchar_to_string(class);
    sprintf(buffer, "%s %d ", str, lvl);
    ret = mi_tracelevel_set(buffer);
  }
  return ret;
}
