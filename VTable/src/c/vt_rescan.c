#include <mi.h>
#include <miami.h>

mi_integer vt_rescan (MI_AM_SCAN_DESC *sd)
{
	mi_integer rc;

	rc = vt_endscan(sd);

	if (rc != MI_OK)
		return (rc);

	rc = vt_beginscan(sd);

	return (rc);
}

