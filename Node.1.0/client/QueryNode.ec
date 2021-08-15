/*
 * Query a Node type
 */
#include <stdio.h>
#include <string.h>

EXEC SQL INCLUDE sqlca.h;

typedef struct Node {
  int len;
  unsigned int values[1];
} NODE;

int main(int argc, char *argv[]) {
  NODE *pnode;
  char buffer[80];
EXEC SQL BEGIN DECLARE SECTION;
  var binary 'node' nodeVal;
  var binary 'node' id;
  char name[31], query[120];
EXEC SQL END DECLARE SECTION;

EXEC SQL CONNECT TO 'jroy';
  testcode("connect");

  strcpy(query, "SELECT * FROM employee2 ");
  strcat(query, "WHERE isAncestor(Employee_Id, ? ) ");
  strcat(query, "ORDER BY Employee_Id DESC");

EXEC SQL PREPARE myprep FROM :query;
EXEC SQL DECLARE mycur CURSOR FOR myprep;
  testcode("declare cursor") ;

  /* set parameter use node value: 1.7.43.256.1537 */
  ifx_var_alloc(&nodeVal, 24);
  pnode = (NODE *)ifx_var_getdata(&nodeVal);
  pnode->len = 5;
  pnode->values[0] = 1;
  pnode->values[1] = 7;
  pnode->values[2] = 43;
  pnode->values[3] = 256;
  pnode->values[4] = 1537;

EXEC SQL OPEN mycur USING :nodeVal;
  testcode("open cursor") ;
  for (;;) {
    fprintf(stderr, "about to fetch...\n");
    EXEC SQL FETCH mycur into :id, :name;

    /* check error code, break if needed */
    if (SQLCODE != 0) {
      fprintf(stderr, "Fetch code: %d\n", SQLCODE);
      if (SQLCODE == 100)
       break; /* no more rows */
      testcode("fetch");
    }
    /* print data */
    pnode = (NODE *)ifx_var_getdata(&id);
    (void)Node2String(pnode, buffer);
      fprintf(stderr, "id: %s, name: %s\n", buffer, name);
  } /* end for */
  EXEC SQL CLOSE mycur;
  EXEC SQL FREE mycur;
  EXEC SQL DISCONNECT CURRENT;

  return(0);
}
/*---------------------------------------------------------*/
int Node2String(NODE *x, char *buffer) {
  int i, pos;

  pos = 0;
  for (i = 0; i < x->len; i++) {
    sprintf(buffer+pos, "%d.", x->values[i]);
    pos = strlen(buffer);
  }
  buffer[pos - 1] = 0;
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
