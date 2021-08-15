/*
** NAME
**   ddl_exec
**
** ARGUMENTS
**   SQL DDL statement passed in as lvarchar
**
** DESCRIPTION
**   Executes SQL DLL statement passed in as lvarchar
**
** BUILD INSTRUCTIONS
**   See Makefile included in this directory
**
** DATABASE CREATE
**   create procedure ddl_exec(lvarchar)
**   external name
**   '<PATH_TO_SHLIB>/ddl_exec.so(ddl_exec)'
**   language C;
**
*/

#include "mi.h"

void
ddl_exec( mi_lvarchar *sql_cmd, MI_FPARAM *fParam )
{
	char		*cmd;
	MI_CONNECTION	*conn;
	mi_integer	res;

	/* Allocate string buffer to hold SQL command */
	if ((cmd = (char *) mi_lvarchar_to_string( sql_cmd )) == NULL)
		mi_db_error_raise(conn, MI_EXCEPTION,
			"mi_lvarchar_to_string failed");

	/* Get connection handle */
	if ((conn = mi_open(NULL, NULL, NULL)) == NULL)
		mi_db_error_raise(conn, MI_EXCEPTION,
			"mi_open failed");

	/* Send the SQL command */
	if (mi_exec(conn, cmd, 0) == MI_ERROR)
		mi_db_error_raise(conn, MI_EXCEPTION,
			"mi_exec failed");

	/* Get result of SQL command */
	while ((res = mi_get_result(conn)) != MI_NO_MORE_RESULTS) {

		/* Switch on result ... MI_DDL expected */
		switch (res) {
		case MI_ERROR:
			mi_db_error_raise(conn, MI_EXCEPTION,
				"mi_get_result returned MI_ERROR");

		case MI_DDL:
			break;

		case MI_DML:
			mi_db_error_raise(conn, MI_EXCEPTION,
				"mi_get_result returned MI_DML");

		case MI_ROWS:
			mi_db_error_raise(conn, MI_EXCEPTION,
				"mi_get_result returned MI_ROWS");

		default:
			mi_db_error_raise(conn, MI_EXCEPTION,
				"mi_get_result returned unknown results");
		}
	}

	/* Free up the allocated memory */
	mi_free((char *)cmd) ;

	/* Release statement resources */
	if ( mi_query_finish(conn) == MI_ERROR )
		mi_db_error_raise(conn, MI_EXCEPTION,
			"mi_query_finish failed");

	/* Close the connection */
	if ( mi_close(conn) == MI_ERROR )
		mi_db_error_raise(conn, MI_EXCEPTION,
			"mi_close failed");

}

