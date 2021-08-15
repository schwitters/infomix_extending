/*
 * This file has been modified by Chris Bosch as follows:
 *
 *  1.  The 'regerror' procedure has been re-written so that it
 *      raises an exception using 'mi_db_error_raise'.
 *
 */

#include <mi.h>

void
regerror(s)
char *s;
{
	mi_db_error_raise((MI_CONNECTION *)NULL, MI_EXCEPTION, s);
}
