
#include <stdio.h>

#include <mi.h>
#include <miami.h>

#include "vtable.h"

mi_integer vt_parse_qd( MI_AM_QUAL_DESC *, MI_ROW_DESC *, char *, mi_integer *);
mi_integer vt_populate_arrays(MI_AM_QUAL_DESC *qd, MI_ROW_DESC *rd, MI_DATUM **varr, mi_integer **larr, mi_integer **narr, mi_string ***tarr, mi_integer *n, mi_integer i);
mi_integer vt_get_amparam( mi_string *, mi_string *, mi_string * );

mi_integer vt_beginscan (MI_AM_SCAN_DESC *sd)
{
	MI_CONNECTION         *conn;  /* Connection handle         */

        MI_AM_TABLE_DESC        *td;  /* Table descriptor          */
        MI_ROW_DESC             *rd;  /* Row descriptor            */
        MI_AM_QUAL_DESC         *qd;  /* Qualification descriptor  */

	mi_integer  i, ncols, nprojcols;
	mi_smallint c, *projcols;

        mi_char     qualbuf[BUFSIZ];  /* Buffer for qualification string */
        mi_char     sqltmp[BUFSIZ];  /* Buffer for SQL statement string */
        mi_char     sqlbuf[BUFSIZ];  /* Buffer for SQL statement string */

	mi_string  *virt_tab_name;
	mi_string  base_tab_name[MI_IDENTSIZE];
	mi_string  adds_tab_name[MI_IDENTSIZE];
	mi_string  dels_tab_name[MI_IDENTSIZE];

	mi_string  *amparam;
	mi_string  pkcolname[MI_IDENTSIZE];

	mi_integer res;

	MI_DATUM *col_vals;
	mi_boolean *col_nulls;

	vt_scanud_t *vt_scanud;

	MI_FUNC_DESC *fd;
	mi_integer error;
	mi_lvarchar *lineage;
	char *lineage_string;

	MI_STATEMENT *stmt_desc;
	mi_integer    n_qd_params, n_params;
	MI_DATUM     *values;
	mi_integer   *lengths;
	mi_integer   *nulls;
	mi_string   **types;
	mi_string   **rettypes;
	MI_MEMORY_DURATION md;


	md = mi_switch_mem_duration(PER_COMMAND);

	/* Initialize buffers */
	memset(pkcolname, 0, MI_IDENTSIZE);
	memset(sqltmp, 0, BUFSIZ);
	memset(sqlbuf, 0, BUFSIZ);
	sprintf(qualbuf, "AND");

        /* Get connection handle */
        if ((conn = mi_open(NULL, NULL, NULL)) == NULL)
	{
                mi_db_error_raise(conn, MI_EXCEPTION,
                        "Error getting connection handle");
	}

	/* Get the table and row descriptor */
	td = mi_scan_table(sd);
	rd = mi_tab_rowdesc(td);

	/* Get the number of columns in the row descriptor */
	ncols = mi_column_count(rd);

	/* Get the number of projected columns from the scan descriptor */
	nprojcols = mi_scan_nprojs(sd);

	/* Get the projected columns from the scan descriptor */
	projcols = mi_scan_projs(sd);

	/* Get the virtual table name */
	virt_tab_name = mi_tab_name(td);

	/* Format name strings for base, additions, deletes tables */
	sprintf(base_tab_name, "%s", &virt_tab_name[2]);
	sprintf(adds_tab_name, "%s_add", virt_tab_name);
	sprintf(dels_tab_name, "%s_del", virt_tab_name);

	/* Get access method parameters */
	if((amparam = mi_tab_amparam(td)) == NULL)
	{
		mi_db_error_raise(conn, MI_EXCEPTION,
			"Must supply access method parameters");
	}

	/* Get value for 'pkcolname' access method parameter */
	if((res = vt_get_amparam(amparam,"pkcolname=",pkcolname)) == MI_ERROR)
	{
		mi_db_error_raise(conn, MI_EXCEPTION,
			"Must supply 'pkcolname' access method parameter");
	}

	/* Get the qualification descriptor */
	qd = mi_scan_quals((MI_AM_SCAN_DESC *)sd);

	/* Parse the qualification descriptor and format string in qualbuf */
	n_qd_params = 0;
	if (qd != NULL)
	{
		vt_parse_qd(qd, rd, qualbuf, &n_qd_params);
	}
	n_params = 2 * n_qd_params;

	/* Create input parameter arrays if necessary */
	if (n_params > 0)
	{

		/* Allocate memory for input parameters values array */
		if((values = mi_alloc(n_params * sizeof(MI_DATUM))) == NULL)
		{
			mi_db_error_raise(conn, MI_SQL, "VM001");
		}

		/* Allocate memory for input parameters lengths array */
		if((lengths = mi_alloc(n_params * sizeof(mi_integer))) == NULL)
		{
			mi_db_error_raise(conn, MI_SQL, "VM001");
		}

		/* Allocate memory for input parameters nulls array */
		if((nulls = mi_alloc(n_params * sizeof(mi_integer))) == NULL)
		{
			mi_db_error_raise(conn, MI_SQL, "VM001");
		}

		/* Allocate memory for input parameters types array */
		if((types = mi_alloc(n_params * sizeof(mi_string *))) == NULL)
		{
			mi_db_error_raise(conn, MI_SQL, "VM001");
		}

		/* Initialize values, lengths, nulls, and types arrays */
		i=0;
		vt_populate_arrays(qd, rd, &values, &lengths, &nulls, &types, &i, n_qd_params);

	}

	/* Get the lineage for the current state */
	if ((fd = mi_routine_get(conn, 0, "current_lineage()")) == NULL)
	{
	    mi_db_error_raise(conn, MI_EXCEPTION,
		"Error getting 'current_lineage' function descriptor");
	}

	lineage = (mi_lvarchar *)mi_routine_exec(conn, fd, &error);

	if (error == MI_ERROR)
	{
	    mi_db_error_raise(conn, MI_EXCEPTION,
		"Error executing 'current_lineage' function");
	}

	lineage_string = mi_lvarchar_to_string(lineage);

	/* Format the SQL statement */
	sprintf(sqltmp, "\n\
	SELECT 0, * \n\
	FROM   %s b \n\
	WHERE  NOT EXISTS \n\
	       (SELECT * \n\
		FROM   %s d \n\
		WHERE  d.deleted_at in (%s) AND \n\
		       b.%s = d.%s) \n\
	       %%s \n\
	UNION ALL \n\
	SELECT * \n\
	FROM   %s a \n\
	WHERE  a.state_id IN (%s) AND \n\
	       NOT EXISTS \n\
	       (SELECT * \n\
		FROM   %s d \n\
		WHERE  d.deleted_at in (%s) AND \n\
		       a.%s = d.%s AND \n\
		       a.state_id = d.state_id) \n\
	       %%s ;\n\
	\n",

	base_tab_name,
	dels_tab_name,
	lineage_string,
	pkcolname,
	pkcolname,

	adds_tab_name,
	lineage_string,
	dels_tab_name,
	lineage_string,
	pkcolname,
	pkcolname

	);

	if (qd != NULL)
	{
		sprintf(sqlbuf, sqltmp, qualbuf, qualbuf);
	}
	else
	{
		sprintf(sqlbuf, sqltmp, "", "");
	}

fprintf(stderr, "%s", sqlbuf);

	if (n_params > 0)
	{

		/* Prepare the SQL statement */
		if ((stmt_desc = mi_prepare(conn, sqlbuf, NULL)) == NULL)
		{
			mi_db_error_raise(conn, MI_EXCEPTION,
				"Error preparing SQL statement");
		}

		/* Send the SQL command */
		if (mi_exec_prepared_statement(stmt_desc,
			MI_BINARY,
			1,
			n_params,
			values,
			lengths,
			nulls,
			types,
			0,
			NULL) != MI_OK)
		{
			mi_db_error_raise(conn, MI_EXCEPTION,
				"Error sending SQL command");
		}
	}
	else
	{
		stmt_desc = (MI_STATEMENT *)NULL;
		/* Send the SQL command */
		if (mi_exec(conn, sqlbuf, MI_QUERY_BINARY) == MI_ERROR)
		{
			mi_db_error_raise(conn, MI_EXCEPTION,
				"Error sending SQL command");
		}
	}

        /* Get result of SQL command */
        if ((res = mi_get_result(conn)) != MI_ROWS)
	{
		mi_db_error_raise(conn, MI_EXCEPTION,
			"Incorrect result from mi_get_result");
	}

	/* Allocate memory for scan userdata structure */
	if ((vt_scanud = (vt_scanud_t *)mi_alloc(sizeof(vt_scanud_t))) == NULL)
	{
		mi_db_error_raise(conn, MI_SQL, "VM001");
	}

	/* Allocate memory for column values array */
	if((col_vals = mi_alloc(ncols * sizeof(MI_DATUM))) == NULL)
	{
		mi_db_error_raise(conn, MI_SQL, "VM001");
	}

	/* Allocate memory for column null value flags array */
	if((col_nulls = mi_alloc(ncols * sizeof(mi_boolean))) == NULL)
	{
		mi_db_error_raise(conn, MI_SQL, "VM001");
	}

	/* Set values in scan userdata structure */
	vt_scanud->conn = conn;
	vt_scanud->rd = rd;
	vt_scanud->ncols = ncols;
	vt_scanud->nprojcols = nprojcols;
	vt_scanud->projcols = projcols;
	vt_scanud->col_vals = col_vals;
	vt_scanud->col_nulls = col_nulls;
	vt_scanud->stmt_desc = stmt_desc;
	vt_scanud->values = values;
	vt_scanud->lengths = lengths;
	vt_scanud->nulls = nulls;
	vt_scanud->types = types;

	/* Set scan descriptor userdata */
	mi_scan_setuserdata(sd, vt_scanud);

	/* Reset memory duration */
	mi_switch_mem_duration(md);

	/* Free allocated resources */
	mi_free(lineage_string);

	return MI_OK;
}

