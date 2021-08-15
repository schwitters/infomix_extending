========================================================================
                     DBDK DataBlade Project vti_demo
========================================================================


BladeSmith Version 4.00.TC1B has created the vti_demo
DataBlade module for you. This application not only demonstrates the
basics of using an Informix DataBlade but is also a starting point for
writing your DataBlade module application.

This file contains a summary of what you will find in each of the files
that make up your vti_demo DataBlade module.

vti_demo.h

	This include file contains various definitions along with the
	'C' definitions of all your opaque types.

udr.c

	This file contains all the User Defined Routines,
	Iterators and Aggregates of the vti_demo DataBlade module.

support.c

	This file contains all the support functions that are required
	for the vti_demo DataBlade module.

vti_demo.def

	This file contains a list of all exported C functions
	declared in the vti_demo DataBlade module.
	This file is used by Microsoft's Visual C++ linker.

readme.txt

	This file.

warnings.txt

	This file contains special information pertinent to
	implementing the DataBlade module.

========================================================================

Building your DataBlade module
==============================

If you intend to compile your DataBlade module for NT, you may use either
the WinNT.mak or vti_demo.mak makefiles.

To build your DataBlade module from the command line, type

		nmake -s -nologo -f WinNT.mak

To load your DataBlade module's source files into Microsoft Visual C++
version 4.1, select "Open Workspace..." from the menus.  When using
versions 4.2 or later, select "Open Workspace..." from the menus.  For
the project name, type vti_demo.mak.  For version 5.0 and later,
the makefile is automatically converted.

To build your DataBlade module on UNIX, first transport the generated
files to your UNIX machine.  Be sure to copy in ASCII mode when using
ftp.  On your UNIX machine, the TARGET environment variable must be set
to the location/filename of the platform-specific make include file. For
Solaris, this is "$INFORMIXDIR/incl/dbdk/makeinc.solaris".

vti_demoU.mak is the makefile used to build the DataBlade
module under UNIX.  To build the DataBlade module, type:

               make -f vti_demo.mak

Documentation
=============

The following is a list of documents containing additional information
about DataBlade module programming.

	for procedures on creating DataBlade modules:

		DataBlade Developers Kit User's Guide

	for C coding:

		DataBlade API Programmer's Manual

	for using BladeManager:

		DataBlade Module Installation and Registration Guide

	for understanding opaque data types:

		Extending INFORMIX-Universal Server: Data Types

	for understanding User-Defined Routines:

		Extending INFORMIX-Universal Server: User-Defined Routines

	for SQL use:
		Informix Guide to SQL: Syntax
		Informix Guide to SQL: Tutorial
		Informix Guide to SQL: Reference

