/* Copyright(R) 1997 Informix Software. All rights reserved */
/************************************************************************
 *
 *  Title:       meps.c   // Mi_Exec_Prepared_Statement
 *                           -  -    -        -
 *  Description: Execute a prepared SQL statement with
 *               mi_exec_prepared_statement() to do a binary
 *               insert into a table.
 *
 *  Assumes:
 *
 *       (1) User-defined type "FIXType":
 *
 *              typedef struct
 *              {
 *                  mi_integer        col1;
 *                  mi_char           col2[10];
 *              } FIXType;
 *
 *       (2) database "testdb" and table "foo":
 * 
 *           "create table foo (col1 FIXType);"
 *
 ************************************************************************/

#include <stdio.h>
#include "mi.h"

#define db_name "testdb"

mi_integer
main()
{
    MI_CONNECTION *conn;                     /* database connection */

    /* ------------------------------------------------------- */
    /* "cmd" is the SQL command that we'll send to the server. */
    /* Note that there are two parameters, identified by the   */
    /* question marks.                                         */
    /* ------------------------------------------------------- */
    mi_string     *cmd = "insert into foo values (?);";

    /* ------------------------------------------------------------ */
    /* mi_prepare() will build an MI_STATEMENT from the SQL command */
    /* ------------------------------------------------------------ */
    MI_STATEMENT  *stmt;

    /* ------------------------------------------------------------ */
    /* The parameter will be replaced by a pointer to the following */
    /* structure                                                    */
    /* ------------------------------------------------------------ */
    typedef struct
    {
        mi_integer        col1;
        mi_char           col2[10];
    } FIXType;

    FIXType	input_column;

    /* ------------------------------------------------------------ */
    /* The value of "1" means "send the parameters in binary mode". */
    /* For ascii mode, this variable would be set to "0".           */
    /* ------------------------------------------------------------ */
    mi_integer    are_binary = 1;  

    /* ------------------------------------------------------------ */
    /* n_params is the number of parameters in the SQL command.     */
    /* ------------------------------------------------------------ */
    mi_integer    n_params = 1;

    /* ------------------------------------------------------------ */
    /* The one element of the "values" array will be set to point   */
    /* to the one parameter.                                        */
    /* ------------------------------------------------------------ */
    MI_DATUM      values[1];

    /* ------------------------------------------------------------ */
    /* The "lengths" array tells us how long each is.               */
    /* ------------------------------------------------------------ */
    mi_integer    lengths[1] = {16}; /* the data type's internallength */

    /* ------------------------------------------------------------ */
    /* If the    parameter was a NULL, its corresponding entry in   */
    /* the "nulls" array would be a "1". In this case, it is not    */
    /* NULL                                                         */
    /* ------------------------------------------------------------ */
    mi_integer    nulls[1] = {0};

    /* ------------------------------------------------------------ */
    /* "types" identifies the IUS data type for each parameter      */
    /*                                                              */
    /* IMPORTANT: The server stores these names in *lower case*     */
    /* in the system catalogs, so you *must* use a lower case name  */
    /* here, even if you used mixed case in your "create type"      */
    /* ------------------------------------------------------------ */
    mi_string     *types[] = {"fixtype"};

    /* set up the column value to insert */
    input_column.col1 = 1234;
    (void) strcpy(input_column.col2, "abcdefghij");

    values[0] = &input_column;

    (void)printf("Open a connection...\n");
    if ((conn = mi_open(db_name, NULL, NULL)) == NULL) 
    {
        (void)fprintf(stderr, "Failed to connect.\n");
        return(-1);
    }

    (void)printf("Call mi_prepare...\n");
    if ((stmt = mi_prepare(conn, cmd, NULL)) == NULL)
    {
        (void)fprintf(stderr, "mi_prepare failed.\n");
        return(-1);
    }

    (void)printf("Call mi_exec_prepared_statement...\n");
    if (mi_exec_prepared_statement(stmt, 
                                   MI_SEND_READ, 
                                   are_binary, 
                                   n_params,
                                   values, 
                                   lengths, 
                                   nulls, 
                                   types, 
                                   0, 
                                   0) != MI_OK)
    {
        (void)fprintf(stderr, "mi_exec_prepared_statement failed.\n");
        return(-1);
    }

    (void)printf("Complete the operation with mi_query_finish()\n");
    if (mi_query_finish(conn) == MI_ERROR)
    {
      (void)printf("Error from mi_query_finish()! \n");
    }

    (void)printf("Call mi_drop_prepared_statement...\n");
    if (mi_drop_prepared_statement(stmt) == MI_ERROR)
    {
        (void)fprintf(stderr, "mi_drop_prepared_statement failed.\n");
        return(-1);
    }

    (void)printf("Close the connection...\n");
    if (mi_close(conn) == MI_ERROR)
    {
        (void)fprintf(stderr, "mi_close failed.\n");
        return(-1);
    }

    printf("Program completed successfully.\n");
    return(0);
}
