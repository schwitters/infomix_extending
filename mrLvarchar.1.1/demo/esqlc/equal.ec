/**************************************************************************
 *
 * equal.ec
 *
 * This program demonstrates how to create an idn_mrLvarchar host
 * variable to use in a WHERE clause.
 *
 * The following command will likely compile this source:
 *    esql -g -o equal equal.ec
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
    int           i, n, xid;
    short         typ;
    char          dbname[256];
    char          sql_stmt[200];
    varchar       title_val[51];
    lvarchar      *fable_var; 
    EXEC SQL END DECLARE SECTION;

    /* Declare other local variables */

    int           rc,
                  count=0,
                  fable_len=0;
    char         *fable_buf;

    /* Check for the correct number of arguments entered. */
    if (argc != 2)
    {
        printf ("\nUsage: %s <database>\n", argv[0]);
        exit (1);
    }

    /* Define exception handling routine for EXEC SQL statements */

    EXEC SQL whenever sqlerror CALL ignore206;

    /* Connect to the database. */

    sprintf(dbname, "%s", argv[1]);
    EXEC SQL connect to :dbname;

    /* Point the fable to a constant in memory */

    fable_buf="A MOUSE who always lived on the land, by an unlucky chance formed an intimate acquaintance with a Frog, who lived for the most part in the water.  The Frog, one day intent on mischief, bound the foot of the Mouse tightly to his own.  Thus joined together, the Frog first of all led his friend the Mouse to the meadow where they were accustomed to find their food.  After this, he gradually led him towards the pool in which he lived, until reaching the very brink, he suddenly jumped in, dragging the Mouse with him.  The Frog enjoyed the water amazingly, and swam croaking about, as if he had done a good deed.  The unhappy Mouse was soon suffocated by the water, and his dead body floated about on the surface, tied to the foot of the Frog.  A Hawk observed it, and, pouncing upon it with his talons, carried it aloft.  The Frog, being still fastened to the leg of the Mouse, was also carried off a prisoner, and was eaten by the Hawk.  Harm hatch, harm catch.";

    fable_len = strlen(fable_buf);

    /* Create host variable for idn_mrLvarchar representation */

    if ((rc = ifx_var_alloc(&fable_var, fable_len)) < 0)
    {
        fprintf(stderr, "Error calling ifx_var_alloc.");
        exit(1);
    }

    if ((rc = ifx_var_setdata(&fable_var, fable_buf, fable_len)) < 0)
    {
        fprintf(stderr, "Error calling ifx_var_setdata.");
        exit(1);
    }

    if ((rc = ifx_var_setlen(&fable_var, fable_len)) < 0)
    {
        fprintf(stderr, "Error calling ifx_var_setlen.");
        exit(1);
    }

    /* Create the SELECT statement. */

    sprintf(sql_stmt, "SELECT title from aesop WHERE fable = ?");

    /* Prepare the SELECT statement. */

    EXEC SQL prepare sel_stmt from :sql_stmt;

    /* Declare cursor for the SELECT statement. */

    EXEC SQL declare sel_curs cursor for sel_stmt;

    /* Allocate a statement descriptor. */

    EXEC SQL allocate descriptor 'sel_desc';

    /* Set the number of input parameters */

    n = 1;
    EXEC SQL set descriptor 'sel_desc' COUNT = :n;

    /* Bind the query idn_mrLvarchar to the input parameter */

    typ = SQLUDTVAR;
    xid = XID_LVARCHAR;

    EXEC SQL set descriptor 'sel_desc' VALUE :n
        TYPE = :typ,
        EXTYPEID = :xid,
        DATA = :fable_var;

    /* Open cursor for the SELECT statement. */

    EXEC SQL open sel_curs using sql descriptor 'sel_desc';

    /* Fetch the title for this fable. */

    while (1)
    {
        EXEC SQL fetch sel_curs into :title_val;

        if (sqlca.sqlcode == SQLNOTFOUND)
            break;

        printf("Title=%s\n", title_val);
        count++;
    }

    printf("Found %d titles that match the fable.", count);

    /* Close the result set cursor */

    EXEC SQL close sel_curs;

    /* Deallocate host variable for idn_mrLvarchar representation */

    if ((rc = ifx_var_dealloc(&fable_var)) < 0)
    {
        fprintf(stderr, "Error calling ifx_var_dealloc.");
        exit(1);
    }

    /* Free resources associated with SELECT statement */

    EXEC SQL deallocate descriptor 'sel_desc';
    EXEC SQL free sel_curs;
    EXEC SQL free sel_stmt;

    /* Disconnect from the database */

    EXEC SQL disconnect current;

    printf( "\nTest Complete\n");
}

