/**************************************************************************
 *
 * aesop.ec
 *
 * This program demonstrates how to retrieve idn_mrLvarchar data.
 *
 * The following command will likely compile this source:
 *    esql -g -o aesop aesop.ec
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EXEC SQL include sqltypes;
EXEC SQL include exp_chk.ec;

/*
 * Main program
 *
 */

void main (int argc, char **argv)
{

    /* Declare local host variables */

    EXEC SQL BEGIN DECLARE SECTION;
    char          dbname[256];
    char          sql_stmt[200];
    char          re[256];
    varchar       title_val[51];
    lvarchar 'idn_mrlvarchar' *fable_ptr; -- naming the type is optional here
    EXEC SQL END DECLARE SECTION;

    /* Declare other local variables */
    int           rc;
    char         *fable_buf;
    int           fable_len;
    int           count=0;

    /* Check for the correct number of arguments entered. */

    if (argc != 3)
    {
        printf ("\nUsage: %s <database> <regular expression>\n", argv[0]);
        exit (1);
    }

    /* Define exception handling routine for EXEC SQL statements */

    EXEC SQL whenever sqlerror CALL ignore206;

    /* Connect to the database. */

    sprintf(dbname, "%s", argv[1]);
    EXEC SQL connect to :dbname;

    sprintf(re, "%s", argv[2]);

    /* Create the SELECT statement. 
     * Yeah, this is cheating. We should bind re as a host variable.
     */

    sprintf(sql_stmt,
            "SELECT title, fable "
            "FROM aesop "
            "WHERE regexp_match(lower(fable), '%s') "
            "ORDER BY title", re);

    /* Prepare the SELECT statement. */

    EXEC SQL prepare sel_stmt from :sql_stmt;

    /* Declare cursor for the SELECT statement. */

    EXEC SQL declare sel_curs cursor for sel_stmt;

    /* Open cursor for the SELECT statement. */

    EXEC SQL open sel_curs;

    /* Fetch each fable and display it. */

    while (1)
    {
        EXEC SQL fetch sel_curs into :title_val, :fable_ptr;

        if (sqlca.sqlcode == SQLNOTFOUND)
            break;

        printf("\nFetched row.\n\n");

        if ((fable_len = ifx_var_getlen(&fable_ptr)) == (int) NULL)
        {
            fprintf(stderr, "Error calling ifx_var_getlen.");
            exit(1);
        }

        if ((fable_buf = ifx_var_getdata(&fable_ptr)) == NULL)
        {
            fprintf(stderr, "Error calling ifx_var_getdata.");
            exit(1);
        }

        printf("%s\n", title_val);
        printf("%s\n", fable_buf);
        printf("(Fable length=%d bytes)\n\n", fable_len);

        if ((rc = ifx_var_dealloc(&fable_ptr)) < 0)
        {
            fprintf(stderr, "Error calling ifx_var_dealloc.");
            exit(1);
        }
        count++;
    }

    printf("\n\nFound %d fable(s) that matched regular expression '%s'.\n", 
         count, re);

    /* Close the result set cursor */

    EXEC SQL close sel_curs;

    /* Free resources associated with SELECT statement */

    EXEC SQL free sel_curs;
    EXEC SQL free sel_stmt;

    /* Disconnect from the database */

    EXEC SQL disconnect current;

    printf( "\nTest Complete\n");
}
