#include <mi.h>
#include <miami.h>

mi_real *
vt_scancost (MI_AM_TABLE_DESC *td, MI_AM_QUAL_DESC *qd)
{

	mi_real *scancost;

	/* Allocate result */
	if((scancost = mi_alloc(sizeof(mi_real))) == NULL)
	{
		mi_db_error_raise((MI_CONNECTION *)NULL, MI_SQL, "VM001");
	}

	*scancost = 1.0;

	return (scancost);
}

