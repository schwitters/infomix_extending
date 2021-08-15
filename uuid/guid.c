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
 * GUID opaque type implementation
 *
 * typedef unsigned char uuid_t[16];
 */
#include <mi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <uuid/uuid.h>
#endif

#define GUID_SIZE 16
typedef struct {
  unsigned char val[16];
} GUID;

GUID *guid_in(mi_lvarchar *in, MI_FPARAM *fp) {
  GUID *val;
  char *str, *pval, buffer[80];
  int i, j;
  unsigned int a, b;

  val = (GUID *)mi_alloc(sizeof(GUID));
  pval = (unsigned char *)val;
  str = mi_lvarchar_to_string(in);
  j = 0;
  for (i = 0; i < 36; i++) {
	switch(i) {
	  case 8:
	  case 13:
	  case 18:
	  case 23:
	    if (str[i] != '-') {
		  sprintf(buffer, "Guid input format error, position %d", i);
		  mi_db_error_raise(NULL, MI_EXCEPTION, buffer, NULL);
		}
		i++; /* fall into the default for the next character */
	  default:
        if (str[i] >= '0' && str[i] <= '9')
          a = str[i] - '0';
		else {
		  str[i] &= 0x5F;
          if (str[i] >= 'A' && str[i] <= 'F')
            a = 10 + str[i] - 'A';
		  else {
		    sprintf(buffer, "Guid input format error, position %d", i);
		    mi_db_error_raise(NULL, MI_EXCEPTION, buffer, NULL);
		  }
		}
		i++;
        if (str[i] >= '0' && str[i] <= '9')
          b = str[i] - '0';
		else {
		  str[i] &= 0x5F; /* make sure it is uppercase */
          if (str[i] >= 'A' && str[i] <= 'F')
            b = 10 + str[i] - 'A';
		  else {
		    sprintf(buffer, "Guid input format error, position %d", i);
		    mi_db_error_raise(NULL, MI_EXCEPTION, buffer, NULL);
		  }
		}
        pval[j++] = (unsigned char)((a << 4) + b);
    } /* end switch */
  } /* end for */
  return(val);
}
mi_lvarchar *guid_out(GUID *in, MI_FPARAM *fp) {
  mi_string buffer[37];
  unsigned char *pval;
  unsigned int a;
  mi_integer i, j;

  pval = &in->val[0];
  sprintf(buffer,
   "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
   pval[0], pval[1], pval[2], pval[3], pval[4], pval[5], pval[6], pval[7],
   pval[8], pval[9], pval[10], pval[11], pval[12], pval[13], pval[14],
   pval[15] );
  buffer[36] = 0;
  return(mi_string_to_lvarchar(buffer));
}
mi_sendrecv *guid_send(GUID *in, MI_FPARAM *fp) {
  mi_lvarchar *ret;
  unsigned char *pret;

  ret = mi_new_var(sizeof(GUID));
  pret = (unsigned  char *)mi_get_vardata(ret);
  memcpy(pret, in->val, 16);
  return((mi_sendrecv *)ret);
}
GUID * guid_recv(mi_sendrecv *in, MI_FPARAM *fp) {
  GUID * ret;
  unsigned char *ppin;

  ret = (GUID *)mi_alloc(sizeof(GUID));
  ppin = (unsigned char *)mi_get_vardata(in);
  memcpy(ret->val, ppin, 16);
  return(ret);
}
/*
 * Add 2 possible send/receive functions to use the character
 * representation
 * This requires changes to guid.sql
 */
#if 0
mi_sendrecv *guid_send_char(GUID *in, MI_FPARAM *fp) {
  return((mi_sendrecv *)guid_out(in, fp));
}
GUID *guid_recv_char(mi_sendrecv *in, MI_FPARAM *fp) {
  return(guid_in((mi_lvarchar *)in, fp));
}
#endif
/*----------------------------------------------------------*/
mi_integer compare(GUID *a, GUID *b, MI_FPARAM *fp) {
  int i;
  for (i = 0; i < 16; i++) {
    if  (a->val[i] < b->val[i])
      return(-1);
    if (a->val[i] > b->val[i])
      return(1);
  }
  return(0);
}
mi_boolean lessthanorequal(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (0 >= compare(a, b, fp) )
    return(1);
  return(0);
}
mi_boolean lessthan(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (-1 == compare(a, b, fp) )
    return(1);
  return(0);
}
mi_boolean equal(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (0 == compare(a, b, fp) )
    return(1);
  return(0);
}
mi_boolean notequal(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (0 != compare(a, b, fp) )
    return(1);
  return(0);
}
mi_boolean greaterthanorequal(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (0 <= compare(a, b, fp) )
    return(1);
  return(0);
}
mi_boolean greaterthan(GUID *a, GUID *b, MI_FPARAM *fp) {
  if (1 == compare(a, b, fp) )
    return(1);
  return(0);
}
GUID *makeguid(MI_FPARAM *fp)
{
#ifndef WIN32
  GUID *val;
  uuid_t guid;

  uuid_generate(guid);
  val = (GUID *)mi_alloc(sizeof(GUID));
  memcpy((void *)val, (void *)guid, 16);
  return(val);
#else
  mi_db_error_raise(NULL, MI_EXCEPTION, "makeguid() Not implemented", NULL);
  return(NULL);
#endif
}

