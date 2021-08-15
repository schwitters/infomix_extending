
/*
** Structure for scan userdata
*/

typedef struct vt_scanud
{
	MI_CONNECTION *conn;         /* connection handle           */
	MI_ROW_DESC   *rd;           /* row descriptor              */
	mi_integer     ncols;        /* number of columns in row    */
	mi_integer     nprojcols;    /* number of projected columns */
	mi_smallint   *projcols;     /* projected column numbers    */
	MI_DATUM      *col_vals;     /* column values array         */
	mi_boolean    *col_nulls;    /* column null value flags     */

	MI_STATEMENT  *stmt_desc;
	MI_DATUM      *values;
	mi_integer    *lengths;
	mi_integer    *nulls;
	mi_string    **types;

} vt_scanud_t;
