/* Copyright(R) 1997 Informix Software. All rights reserved */
/************************************************************************
 *
 *  Title:       meps.c   // Mi_Exec_Prepared_Statement
 *                           -  -    -        -
 *  Description: Execute a prepared SQL statement with
 *               mi_exec_prepared_statement() to send an in-memory
 *               copy of an image file to a server function.
 *
 *  Assumes:
 *
 *       (1) User-defined type "BinaryType":
 *
 *              typedef struct
 *              {
 *                  mi_int1           data[1];
 *              } BinaryType;
 *
 ************************************************************************/

#include <stdio.h>
#include "mi.h"
#include "milib.h"
#include <fcntl.h>

#define db_name "test"
#define FNAME   "image.jpg"

mi_integer
main()
{
    MI_CONNECTION *conn;                     /* database connection */

    /* ------------------------------------------------------- */
    /* "cmd" is the SQL command that we'll send to the server. */
    /* Note that there are two parameters, identified by the   */
    /* question marks.                                         */
    /* ------------------------------------------------------- */
    mi_string     *cmd = "execute procedure WriteIt (?);";

    /* ------------------------------------------------------------ */
    /* mi_prepare() will build an MI_STATEMENT from the SQL command */
    /* ------------------------------------------------------------ */
    MI_STATEMENT  *stmt;

    /* ------------------------------------------------------------ */
    /* This structure describes the input column. It will be passed */
    /* to mi_exec_prepared_statement() in an mi_lvarchar, however.  */
    /* ------------------------------------------------------------ */
    typedef struct
    {
        mi_int1           data[1];
    } BinaryType;

    mi_integer   img_len;
    BinaryType	*input_value;
    mi_lvarchar *pass_value;
    mi_integer    fd;
    mi_integer    are_binary;
    mi_integer    n_params;
    mi_integer    nulls[1];
    /* ------------------------------------------------------------ */
    /* The one element of the "values" array will be set to point   */
    /* to the one parameter ("?") in the prepared statement.        */
    /* ------------------------------------------------------------ */
    MI_DATUM      values[1];
    mi_integer    lengths[1];
    /* ------------------------------------------------------------ */
    /* "types" identifies the IUS data type for each parameter      */
    /*                                                              */
    /* IMPORTANT: The server stores these names in *lower case*     */
    /* in the system catalogs, so you *must* use a lower case name  */
    /* here, even if you used mixed case in your "create type"      */
    /* ------------------------------------------------------------ */
    mi_string     *types[] = {"binarytype"};

    /* 
    ** Read in the local copy of image.jpg
    */
    fd = mi_file_open(FNAME, O_RDONLY, NULL);
    if (fd == NULL)
    {
        (void)fprintf(stderr, "Unable to open input file image.jpg\n");
        (void)fprintf(stderr, "Exiting.\n");
        return(-1);
    }

    /* Get the length of the file, then reposition to the beginning */
    img_len = mi_file_seek(fd, MI_LO_SEEK_SET, MI_LO_SEEK_END);
    (void)mi_file_seek(fd, 0, MI_LO_SEEK_SET);

    input_value = (BinaryType *)mi_alloc(img_len);

    /* set values */

    if (mi_file_read(fd, (char *)input_value, img_len) != img_len)
    {
        (void)fprintf(stderr, "Error when reading image.jpg\n");
        (void)fprintf(stderr, "Exiting.\n");
        return(-1);
    }

    /*
    ** We're going to encapsulate this in an mi_lvarchar for passing
    ** to the server.
    */
    pass_value = mi_new_var(img_len);
    mi_set_varptr(pass_value, (char *)input_value);

    values[0] = input_value;
        
    /* ------------------------------------------------------------ */
    /* The value of "1" means "send the parameters in binary mode". */
    /* For ascii mode, this variable would be set to "0".           */
    /* ------------------------------------------------------------ */
    are_binary = 1;  

    /* ------------------------------------------------------------ */
    /* n_params is the number of parameters in the SQL command.     */
    /* ------------------------------------------------------------ */
    n_params = 1;

    /* ------------------------------------------------------------ */
    /* The "lengths" array tells us how long each is.               */
    /* ------------------------------------------------------------ */
    lengths[0] = img_len;

    /* ------------------------------------------------------------ */
    /* If the    parameter was a NULL, its corresponding entry in   */
    /* the "nulls" array would be a "1". In this case, it is not    */
    /* NULL                                                         */
    /* ------------------------------------------------------------ */
    nulls[0] = 0;

    values[0] = pass_value;

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

    /*
    ** As with mi_exec(), mi_query_finish() completes the command. An   **
    ** mi_get_results() loop would also have this effect                **
    */
    (void)mi_query_finish(conn);

    (void)printf("Clean up...\n");
    if (mi_drop_prepared_statement(stmt) == MI_ERROR)
    {
        (void)fprintf(stderr, "mi_drop_prepared_statement failed.\n");
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