mi_integer
vt_parse_qd(MI_AM_QUAL_DESC *qd, MI_ROW_DESC *rd, char *qualbuf, mi_integer *n)
{
	mi_integer     i;

	mi_char        qual_boolop[4]; /* The boolean op in a complex qd */
	mi_integer     colnum;         /* Column number                  */
	mi_string     *colname;        /* Column name                    */

	/* Do we have a simple predicate here? */
	if (mi_qual_issimple(qd))
	{

	  if (mi_qual_negate(qd))
	  {
	    sprintf(qualbuf, "%s %s", qualbuf, "NOT");
	  }
	  sprintf(qualbuf, "%s %s", qualbuf, mi_qual_funcname(qd));

	  /* Get the column number and name */
	  colnum = mi_qual_column(qd);
	  colname = mi_column_name(rd,colnum);

	  /* If args are commuted, flip them. easier for remote parser */
	  if (mi_qual_commuteargs(qd))
	  {
	    sprintf(qualbuf, "%s(%s,%s)", qualbuf, "?", colname);
	  }
	  else
	  {
	    sprintf(qualbuf, "%s(%s,%s)", qualbuf, colname, "?");
	  }

	  /* Increment number of ? input parameters in qualbuf */
	  (*n)++;

	}
	else /* Qualification is complex. */
	{

	  if (mi_qual_boolop(qd) == MI_BOOLOP_AND)
	  {
	    strcpy(qual_boolop, "AND");
	  }
	  else
	  {
	    strcpy(qual_boolop, "OR");
	  }

	  sprintf(qualbuf, "%s (", qualbuf);
	  for (i=0; i<mi_qual_nquals(qd); i++)
	  {

	    vt_parse_qd(mi_qual_qual(qd, i), rd, qualbuf, n);

	    if (i < (mi_qual_nquals(qd) - 1))
	    {
	      sprintf(qualbuf, "%s %s ", qualbuf, qual_boolop);
	    }

	  }
	  sprintf(qualbuf, "%s )", qualbuf);

	}

	return 0;

}

