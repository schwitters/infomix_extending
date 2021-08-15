#include <mi.h>
#include <miami.h>
#include "vtable.h"

mi_integer vt_getnext (MI_AM_SCAN_DESC *sd, MI_ROW **retrow, mi_integer *retrowid)
{

        MI_AM_TABLE_DESC *td;
        MI_ROW_DESC *rd;
	MI_CONNECTION *conn;
	mi_integer result, error;
	mi_integer i, ncols, nprojcols;
	mi_smallint c, *projcols;
	MI_ROW *row;

	MI_DATUM *col_vals;
	mi_boolean *col_nulls;

	MI_DATUM colval;
	mi_integer collen;

	vt_scanud_t *vt_scanud;

        /* Get the cached user data from scan descriptor */
	vt_scanud = mi_scan_userdata(sd);

	/* Get the connection handle */
	conn = vt_scanud->conn;

	/* Get the row descriptor */
	rd = vt_scanud->rd;

	/* Get the number of columns in the row */
	ncols = vt_scanud->ncols;

	/* Get the number of projected columns */
	nprojcols = vt_scanud->nprojcols;

	/* Get the projected column numbers */
	projcols = vt_scanud->projcols;

	/* Get the column values array */
	col_vals = vt_scanud->col_vals;

	/* Get the column null value flags array */
	col_nulls = vt_scanud->col_nulls;

	/* Fetch the next row from the connection cursor */
	row = mi_next_row(conn, &error);

	if (error == MI_ERROR)
	{
		result = MI_ERROR;
	}
	else if (row == NULL)
	{
		result = MI_NO_MORE_RESULTS;
	}
	else
	{
		/* Initialize the column null value indicator array */
		for (i=0; i < ncols; i++)
		{
			col_nulls[i] = MI_TRUE;
		}

		/* Set the column values for the projected columns */
		for (i=0; i < nprojcols; i++)
		{
			/* Initialize variables */
			colval = NULL;
			collen = 0;

			c = projcols[i];

			switch (mi_value(row, c, (MI_DATUM) &colval, &collen)) {

			case MI_ERROR:
				mi_db_error_raise(conn, MI_EXCEPTION,
					"MI_ERROR returned from call to mi_value");

			case MI_NULL_VALUE:
				col_nulls[c] = MI_TRUE;
				break;

			case MI_NORMAL_VALUE:
				col_vals[c] = colval;
				col_nulls[c] = MI_FALSE;
				break;

			default:
				mi_db_error_raise(conn, MI_EXCEPTION,
					"Unknown result for call to mi_value");
			}
		}

		*retrow = mi_row_create(conn, rd, col_vals, col_nulls);

		result = MI_ROWS;
	}

	return result;
}

