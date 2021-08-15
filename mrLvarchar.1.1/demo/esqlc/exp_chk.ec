EXEC SQL define SUCCESS 0;
EXEC SQL define WARNING 1;
EXEC SQL define NODATA 100;
EXEC SQL define RTERROR -1;

char statement[80];

/*
 * The sqlstate_err() function checks the SQLSTATE status variable to see
 * if an error or warning has occurred following an SQL statement.
 */
int4 sqlstate_err()
{
    int4 err_code = RTERROR;

    if(SQLSTATE[0] == '0') /* trap '00', '01', '02' */
        {
        switch(SQLSTATE[1])
            {
            case '0': /* success - return 0 */
                err_code = SUCCESS;
                break;
            case '1': /* warning - return 1 */
                err_code = WARNING;
                break;
            case '2': /* end of data - return 100 */
                err_code = NODATA;
                break;
            default: /* error - return SQLCODE */
                break;
            }
        }
    return(err_code);
}


/*
 * The disp_sqlstate_err() function executes the GET DIAGNOSTICS
 * statement and prints the detail for each exception that is returned.
 */
void disp_sqlstate_err()
{
mint j;

EXEC SQL BEGIN DECLARE SECTION;
    mint exception_count;
    char overflow[2];
    mint exception_num=1;
    char class_id[255];
    char subclass_id[255];
    char message[255];
    mint messlen;
    char sqlstate_code[6];
    mint i;
EXEC SQL END DECLARE SECTION;

    printf("---------------------------------");
    printf("-------------------------\n");
    printf("SQLSTATE: %s\n",SQLSTATE);
    printf("SQLCODE: %d\n", SQLCODE);
    printf("\n");
 
    EXEC SQL get diagnostics :exception_count = NUMBER,
        :overflow = MORE;
    printf("EXCEPTIONS:  Number=%d\t", exception_count);
    printf("More? %s\n", overflow);
    for (i = 1; i <= exception_count; i++)
        {
        EXEC SQL get diagnostics  exception :i
            :sqlstate_code = RETURNED_SQLSTATE,
            :class_id = CLASS_ORIGIN, :subclass_id = SUBCLASS_ORIGIN,
            :message = MESSAGE_TEXT, :messlen = MESSAGE_LENGTH;
        printf("- - - - - - - - - - - - - - - - - - - -\n");
        printf("EXCEPTION %d: SQLSTATE=%s\n", i,
            sqlstate_code);
        message[messlen-1] = '\0';
        printf("MESSAGE TEXT: %s\n", message);
 
        j = byleng(class_id, stleng(class_id));
        class_id[j] = '\0';
        printf("CLASS ORIGIN: %s\n",class_id);
 
        j = byleng(subclass_id, stleng(subclass_id));
        subclass_id[j] = '\0';
        printf("SUBCLASS ORIGIN: %s\n",subclass_id);
        }
 
    printf("---------------------------------");
    printf("-------------------------\n");
}
 
void disp_error(stmt)
char *stmt;
{
    printf("\n********Error encountered in %s********\n",
        stmt);
    disp_sqlstate_err();
}

void disp_warning(stmt)
char *stmt;
{
    printf("\n********Warning encountered in %s********\n",
        stmt);
    disp_sqlstate_err();
}

void disp_exception(stmt, sqlerr_code, warn_flg)
char *stmt;
int4 sqlerr_code;
mint warn_flg;
{
    switch (sqlerr_code)
        {
        case SUCCESS:
        case NODATA:
            break;
        case WARNING:
            if(warn_flg)
                disp_warning(stmt);
            break;
        case RTERROR:
            disp_error(stmt);
            break;
        default:
            printf("\n********INVALID EXCEPTION STATE for %s********\n",
                stmt);
            break;
        }
}

