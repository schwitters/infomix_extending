/*(c) Copyright IBM Corp. 2003  All rights reserved.                 */
/*                                                                   */
/*This sample program is owned by International Business Machines    */
/*Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
/*and licensed, not sold.                                            */
/*                                                                   */
/*You may copy, modify, and distribute this sample program in any    */
/*form without payment to IBM,  for any purpose including developing,*/
/*using, marketing or distributing programs that include or are      */
/*derivative works of the sample program.                            */
/*                                                                   */
/*The sample program is provided to you on an "AS IS" basis, without */
/*warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
/*WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
/*THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
/*ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
/*limitation of implied warranties, so the above limitations or      */
/*exclusions may not apply to you.  IBM shall not be liable for any  */
/*damages you suffer as a result of using, modifying or distributing */
/*the sample program or its derivatives.                             */
/*                                                                   */
/*Each copy of any portion of this sample program or any derivative  */
/*work,  must include a the above copyright notice and disclaimer of */
/*warranty.                                                          */
/*                                                                   */
/*********************************************************************/
/*
 * return XML rows or documents
 */

#include <string.h>
#include <mi.h>
#include <miami.h>
#include "genxml.h"

#define VERSIONSTRING "Version 1.01, June 2004"
#define BUFSIZE  2048
#define BIGBUFSIZE  32000

typedef struct aggrxml_t {
  mi_string name[30], buffer[1];
} AGGRXML_T;

