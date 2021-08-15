## <font color="#0000FF">VTable</font>

### <font color="#0000FF">An Access Method For Managing Versioned Tables</font>

* * *

<big>_

For some applications, a "versioned" database table has advantages over a standard one. Standard database tables can only maintain a single state of the data that they contain, but versioned tables can simultaneously manage multiple states. With a versioned table, you can create a snapshot of your data at a point in time, and, later, query the table as it existed then. You can undo changes back to any saved point. You can maintain multiple concurrent versions of your data, each evolving independently from a common ancestor.

This paper describes "VTable", which uses a primary access method, together with a set of SPL routines, triggers, and tables, to support versioned tables in IDS.

_</big>

* * *

<a name="contents">

### <font color="#0000FF">Contents</font>

</a>

<a name="contents"></a>
*   <a name="contents"></a>[Overview](#overview)
*   [Architecture](#architect)

*   [Tables](#database_tables)
*   [SPL Routines](#spl_routines)
*   [Access Method](#access_method)
*   [Triggers](#triggers)

*   [Software Requirements](#requirements)
*   [Getting Started](#start)

*   [Download the distribution](#download)
*   [Install VTable](#install)
*   [Register VTable to a database](#register)
*   [Build a new shared object](#build)

*   [User Defined Routines (UDRs)](#udrs)

*   [current_lineage](#current_lineage)
*   [current_state](#current_state)
*   [set_state](#set_state)
*   [state_close](#state_close)
*   [state_create](#state_create)
*   [state_delete](#state_delete)
*   [state_description](#state_description)
*   [state_is_closed](#state_is_closed)
*   [state_is_open](#state_is_open)
*   [state_open](#state_open)
*   [versioned_table_create](#versioned_table_create)

*   [References](#references)

* * *

<a name="overview">

### <font color="#0000FF">Overview</font>

> VTable is an access method that extends IDS with the ability to maintain versioned tables. A versioned table is one that may present multiple views of the data that it contains, based on previously defined "states".
> 
> When a user connects to the database, a default table state is assigned, and that user will see versioned tables as they appear for that state. The user can work with the default state for versioned tables, or can request a different one by setting the state for her session using the <font face="courier">set_state()</font> command.
> 
> Users can also create new states that branch from existing ones. In this manner, versioned tables can evolve along several parallel paths, with each path serving a different purpose.
> 
> Note that if your database contains more than one versioned table, _**all**_ of the versioned tables move in lock step with respect to states. When you create a new state, or set your session to work with a specific state, the state applies to all of the versioned tables defined for your database.
> 
> This document will present an overview of VTable. Readers interested in understanding the utility in more detail should download the distribution and examine the registration scripts and the source code. All user-callable routines are written in SPL, the source for which is contained in the registration script (<font face="courier">register.sql</font>). The code to support the access method is written in C, and is in the <font face="courier">src/c</font> directory.

* * *

</a><a name="architect">

#### <font color="0000FF">Architecture</font>

</a>

> <a name="architect">
> 
> VTable consists of:
> 
> </a>
> 
> <a name="architect"></a>
> *   <a name="architect"></a>[Database tables](#database_tables)
> [](#database_tables)
> *   [](#database_tables)[A collection of SPL routines](#spl_routines)
> *   [A "Primary Access Method" developed with the Virtual Table Interface (VTI)](#access_method)
> *   [A collection of triggers](#triggers)
> 
> * * *
> 
> <a name="database_tables">
> 
> #### <font color="0000FF">Database Tables</font>
> 
> When VTable is registered to a database, three database tables are created:
> 
> *   **STATES** holds information about the states that have been defined for versioned tables.
> *   **SESSIONS** holds information about user sessions that are working with versioned tables.
> *   **VERSIONED_TABLES** holds information about versioned tables that exist in the database.
> 
> In addition, two database tables are created when you create a new versioned table using <font face="courier">versioned_table_create()</font>. <font face="Courier">versioned_table_create</font> takes the name of an existing table, and builds:
> 
> 1.  A "virtual" table as the version-enabled mirror of the base table.
> 2.  A "deletes" table.
> 3.  An "adds" table.
> 
> For example, if you have a base table named "mytable", then <font face="courier">versioned_table_create("mytable")</font> will produce:
> 
> *   <font face="courier">v_mytable</font>
> *   <font face="courier">v_mytable_add</font>
> *   <font face="courier">v_mytable_del</font>
> 
> "<font face="courier">v_mytable</font>" is the new "versioned" table, and should be the target table for all user sessions. All of the other tables are managed by the VTable routines, and should not be updated manually.
> 
> The SQL text for creating the <font face="courier">STATES, SESSIONS</font>, and <font face="courier">VERSIONED_TABLES</font> tables can be found near the beginning of the <font face="courier">register.sql</font> script. The text that creates the versioned table and its "add" and "delete" companion tables is in the SPL code for the <font face="courier">versioned_table_create</font> routine, which is elsewhere in <font face="courier">register.sql</font>.
> 
> * * *
> 
> </a><a name="spl_routines">
> 
> #### <font color="0000FF">SPL Routines</font>
> 
> </a>
> 
> <a name="spl_routines">All of the VTable functions and procedures that are called directly by users were written using SPL. This includes the list defined under</a> [User Defined Routines](#udrs) above. The complete definition for each SPL routine can be found in the <font face="courier">register.sql</font> script.
> 
> * * *
> 
> <a name="access_method">
> 
> #### <font color="0000FF">Access Method</font>
> 
> A Primary Access Method is a set of functions that the database server can call to access data that may not reside in an Informix table. Typically, Primary Access Methods are used to allow SQL queries to be run against data that is stored outside the database. An access method could be written, for example, to read a formatted flat file and present its fields as database columns.
> 
> The VTable access method works a little differently. Instead of accessing external data and presenting it as an Informix database table, VTable intercepts queries that are sent to a versioned table, and rewrites them to take the user's session state into account. In doing so, VTable refers to the "adds" table and the "deletes" table, as well as the base table, and returns results that are appropriate to the user session that sent the query.
> 
> Query rewriting is performed in the "<font face="courier">vt_beginscan</font>" function, which is part of the VTable access method code. Please refer to the C code if you are interested in seeing exactly how this works.
> 
> * * *
> 
> </a><a name="triggers">
> 
> #### <font color="0000FF">Triggers</font>
> 
> In addition to creating "add" and "delete" tables to manage changes to versioned tables, <font face="courier">versioned_table_create()</font> defines a set of triggers that fire whenever an insert, update, or delete is performed on a versioned table. These triggers make sure that the appropriate information is recorded in the "adds" or "deletes" table for the versioned table.
> 
> The text that defines the triggers can be found in the <font face="courier">ins_trig_dll</font> SPL routine in the <font face="courier">register.sql</font> script. <font face="courier">ins_trig_ddl</font> is called by <font face="courier">versioned_table_create()</font> when you create a new versioned table.
> 
> </a>[Contents](#contents)

* * *

<a name="requirements">

### <font color="#0000FF">Software Requirements</font>

</a>

> <a name="requirements">
> 
> **VTable** has been tested with the following configurations:
> 
> <dl>
> 
> <dt>UNIX:</dt>
> 
> <dd>
> 
> *   Solaris OS version 5.7
> *   IDS 9.21.UC2
> 
> </dd>
> 
> <dt>NT:</dt>
> 
> <dd>
> 
> *   NT 4.0 with Service Pack 5
> *   IDS 9.21.TC2
> 
> </dd>
> 
> </dl>
> 
> </a>[Contents](#contents)

* * *

<a name="start">

### <font color="#0000FF">Getting Started</font>

</a>

> <a name="start"></a><a name="download">
> 
> #### <font color="0000FF">Download the distribution</font>
> 
> Download the **VTable.zip** distribution file, and extract the contents.
> 
> The VTable files are extracted into a directory hierarchy under the directory **VTable**.
> 
> * * *
> 
> </a><a name="install">
> 
> #### <font color="0000FF">Install</font>
> 
> On Unix, login as user informix, or, on NT, log on as a user who is a member of the "informix-admin" group. Set your INFORMIXDIR environment variable to the name of the Informix server in which you wish to install VTable.
> 
> Create a "VTable" directory in your server's "extend" directory:
> 
> > % mkdir $INFORMIXDIR/extend/VTable
> 
> The "VTable" directory into which the distribution files were extracted contains a "scripts" directory and a "src" directory tree. Copy all of the files from the "scripts" directory to your new $INFORMIXDIR/extend/VTable directory.
> 
> Next, locate the appropriate VTable.bld file for your environment. The Solaris version is in "src/solaris-sparc", and the NT version is in "WinNT-i386". Copy the appropriate version of VTable.bld to "$INFORMIXDIR/extend/VTable".
> 
> **IMPORTANT:** If you are using NT, you MUST set the "read-only" property of the VTable.bld file or the bladelet will fail to load at runtime.
> 
> There are two methods you can use to set the "Read-only" attribute:
> 
> 1.  Use the "Explore" utility to locate the file, highlight it, and select "File" and "Properties" from the menu bar. Check the "read-only" attribute, select "Apply" and "Exit".
> 
> 3.  Use the command line in a shell. For example, from an KSH window:  
>     
>     
>     <font face="courier">% attrib +r $INFORMIXDIR/extend/VTable/VTable.bld</font>
>     
>     
> 
> * * *
> 
> </a><a name="build">
> 
> #### <font color="0000FF">Building a new shared object</font>
> 
> The distribution for VTable comes with pre-built shared object libraries for Solaris 2.7 and NT 4.0\. If you are on a different platform, or if you make modifications to the source code, you must recompile for your platform using the instructions provided below.
> 
> The source and make files of VTable are located under the "VTable/src" directory.
> 
> **NT:**
> 
> > On NT, C language database extensions are built using Microsoft Visual Studio. For IDS release 9.21, you should use Visual Studio version 6.0.
> > 
> > Use the "VTable_NT.mak" file to build the bladelet on NT. You will need to modify the makefile to provide the correct path for the Microsoft Visual Studio libraries and include files on your machine. See the definitions for "CPP_PROJ" and "LINK32_FLAGS" in the makefile for these entries.
> > 
> > Once "VTable_NT.mak" has been modified for your environment, build the VTable.bld file with the following command:
> > 
> > > $ nmake -f VTable_NT.mak
> > 
> > If the build succeeds, a new copy of VTable.bld is placed in the "WinNT-i386" directory.
> 
> **UNIX:**
> 
> > On Solaris, VTable was built and tested with version 5.0 of the standard Sun C compiler. The Unix makefile for VTable is designed to use the standard compiler for each platform. It should not be necessary to modify the makefile on Unix.
> > 
> > To build VTable on UNIX, first set your TARGET environment variable to the location/filename of the platform-specific make include file. On Solaris, this is "$INFORMIXDIR/incl/dbdk/makeinc.solaris".
> > 
> > Navigate to the "src" directory, and build the VTable shared object with this command:
> > 
> > > % make -f VTable_Unix.mak
> > 
> > The linker will display a list of unresolved symbols that include several DataBlade API functions (mi_alloc, mi_file_seek, etc.) and a few standard C library routines (sprintf, stcmpr). These modules will be dynamically loaded at runtime, so the fact that they are unresolved during the build is not an error.
> 
> </a>
> 
> <a name="build">Detailed instructions for building UNIX and NT shared object libraries can be found on the "DataBlade Developer" corner of the Informix IDN web site</a> [http://www.informix.com/idn](http://www.informix.com/idn)
> 
> * * *
> 
> <a name="register">
> 
> #### <font color="0000FF">Registering VTable in a database</font>
> 
> Once VTable has been installed on a server, it can be registered to databases that run under that server.
> 
> On both Unix and NT, VTable is registered by running an SQL script. Navigate to your $INFORMIXDIR/extend/VTable directory, and enter the following command, where "myDb" is the name of your target database:
> 
> > % dbaccess -e myDb register
> 
> The "register.sql" script creates the access method, the support tables, functions, and triggers for VTable.
> 
> </a>

<a name="register"></a>[Contents](#contents)

* * *

<a name="udrs">

### <font color="#0000FF">User Defined Routines (UDRs)</font>

</a>

> <a name="udrs"></a><a name="current_lineage">
> 
> #### CURRENT_LINEAGE
> 
> Returns the lineage for the state currently associated with a session.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE current_lineage();
> > 
> > (expression)
> > 
> > > 0,1,7
> 
> * * *
> 
> </a><a name="current_state">
> 
> #### CURRENT_STATE
> 
> Returns the state currently associated with a session.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE current_state();
> > 
> > (expression)
> > 
> > > 7
> 
> * * *
> 
> </a><a name="set_state">
> 
> #### SET_STATE
> 
> Takes an integer representing the ID for an existing state, and sets the current session to work from that state.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE set_state(2);
> > 
> > Routine executed.
> 
> * * *
> 
> </a><a name="state_close">
> 
> #### STATE_CLOSE
> 
> Takes a state ID as input, and marks the state as closed. New states can only be branched from closed states.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_close(2);
> > 
> > Routine executed.
> 
> * * *
> 
> </a><a name="state_create">
> 
> #### STATE_CREATE
> 
> Takes an integer representing the ID of an existing state, and a text string containing a description for a new state. Creates a new state that branches from the one specified. The existing state must be "closed", and must either be owned by the current user, or marked "public".
> 
> Returns the id of the new state.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_create(2, "Temporary state for unit testing");
> > 
> > (expression)
> > 
> > > 5
> 
> * * *
> 
> </a><a name="state_delete">
> 
> #### STATE_DELETE
> 
> Takes the ID of an existing state and deletes it. Only "leaf nodes" can be deleted. A leaf node is a state that is not an element in the lineage of another state.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_delete(5);
> > 
> > Routine executed.
> 
> * * *
> 
> </a><a name="state_description">
> 
> #### STATE_DESCRIPTION
> 
> Takes the id of an existing state. Returns the "description" text that was supplied when the state was created.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_description(5);
> > 
> > (expression)
> > 
> > > Temporary branch for unit testing
> 
> * * *
> 
> </a><a name="state_is_closed">
> 
> #### STATE_IS_CLOSED
> 
> Takes the id of an existing state. Returns "true" if that state is closed, or "false" if it is not.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_is_closed(3);
> > 
> > (expression)
> > 
> > > t
> 
> * * *
> 
> </a><a name="state_is_open">
> 
> #### STATE_IS_OPEN
> 
> Takes the id of an existing state. Returns "true" if that state is open, or "false" if it is not.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_is_open(3);
> > 
> > (expression)
> > 
> > > f
> 
> * * *
> 
> </a><a name="state_open">
> 
> #### STATE_OPEN
> 
> Takes the id of a closed state, and marks it open.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE state_open(3);
> > 
> > Routine executed.
> 
> * * *
> 
> </a><a name="versioned_table_create">
> 
> #### VERSIONED_TABLE_CREATE
> 
> Takes the name of an existing database table, and creates a new versioned table for it. The table must exist, must not already be versioned, and must contain a primary key column.
> 
> Returns the ID for the new versioned table.
> 
> Note: The table name parameter must be entered in lower case.
> 
> **Syntax**
> 
> > > EXECUTE PROCEDURE versioned_table_create("mytable");
> > 
> > (expression)
> > 
> > > 1
> 
> </a>

<a name="versioned_table_create"></a>[Contents](#contents)

* * *

<a name="references">

### <font color="#0000FF">References</font>

> For more information on topics related to VTable, please refer to the following sources:
> 
> *   "The Virtual Table Interface", on the "DataBlade Developers" corner of the IDN web site.
> *   "Virtual-Table Interface Programmer's Manual"
> *   "Informix Guide to SQL: Syntax"
> 
> Informix manuals can be downloaded in PDF format from the Informix web site.

* * *

</a>

<a name="references">Last modified on . Please send your comments or questions concerning this page to</a> ["><nobr>idn-foundation@informix.com</nobr>](mailto:idn-foundation@informix.com?subject=Comments on 
<!--#echo var=).

* * *