/*
 * The exp_chk() function calls sqlstate_err() to check the SQLSTATE
 * status variable to see if an error or warning has occurred following
 * an SQL statement. If either condition has occurred, exp_chk()
 * calls disp_sqlstate_err() to print the detailed error information.
 *
 * This function handles exceptions as follows:
 *   runtime errors - call exit(1)
 *   warnings - continue execution, returning "1"
 *   success - continue execution, returning "0"
 *   Not Found - continue execution, returning "100"
 */
int4 exp_chk(stmt, warn_flg)
char *stmt;
mint warn_flg;
{
    int4 sqlerr_code = SUCCESS;
 
    sqlerr_code = sqlstate_err();
    disp_exception(stmt, sqlerr_code, warn_flg);
 
    if(sqlerr_code == RTERROR)   /* Exception is a runtime error */
        {
        /* Exit the program after examining the error */
        printf("********Program terminated*******\n\n");
        exit(1);
        }
    else                        /* Exception is "success", "Not Found",*/
        return(sqlerr_code);    /*  or "warning"                       */
}

/*
 * The exp_chk2() function calls sqlstate_err() to check the SQLSTATE
 * status variable to see if an error or warning has occurred following
 * an SQL statement. If either condition has occurred, exp_chk2()
 * calls disp_exception() to print the detailed error information.
 *
 * This function handles exceptions as follows:
 *   runtime errors - continue execution, returning SQLCODE (<0)
 *   warnings - continue execution, returning one (1)
 *   success - continue execution, returning zero (0)
 *   Not Found - continue execution, returning 100
 */
int4 exp_chk2(stmt, warn_flg)
char *stmt;
mint warn_flg;
{
    int4 sqlerr_code = SUCCESS;
    int4 sqlcode;
 
    sqlcode = SQLCODE;  /* save SQLCODE in case of error */
    sqlerr_code = sqlstate_err();
    disp_exception(stmt, sqlerr_code, warn_flg);
 
    if(sqlerr_code == RTERROR)  /* if runtime error, return SQLCODE */
        sqlerr_code = sqlcode;
 
    return(sqlerr_code);
}

/*
 * The whenexp_chk() function calls sqlstate_err() to check the SQLSTATE
 * status variable to see if an error or warning has occurred following
 * an SQL statement. If either condition has occurred, whenerr_chk()
 * calls disp_sqlstate_err() to print the detailed error information.
 *
 * This function is expected to be used with the WHENEVER SQLERROR
 * statement: it executes an exit(1) when it encounters a negative
 * error code. It also assumes the presence of the "statement" global
 * variable, set by the calling program to the name of the statement
 * encountering the error.
*/
whenexp_chk()
{
    int4 sqlerr_code = SUCCESS;
    mint disp = 0;
 
    sqlerr_code = sqlstate_err();
 
    if(sqlerr_code == WARNING)
        {
        disp = 1;
        printf("\n********Warning encountered in %s********\n",
            statement);
        }
    else
        if(sqlerr_code == RTERROR)
            {
            printf("\n********Error encountered in %s********\n",
                statement);
            disp = 1;
            }
    if(disp)
        disp_sqlstate_err();
 
    if(sqlerr_code == RTERROR)
        {
        /* Exit the program after examining the error */
        printf("********Program terminated*******\n\n");
        exit(1);
        }
    else
        {
        if(sqlerr_code == WARNING)
            printf("\n********Program execution continues********\n\n");
        return(sqlerr_code);
        }
}

/*
 * The 'ignore206' error handling routine ignores the -206 error
 * described below:
 * 
 * ----------------------------------------------------------------------------
 * -206	The specified table <table-name> is not in the database.
 * 
 * The database server cannot find a table or view specified in the statement.
 * The table or view might have been renamed or dropped from the database.
 * 
 * Check the names of tables and views in the statement. If the names are
 * spelled as you intended, check that you are using the database you want.
 * To find the names of all tables in the database, query the systables table.
 * To find the names of all views, query the sysviews table.
 * ----------------------------------------------------------------------------
 */

ignore206 ()
{
    int4 sqlerr_code = SUCCESS;

    if (SQLCODE != -206)
        sqlerr_code = whenexp_chk();

    return(sqlerr_code);
}
