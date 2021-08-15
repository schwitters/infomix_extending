/*
 * tutorial.c
 *
 *  Created on: Oct 22, 2010
 *      Author: root
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/* Used by Informix GLS routines. */
#include <ifxgls.h>

/* Include when accessing the Informix API. */
#include <mi.h>
#include <miami.h>
#include <minmmem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

/* My shared library */
#include "sample.h"



mi_boolean tutorial_loadlib
(
MI_FPARAM *	fparam
)
{
	print("loadlib");

	return MI_TRUE;
}



mi_integer tutorial_close
(
MI_AM_TABLE_DESC*	tableDesc,
MI_FPARAM *	fparam
)
{
	print("close");

	return MI_OK;
}



mi_integer tutorial_create
(
MI_AM_TABLE_DESC*	tableDesc,
MI_FPARAM *	fparam
)
{
	print("create");

	return MI_OK;
}


mi_integer tutorial_open
(
MI_AM_TABLE_DESC*	tableDesc,
MI_FPARAM *	fparam
)
{
	print("open");

	return MI_OK;
}



mi_integer tutorial_drop
(
MI_AM_TABLE_DESC*	tableDesc,
MI_FPARAM *	fparam
)
{
	print("drop");

	return MI_OK;
}