mi_integer
vt_populate_arrays(MI_AM_QUAL_DESC *qd, MI_ROW_DESC *rd,
	MI_DATUM   **varr,
	mi_integer **larr,
	mi_integer **narr,
	mi_string ***tarr,
	mi_integer  *n,
	mi_integer  n_qd_params)
{
	mi_integer     i, index;
	mi_integer     colnum;
        MI_TYPE_DESC   *typedesc;
        mi_char        *typename;

	MI_DATUM     *values;
	mi_integer   *lengths, length;
	mi_integer   *nulls;
	mi_string   **types;

	/* Do we have a simple predicate here? */
	if (mi_qual_issimple(qd))
	{

		values = *varr;
		lengths = *larr;
		nulls = *narr;
		types = *tarr;
		index = *n;

		/* Get the column number */
		colnum = mi_qual_column(qd);

		/* Get the type descriptor */
		if (! (typedesc = mi_column_typedesc(rd, colnum)))
		{
			mi_db_error_raise(NULL, MI_EXCEPTION,
			    "Unable to get typedesc for qd column");
		}

		/* Get the type name */
		if (! (typename = mi_type_typename(typedesc)))
		{
			mi_db_error_raise(NULL, MI_EXCEPTION,
			    "Unable to get typename for qd column");
		}

		/* Set values for values array elements */
		values[index] = mi_qual_constant(qd);
		values[index + n_qd_params] = mi_qual_constant(qd);

		/* Set values for lengths array elements */
		if ((length = mi_type_length(typedesc)) == MI_ERROR)
		{
			length = 
			mi_get_varlen((mi_lvarchar *) values[index]);
		}
		lengths[index] = length;
		lengths[index + n_qd_params] = length;

		/* Set values for nulls array elements */
		if (mi_qual_constisnull(qd)==MI_TRUE)
		{
			nulls[index] = (mi_integer)1;
			nulls[index + n_qd_params] = (mi_integer)1;
		}
		else
		{
			nulls[index] = (mi_integer)0;
			nulls[index + n_qd_params] = (mi_integer)0;
		}

		/* Set values for types array elements */
		types[index] = typename;
		types[index + n_qd_params] = typename;

		/* Increment number of input parameters */
		(*n)++;

	}
	else /* Qualification is complex. */
	{
	  for (i=0; i<mi_qual_nquals(qd); i++)
	  {
	    vt_populate_arrays(mi_qual_qual(qd, i), rd, varr, larr, narr, tarr, n, n_qd_params);
	  }
	}

	return 0;

}

mi_integer
vt_get_amparam( mi_string *amparam, mi_string *name, mi_string *value )
{
	int i,j;
	mi_string *str, *res;

	/* Find param name in amparam string */
	str = (mi_string *)strstr(amparam, name);
	if (str == NULL)
	{
		return MI_ERROR;
	}

	/* Save index to start of param value in string */
	i = strlen(name);

	/* Skip characters to find end of param value (comma -or- NULL) */
	for (j = i; str[j] && str[j] != ',' ; j++) ;

	strncpy(value,&str[i],j-i);

	return MI_OK;
}