mi_lvarchar *genxml2(mi_lvarchar *rowname, MI_ROW *arow, MI_FPARAM *fp);
/*------------------------------------------------------*/
mi_lvarchar *genxmlversion(MI_FPARAM *fp)
{
  return(mi_string_to_lvarchar(VERSIONSTRING));
}
/*------------------------------------------------------*/
mi_string *do_cast(MI_CONNECTION *conn, MI_DATUM *datum,
                   MI_TYPEID *tid, MI_TYPEID *lvar_id)
{
  MI_FUNC_DESC *fn;
  MI_FPARAM    *fp;
  MI_DATUM     new_datum;
  MI_TYPEID    *ftid;
  MI_TYPE_DESC  *td;
  mi_integer   i;
  mi_integer   ret;
  mi_char      status, *pbuf;

  fn = mi_cast_get(conn, tid, lvar_id,  &status);
  if (NULL == fn) {
    switch(status) {
    case MI_ERROR_CAST:
    case MI_NO_CAST:
    case MI_SYSTEM_CAST:
    case MI_UDR_CAST:
    case MI_IMPLICIT_CAST:
    case MI_EXPLICIT_CAST:
         return("error");
         break;
    case MI_NOP_CAST:
         return(mi_lvarchar_to_string((mi_lvarchar *)datum));
         break;
    } /* end switch */
  }
  fp = mi_fparam_get(conn, fn);
  for (i = 0; i < mi_fp_nargs(fp); i++)
    mi_fp_setargisnull(fp, i, MI_FALSE);
  /* in this case, we know it is int to lvarchar.  It is a system cast */
  /* that is done by dosyscast(datum *, int, int) */
  /* arguments 2 and 3 represent length and precision of the return value */
  new_datum = mi_routine_exec(conn, fn, &ret, datum, 30, 10, fp);
  pbuf = mi_lvarchar_to_string(new_datum);
  mi_routine_end(conn, fn);
  return(pbuf);
}
/*------------------------------------------------------*/
mi_string *do_collection(MI_CONNECTION *conn, mi_string *colName,
				MI_TYPEID *ctid, MI_COLLECTION *col) {
  MI_COLL_DESC  *pcoldesc;
  MI_TYPEID     *tid, *lvarTid;
  MI_TYPE_DESC  *td;
  MI_DATUM      datum2;
  mi_lvarchar   *lvarret;
  mi_string     typeName[256], *tname, *buffer, *pbuf2, *pcolname, *pcast;
  mi_integer    j, colType, len, pbufLen;


  DPRINTF("myclass", 10, ("entering do_collection()"));
  buffer = (char *)mi_alloc(BUFSIZE);
  if (0 == strlen(colName))
    colName = "collection";
  sprintf(buffer, "<%s>\n", colName);
  tname = mi_type_typename(mi_type_typedesc(conn, ctid));
  DPRINTF("myclass", 10, ("tname: %s", tname));
  if (tname[0] == 'M')
	tname += 9;
  else if (tname[0] = 'S')
		  tname += 4;
  else
		  tname += 5;
  strcpy(typeName, tname);
  typeName [strlen(typeName) - 10] = 0;
  
  DPRINTF("myclass", 10, ("typeName: %s", typeName));
  tid = mi_typename_to_id(conn, mi_string_to_lvarchar(typeName));
  lvarTid = mi_typename_to_id(conn, mi_string_to_lvarchar("lvarchar"));
  pbufLen = strlen(buffer);
  pcoldesc = mi_collection_open(conn, col);
  DPRINTF("myclass", 10, ("collection opened"));
  for (j = 1; MI_END_OF_DATA != (colType = 
			 mi_collection_fetch(NULL, pcoldesc, MI_CURSOR_NEXT,
			 0, (MI_DATUM *)&datum2, &len)); j++) {
    switch(colType) {
    case MI_NULL_VALUE:
         break;
    case MI_NORMAL_VALUE:
  DPRINTF("myclass", 10, ("Got a normal value"));
         if (BUFSIZE <= (pbufLen + 7) )
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         sprintf(&buffer[pbufLen], "<value>");
         pbufLen = strlen(buffer);
         pcast = do_cast(conn, datum2, tid, lvarTid);
         if (BUFSIZE <= ((pbufLen + 14 ) + strlen(pcast)))
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pcast);
         pbufLen = strlen(buffer);
         sprintf(&buffer[pbufLen], "</value>\n");
         pbufLen = strlen(buffer);
         break;
    case MI_COLLECTION_VALUE:
  DPRINTF("myclass", 10, ("Got a collection"));
		 pbuf2 = do_collection(conn, "collection", tid, (MI_COLLECTION *)datum2);
         if (BUFSIZE <= ((pbufLen + 24 ) + strlen(pbuf2)))
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pbuf2);
		 mi_free(pbuf2);
         pbufLen = strlen(buffer);
         break;
    case MI_ROW_VALUE: 
  DPRINTF("myclass", 10, ("Got a row"));
         lvarret = genxml2(mi_string_to_lvarchar("row"),
						 (MI_ROW *)datum2, NULL);
         pcast = mi_lvarchar_to_string(lvarret);
         if (BUFSIZE <= (pbufLen + strlen(pcast)) )
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pcast);
         pbufLen = strlen(buffer);
         break;
    } /* end switch */
  } /* end for each element in the collection */
  mi_collection_close(NULL, pcoldesc);
  sprintf(&buffer[pbufLen], "</%s>\n", colName);
  DPRINTF("myclass", 10, ("Exiting do_collection()"));
  return(buffer);
}
/*------------------------------------------------------*/
mi_lvarchar *genxml2(mi_lvarchar *rowname, MI_ROW *arow, MI_FPARAM *fp)
{
  MI_CONNECTION *conn;
  MI_TYPEID     *tid, *lvarTid;
  MI_TYPE_DESC  *td;
  MI_ROW_DESC   *rd;
  MI_DATUM      datum;
  mi_lvarchar   *lvarret;
  mi_integer    i, j, colType, colCount, collen;
  mi_string     *buffer, *pbuf2, *pcolname, *prowname, *pcast;
  mi_integer    pbufLen;

  buffer = (mi_string *)mi_alloc(BUFSIZE);
  conn = mi_open(NULL, NULL, NULL);
  rd = mi_get_row_desc(arow);
  colCount = mi_column_count(rd);
  strcpy(buffer, HEADER);
  pbufLen = strlen(buffer);
  tid = mi_rowdesc_typeid(rd);
  lvarTid = mi_typename_to_id(conn, mi_string_to_lvarchar("lvarchar"));
  td = mi_type_typedesc(NULL, tid);
  prowname = mi_lvarchar_to_string(rowname);
  sprintf(buffer, "<%s>\n", prowname);
  pbufLen = strlen(buffer);

  /* Process each column */
  for (i = 0; i < colCount; i++) {
    /* get column name and type id */
    pcolname = mi_column_name(rd, i);
    tid = mi_column_type_id(rd, i);
    switch(mi_value(arow, i, &datum, &collen)) {
    /* we should do this test */
    case MI_NULL_VALUE:
         break;
    case MI_NORMAL_VALUE:
         if (BUFSIZE <= (pbufLen + 2 + strlen(pcolname)) )
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         sprintf(&buffer[pbufLen], "<%s>", pcolname);
         pbufLen = strlen(buffer);
         pcast = do_cast(conn, datum, tid, lvarTid);
         if (BUFSIZE <= (pbufLen + 4 + (2 * strlen(pcolname)) + strlen(pcast)))
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pcast);
         pbufLen = strlen(buffer);
         sprintf(&buffer[pbufLen], "</%s>\n", pcolname);
         pbufLen = strlen(buffer);
         break;
    case MI_COLLECTION_VALUE:
		 pbuf2 = do_collection(conn, pcolname, tid, (MI_COLLECTION *)datum);
         if (BUFSIZE <= (pbufLen + 4 + (2 * strlen(pcolname)) + strlen(pbuf2)))
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pbuf2);
		 mi_free(pbuf2);
         pbufLen = strlen(buffer);
         break;
    case MI_ROW_VALUE:
         lvarret = genxml2(mi_string_to_lvarchar(pcolname), (MI_ROW *)datum, fp);
         pcast = mi_lvarchar_to_string(lvarret);
         if (BUFSIZE <= (pbufLen + strlen(pcast)) )
           mi_db_error_raise(NULL, MI_EXCEPTION,
            "XML row representation exceeds buffer size", NULL); 
         strcat(buffer, pcast);
         pbufLen = strlen(buffer);
         break;
    } /* end switch */
  } /* end for */
  if (BUFSIZE <= (pbufLen + 3 + strlen(pcolname)) )
    mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML row representation exceeds buffer size", NULL); 
  sprintf(&buffer[pbufLen], "</%s>\n", prowname);
  mi_close(conn);
  return(mi_string_to_lvarchar(buffer));
}
/*------------------------------------------------------*/
mi_lvarchar *genxmlnamedrow(MI_ROW *arow, MI_FPARAM *fp)
{
  MI_TYPEID *tid;
  MI_TYPE_DESC *td;
  mi_lvarchar *name;
  mi_string *prowname;

  tid = mi_fp_argtype(fp, 0);
  td = mi_type_typedesc(NULL, tid);
  prowname = mi_type_typename(td);
  name = mi_string_to_lvarchar(prowname);
  return(genxml2(name, arow, fp));
}
/*------------------------------------------------------*/
void genhdr(MI_CONNECTION *conn, char *name, char *retval)
{
  MI_ROW *row;
  mi_integer ret, collen;
  mi_string *dtdpath, *xslpath, buffer[BUFSIZE];

  strcpy(retval, HEADER);
  sprintf(buffer, "SELECT %s, %s FROM %s WHERE %s = '%s'",
          GENXMLCOL2, GENXMLCOL3, GENXMLDTDTAB, GENXMLCOL1, name);
  ret = mi_exec(conn, buffer, MI_QUERY_NORMAL);
  if (ret != MI_OK)
   mi_db_error_raise(NULL, MI_EXCEPTION,
       "Unable to execute SQL to det the DTD path", NULL); 
  ret = mi_get_result(conn);
  if (NULL != (row = mi_next_row(conn, &ret)) ) {
    ret = mi_value(row, 0, (MI_DATUM *)&dtdpath, &collen);
    if (ret == MI_NORMAL_VALUE)
      sprintf(&retval[strlen(retval)], DOCTYPE, name, dtdpath);
    ret = mi_value(row, 1, (MI_DATUM *)&xslpath, &collen);
    if (ret == MI_NORMAL_VALUE)
      sprintf(&retval[strlen(retval)], STYLESHEET, xslpath);
  }
  sprintf(&retval[strlen(retval)], "<%s>\n", name);

  return;
}
/*------------------------------------------------------*/
mi_lvarchar *addxmlhdr(mi_lvarchar *name, mi_lvarchar *stmt, MI_FPARAM *fp)
{
  MI_CONNECTION *conn;
  MI_ROW *row;
  mi_lvarchar *retlvar;
  mi_string *retval, *pstmt, *pdatum;
  mi_integer ret, collen, retlen;
  mi_string *pname, *dtdpath, *xslpath, buffer[BUFSIZE];

  /* Get the XML DTD path */
  conn = mi_open(NULL, NULL, NULL);
  retval = (char *)mi_alloc(BIGBUFSIZE);
  pname = mi_lvarchar_to_string(name);
  genhdr(conn, pname, retval);
  pstmt = mi_lvarchar_to_string(stmt);
  ret = mi_exec(conn, pstmt, MI_QUERY_NORMAL);
  ret = mi_get_result(conn);
  while (NULL != (row = mi_next_row(conn, &ret)) ) {
    mi_value(row, 0, (MI_DATUM *)&pdatum, &ret);
    /* checking for buffer overflow */
    retlen = strlen(retval);
    if (BIGBUFSIZE <= (retlen + strlen(pdatum)) )
      mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
    strcat(retval, pdatum);
  }
  retlen = strlen(retval);
  if (BIGBUFSIZE <= (retlen + 3 + strlen(pname)) )
    mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
  sprintf(&retval[retlen], "</%s>\n", pname);
  retlvar = mi_string_to_lvarchar(retval); 
  mi_free(retval);
  mi_close(conn);
  return(retlvar); 
}
/*------------------------------------------------------*/
mi_lvarchar *genxmlsql(mi_integer header, mi_lvarchar *name,
                       mi_lvarchar *stmt, MI_FPARAM *fp)
{
  MI_CONNECTION *conn;
  MI_ROW *row;
  mi_lvarchar *retlvar, *rowname;
  mi_string *retval, *pstmt, *pdatum;
  mi_integer ret, collen;
  mi_string *pname, *pxml, buffer[BUFSIZE];

  /* Get the XML DTD path */
  conn = mi_open(NULL, NULL, NULL);
  retval = (char *)mi_alloc(BIGBUFSIZE);
  pname = mi_lvarchar_to_string(name);
  if (header != 0)
    genhdr(conn, pname, retval);
  else
    sprintf(retval, "<%s>", pname);
  pstmt = mi_lvarchar_to_string(stmt);
  ret = mi_exec(conn, pstmt, MI_QUERY_BINARY);
  ret = mi_get_result(conn);
  rowname = mi_string_to_lvarchar("row");
  while (NULL != (row = mi_next_row(conn, &ret)) ) {
    retlvar = genxml2(rowname, row, fp);
    pxml = mi_lvarchar_to_string(retlvar);
    /* Check for buffer overflow */
    if (BIGBUFSIZE <= (strlen(retval) + strlen(pxml)) )
      mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
    strcat(retval, pxml);
    mi_var_free(retlvar);
  }
  if (BIGBUFSIZE <= (strlen(retval) + 3 + strlen(pname)) )
      mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
  sprintf(&retval[strlen(retval)], "</%s>\n", pname);
  retlvar = mi_string_to_lvarchar(retval); 
  mi_free(retval);
  mi_close(conn);
  return(retlvar); 
}
/*------------------------------------------------------*/
mi_lvarchar *genxmlhdr(mi_lvarchar *name, mi_lvarchar *stmt, MI_FPARAM *fp)
{
  return(genxmlsql(1, name, stmt, fp)); 
}
/*------------------------------------------------------*/
mi_lvarchar *genxmlnhdr(mi_lvarchar *name, mi_lvarchar *stmt, MI_FPARAM *fp)
{
  return(genxmlsql(0, name, stmt, fp)); 
}
/*------------------------------------------------------*/
AGGRXML_T *init_aggrxml(MI_ROW *arow, mi_lvarchar *name, MI_FPARAM *fp)
{
  MI_CONNECTION *conn;
  MI_ROW *row;
  AGGRXML_T *retval;
  mi_integer ret, collen;
  mi_string *pname, *dtdpath, *xslpath, buffer[BUFSIZE];

  /* Get the XML DTD path */
  conn = mi_open(NULL, NULL, NULL);
  retval = (AGGRXML_T *)mi_dalloc(BIGBUFSIZE, PER_COMMAND);
  pname = mi_lvarchar_to_string(name);
  strcpy(retval->name, pname);
  genhdr(conn, pname, retval->buffer);
  mi_close(conn);
  return(retval); 
}
/*------------------------------------------------------*/
AGGRXML_T *iter_aggrxml(AGGRXML_T *state, MI_ROW *arow, MI_FPARAM *fp)
{
  mi_lvarchar *rowname, *xml;
  mi_string *pbuf;

  rowname = mi_string_to_lvarchar("row");
  xml = genxml2(rowname, arow, fp);
  pbuf = mi_lvarchar_to_string(xml);
  if (BIGBUFSIZE <= (strlen(pbuf) + strlen(state->buffer) + sizeof(AGGRXML_T)) )
    mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
  strcat(state->buffer, pbuf);
  return(state);
}
/*------------------------------------------------------*/
/* should never be called since it is non parallel */
AGGRXML_T *comb_aggrxml(AGGRXML_T *state1,
                        AGGRXML_T *state2, MI_FPARAM *fp)
{
   mi_db_error_raise(NULL, MI_EXCEPTION,
       "The combine function should never be called", NULL); 
   return(state1);
}
/*------------------------------------------------------*/
mi_lvarchar *final_aggrxml(AGGRXML_T *state, MI_FPARAM *fp)
{
  mi_integer len;

  len = strlen(state->buffer);
  if (BIGBUFSIZE <=
    (3 + strlen(state->name) + strlen(state->buffer) + sizeof(AGGRXML_T)) )
     mi_db_error_raise(NULL, MI_EXCEPTION,
      "XML document exceeds the 30KB limit", NULL); 
  sprintf(&state->buffer[len], "</%s>", state->name);
  state->buffer[BIGBUFSIZE - 1] = 0;
  return(mi_string_to_lvarchar(state->buffer));
}
/*------------------------------------------------------*/
mi_lvarchar *castgenxml_t_lvar(mi_lvarchar *lvar, MI_FPARAM *fp)
{
  return(lvar);
}
/*------------------------------------------------------*/
/* Generate a simple xsl based on the row given */
mi_lvarchar *genxsl(mi_lvarchar *setname, mi_lvarchar *rowname,
				    MI_ROW *arow, MI_FPARAM *fp)
{
  MI_ROW_DESC   *rd;
  mi_integer    i, colCount;
  mi_string     *pcolname, *prowname;
  mi_string     buffer[BUFSIZE];

  rd = mi_get_row_desc(arow);
  colCount = mi_column_count(rd);
  prowname = mi_lvarchar_to_string(rowname);

  strcpy(buffer, XSLHEADER1);
  strcat(buffer, XSLHEADER2);
  strcat(buffer, XSLHEADER3);
  strcat(buffer, XSLHEADER4);
  strcat(buffer, XSLHEADER5);

  /* Generate header information */
  strcat(buffer, "<tr>\n");
  for (i = 0; i < colCount; i++) {
    /* get column name */
    pcolname = mi_column_name(rd, i);
	strcat(buffer, "<th>");
	strcat(buffer, pcolname);
	strcat(buffer, "</th>\n");
  }
  strcat(buffer, "</tr>\n");
  /* Generate the xsl for-each header */
  strcat(buffer, "<xsl:for-each select=\"");
  strcat(buffer, mi_lvarchar_to_string(setname));
  strcat(buffer, "/row\">\n");
  /* for each column */
  strcat(buffer, "<tr>\n");
  for (i = 0; i < colCount; i++) {
    /* get column name */
    pcolname = mi_column_name(rd, i);
    strcat(buffer, "<td>\n<xsl:value-of select=\"");
	strcat(buffer, pcolname);
	strcat(buffer, "\" />\n</td>\n");
  }
  strcat(buffer, "</tr>\n</xsl:for-each>\n");
  /* Write the end of the definition */
  strcat(buffer, XSLEND);

  return(mi_string_to_lvarchar(buffer));
}
/*------------------------------------------------------*/
mi_string *do_element(MI_CONNECTION *conn,
				mi_string *colname, MI_TYPEID *tid)
{
  MI_TYPEID     *ctid;
  MI_TYPE_DESC  *td;
  MI_ROW_DESC   *rd;
  mi_integer     i, len, colCount;
  mi_string     *buffer, *pbuf, *tname, typeName[256], *pcolname;

  buffer = (mi_string *)mi_alloc(2048);
  buffer[0] = 0;
  if (MI_TRUE == mi_typeid_is_collection(tid)) {
    tname = mi_type_typename(mi_type_typedesc(conn, tid));
    if (tname[0] == 'M')
	  tname += 9;
    else if (tname[0] = 'S')
		    tname += 4;
    else
		    tname += 5;
    strcpy(typeName, tname);
    typeName [strlen(typeName) - 10] = 0;
    ctid = mi_typename_to_id(conn, mi_string_to_lvarchar(typeName));
    if (MI_TRUE == mi_typeid_is_collection(ctid)) {
	  pbuf = do_element(conn, "collection", ctid);
    } else {
	  if (MI_TRUE == mi_typeid_is_row(ctid)) {
		 sprintf(buffer, 
		  "<!ELEMENT %s (row)* >\n", colname);
	     pbuf = do_element(conn, "row", ctid);
	  } else {
		pbuf = (mi_string *)mi_alloc(2048);
		sprintf(pbuf,
		  "<!ELEMENT %s (value)* >\n<!ELEMENT value (#PCDATA)>\n", colname);
	  }
	}
	strcat(buffer, pbuf);
	mi_free(pbuf);
	return(buffer);
  } /* end if it is a collection */

  if (MI_TRUE == mi_typeid_is_row(tid)) {
	td = mi_type_typedesc(conn, tid);
	rd = mi_get_row_desc_from_type_desc(td);
    colCount = mi_column_count(rd);
    sprintf(buffer, "<!ELEMENT %s (", colname);
    for (i = 0; i < colCount; i++) {
      /* get column name */
      pcolname = mi_column_name(rd, i);
	  if (i != 0)
	    strcat(buffer, ", ");
	  strcat(buffer, pcolname);
    }
    strcat(buffer, ")>\n");
    for (i = 0; i < colCount; i++) {
      /* get column name */
      pcolname = mi_column_name(rd, i);
	  tid = mi_column_type_id(rd, i);
	  pbuf = do_element(conn, pcolname, tid);
	  strcat(buffer, pbuf);
	  mi_free(pbuf);
    }
	return(buffer);
  } /* end if it is a row */
  sprintf(buffer, "<!ELEMENT %s (#PCDATA)>\n", colname);
  return(buffer);
}
/*------------------------------------------------------*/
/* Generate a simple dtd based on the row given */
/* does not look for key and required fields */
mi_lvarchar *gendtd(mi_lvarchar *setname, mi_lvarchar *rowname,
				    MI_ROW *arow, MI_FPARAM *fp)
{
  MI_CONNECTION *conn;
  MI_ROW_DESC   *rd;
  MI_TYPEID     *tid;
  mi_integer    i, colCount, len;
  mi_string     *pcolname, *psetname, *prowname;
  mi_string buffer[BUFSIZE], *pbuf;

  conn = mi_open(NULL, NULL, NULL);
  rd = mi_get_row_desc(arow);
  colCount = mi_column_count(rd);
  prowname = mi_lvarchar_to_string(rowname);
  psetname = mi_lvarchar_to_string(setname);

#if 0
  strcpy(buffer, HEADER);
  len = strlen(buffer);
#else
  len = 0;
#endif

  /* Generate header information */
  sprintf(&buffer[len], "<!ELEMENT %s (", psetname);
  for (i = 0; i < colCount; i++) {
    /* get column name */
    pcolname = mi_column_name(rd, i);
	if (i != 0)
	  strcat(buffer, ", ");
	strcat(buffer, pcolname);
  }
  strcat(buffer, ")>\n");
  /* for each column */
  for (i = 0; i < colCount; i++) {
    /* get column name */
    pcolname = mi_column_name(rd, i);
	tid = mi_column_type_id(rd, i);
	pbuf = do_element(conn, pcolname, tid);
	strcat(buffer, pbuf);
	mi_free(pbuf);
  }
  mi_close(conn);
  return(mi_string_to_lvarchar(buffer));
}
/*------------------------------------------------------*/
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
