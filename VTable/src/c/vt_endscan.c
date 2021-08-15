#include <mi.h>
#include <miami.h>
#include "vtable.h"

mi_integer vt_endscan (MI_AM_SCAN_DESC *sd)
{

	MI_CONNECTION *conn;
	MI_DATUM *col_vals;
	mi_boolean *col_nulls;

        MI_STATEMENT *stmt_desc;
        MI_DATUM     *values;
        mi_integer   *lengths;
        mi_integer   *nulls;
        mi_string   **types;

	vt_scanud_t *vt_scanud;

        /* Get the cached user data from scan descriptor */
	vt_scanud = mi_scan_userdata(sd);

        /* Get the connection handle */
	conn = vt_scanud->conn;

	/* Get column values and null value flags arrays */
	col_vals = vt_scanud->col_vals;
	col_nulls = vt_scanud->col_nulls;

        /* Get prepared statement and associated input parameter arrays */
	stmt_desc = vt_scanud->stmt_desc;
	values = vt_scanud->values;
	lengths = vt_scanud->lengths;
	nulls = vt_scanud->nulls;
	types = vt_scanud->types;

	/* Finish query */
	mi_query_finish(conn);

	/* Close prepared statement and free associated resources*/
	if (stmt_desc != NULL)
	{
		mi_close_statement(stmt_desc);
		mi_drop_prepared_statement(stmt_desc);
		mi_free(values);
		mi_free(lengths);
		mi_free(nulls);
		mi_free(types);
	}

	/* Free column values and null value flags arrays */
	mi_free(col_vals);
	mi_free(col_nulls);

	/* Close the connection */
	mi_close(conn);

	return MI_OK;
}
