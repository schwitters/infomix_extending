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
 * Query a Guid type
 */
#include <stdio.h>
#include <string.h>

EXEC SQL INCLUDE sqlca.h;

typedef struct Guid {
  unsigned char values[16];
} GUID;

void String2Guid(GUID *val, char *str);
void Guid2String(GUID *x, char *buffer);

int main(int argc, char *argv[]) {
EXEC SQL BEGIN DECLARE SECTION;
  fixed binary 'guid' GUID guidVal;
  fixed binary 'guid' GUID id;
  char query[120];
EXEC SQL END DECLARE SECTION;
  GUID *pguid;
  char buffer[80];

EXEC SQL CONNECT TO 'jroy';
  testcode("connect");

  strcpy(query, "SELECT col1 FROM tab WHERE col1 > ? ");

EXEC SQL PREPARE myprep FROM :query;
EXEC SQL DECLARE mycur CURSOR FOR myprep;
  testcode("declare cursor") ;

  /* set parameter use guid value: "0ac07282-184e-4103-ad2a-ad8abdab8f0b" */
  memcpy(guidVal.values,
   "\x01\xc0\x72\x82\x18\x4e\x41\x03\xad\x2a\xad\x8a\xbd\xab\x8f\x0b", 16);

EXEC SQL OPEN mycur USING :guidVal;
  testcode("open cursor") ;
  for (;;) {
    fprintf(stderr, "about to fetch...\n");
    EXEC SQL FETCH mycur into :id;

    /* check error code, break if needed */
    if (SQLCODE != 0) {
      fprintf(stderr, "Fetch code: %d\n", SQLCODE);
      if (SQLCODE == 100)
       break; /* no more rows */
      testcode("fetch");
    }
    /* print data */
    Guid2String(&id, buffer);
      fprintf(stderr, "col1: %s\n", buffer);
  } /* end for */
  EXEC SQL CLOSE mycur;
  EXEC SQL FREE mycur;
  EXEC SQL DISCONNECT CURRENT;

  return(0);
}
/*---------------------------------------------------------*/
void Guid2String(GUID *x, char *buffer) {
  int i, pos;

  /* set parameter use guid value: "0ac07282-184e-4103-ad2a-ad8abdab8f0b" */
  sprintf(buffer,
    "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    x->values[0], x->values[2], x->values[3], x->values[4], x->values[5],
    x->values[6], x->values[7], x->values[8], x->values[9], x->values[10],
    x->values[11], x->values[12], x->values[13], x->values[14], x->values[15]);
  buffer[37] = 0;
  return;
}
/*---------------------------------------------------------*/
void String2Guid(GUID *val, char *str) {
  char *pval;
  int i, j;
  unsigned int a, b;

  pval = (unsigned char *)val->values;
  j = 0;
  for (i = 0; i < 36; i++) {
    if (str[i] == '-')
      continue;
    if (str[i] >= 'A' && str[i] <= 'Z')
      a = 10 + str[i] - 'A';
    if (str[i] >= 'a' && str[i] <= 'z')
      a = 10 + str[i] - 'a';
    if (str[i] >= '0' && str[i] <= '9')
      a = str[i] - '0';
    i += 1;
    if (str[i] >= 'A' && str[i] <= 'Z')
      b = 10 + str[i] - 'A';
    if (str[i] >= 'a' && str[i] <= 'z')
      b = 10 + str[i] - 'a';
    if (str[i] >= '0' && str[i] <= '9')
      b = str[i] - '0';
    pval[j++] = (unsigned char)((a << 4) + b);
  }
  return;
}
/*---------------------------------------------------------*/
int testcode(char *op)
{
EXEC SQL BEGIN DECLARE SECTION;
  int exception_count, messlen, i, j;
  char overflow[2], class_id[255], subclass_id[255], message[8192];
  char sqlstate_code[6];
EXEC SQL END DECLARE SECTION;

  if (SQLCODE < 0) {
    fprintf(stderr, "Error %s: %d\n", op, SQLCODE);
      EXEC SQL GET DIAGNOSTICS :exception_count = NUMBER,
                               :overflow = MORE;
      fprintf(stderr, "Exceptions: number: %d, overflow: %s\n",
              exception_count, overflow);
      for (i = 1; i <= exception_count; i++) {
        EXEC SQL GET DIAGNOSTICS EXCEPTION :i
             :sqlstate_code = RETURNED_SQLSTATE,
             :class_id = CLASS_ORIGIN, :subclass_id = SUBCLASS_ORIGIN,
             :message = MESSAGE_TEXT, :messlen = MESSAGE_LENGTH;
        fprintf(stderr, "Exception %d: SQLSTATE=%s\n", i, sqlstate_code);
        fprintf(stderr, "message length: %d\n", messlen);
        message[messlen] = 0;
        fprintf(stderr, "message text: %s\n", message);
        j = stleng(class_id);
        while ((class_id[j] == 0 || class_id[j] == ' '))
            j--;
        class_id[j + 1] = 0;
        j = stleng(subclass_id);
        while ((subclass_id[j] == 0 || subclass_id[j] == ' '))
            j--;
        subclass_id[j + 1] = 0;
        fprintf(stderr, "Class origin: %s, subclass origin: %s\n",
                class_id, subclass_id);
      } /* end for */
      exit(1);
  }

  return(0);
}
