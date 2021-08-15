-- -----------------------------------------------
-- Simple script to register the VTable Bladelet
-- -----------------------------------------------
--
-- Create the purpose functions:
--
create procedure ddl_exec (lvarchar)
external name "$INFORMIXDIR/extend/VTable/VTable.bld(ddl_exec)"
language c;

create function vt_open (pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_open)"
language c;

create function vt_close (pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_close)"
language c;

create function vt_insert (pointer, pointer, pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_insert)"
language c;

create function vt_delete (pointer, int)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_delete)"
language c;

create function vt_update (pointer, pointer, int)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_update)"
language c;

create function vt_stats (pointer, pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_stats)"
language c;

create function vt_scancost (pointer, pointer)
returns real
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_scancost)"
language c;

create function vt_check (pointer, int)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_check)"
language c;

create function vt_beginscan (pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_beginscan)"
language c;

create function vt_endscan (pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_endscan)"
language c;

create function vt_rescan (pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_rescan)"
language c;

create function vt_getbyid (pointer, pointer, pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_getbyid)"
language c;

create function vt_getnext (pointer, pointer, pointer)
returns int
external name "$INFORMIXDIR/extend/VTable/VTable.bld(vt_getnext)"
language c;

--
-- Create the access method:
--
CREATE PRIMARY ACCESS_METHOD vtable
(
-- am_sptype
-- am_cluster
am_rowids,
am_readwrite,
-- am_parallel
-- am_costfactor
-- am_create    = vt_create,
-- am_drop      = vt_drop,
am_open      = vt_open,
am_close     = vt_close,
am_insert    = vt_insert,
am_delete    = vt_delete,
am_update    = vt_update,
am_stats     = vt_stats,
am_scancost  = vt_scancost,
am_check     = vt_check,
am_beginscan = vt_beginscan,
am_endscan   = vt_endscan,
am_rescan    = vt_rescan,
am_getbyid   = vt_getbyid,
am_getnext   = vt_getnext
);

--
-- create the shared tables that support versioning
--
CREATE TABLE states
(
        state_id    SERIAL,
        owner       CHAR(32),
        created     DATETIME YEAR TO SECOND,
        closed      DATETIME YEAR TO SECOND,
        parent_id   INTEGER,
        description VARCHAR(255),
        lineage     VARCHAR(255)
);

CREATE TABLE versioned_tables
(
        vtabid       SERIAL,
        owner        CHAR(32),
        tabname      VARCHAR(128),
        pkcolid      INTEGER
);

CREATE TABLE sessions
(
        sid          INTEGER,
        connected    INTEGER,
        source_state INTEGER,
        diff_state   INTEGER
);

--
-- Initialize the states table
--
INSERT INTO states VALUES
(
        1,                 -- state_id
        'informix',        -- owner
        CURRENT,           -- created
        NULL,              -- closed
        0,                 -- parent_id
	'default',         -- description
        '0,1'              -- lineage
);

--
-- Add indexes for support tables:
--
CREATE UNIQUE INDEX states_pk ON states(state_id)
USING btree;

CREATE UNIQUE INDEX versioned_tables_pk ON versioned_tables(vtabid)
USING btree;

CREATE INDEX states_pid ON states(parent_id)
USING btree;

--
-- Add constraints:
--
ALTER TABLE states ADD CONSTRAINT
PRIMARY KEY (state_id) ;

--
-- Add supporting functions and procedures:
--

EXECUTE PROCEDURE ifx_allow_newline('t');

-- -----------------------------------------------------------------------------
-- raise_error
-- -----------------------------------------------------------------------------

CREATE PROCEDURE raise_error(code CHAR(4))

	IF code = 'VT01' THEN
		RAISE EXCEPTION -746, 0,
		'Cannot INSERT data ... current state is closed';
	ELIF code = 'VT02' THEN
		RAISE EXCEPTION -746, 0,
		'Cannot UPDATE data ... current state is closed';
	ELIF code = 'VT03' THEN
		RAISE EXCEPTION -746, 0,
		'Cannot DELETE data ... current state is closed';
	ELIF code = 'VT04' THEN
		RAISE EXCEPTION -746, 0,
		'Cannot INSERT data ... id already exists';
	ELSE
		RAISE EXCEPTION -746, 0,
		'Unknown error code (' || code || ')';
	END IF

END PROCEDURE;

-- -----------------------------------------------------------------------------
-- state_create
-- -----------------------------------------------------------------------------

CREATE FUNCTION state_create ( s_state_id INTEGER, s_state_desc VARCHAR(255) )
RETURNING INTEGER;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_closed DATETIME YEAR TO SECOND;
	DEFINE s_lineage VARCHAR(255);
	DEFINE new_state_id INTEGER;
	DEFINE in_trans, already_in_trans BOOLEAN;

	ON EXCEPTION SET esql, eisam, etext

		IF esql = -535 THEN

			LET already_in_trans = 't';

		ELIF (in_trans AND NOT already_in_trans) THEN

			ROLLBACK WORK;
			RAISE EXCEPTION esql, eisam, etext;

		ELSE

			RAISE EXCEPTION esql, eisam, etext;

		END IF

	END EXCEPTION WITH RESUME

	LET in_trans = 'f';
	LET already_in_trans = 'f';

	--
	-- Check preconditions
	--

	SELECT closed, lineage INTO s_closed, s_lineage
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	ELIF s_closed IS NULL THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not closed';
	END IF

	--
	-- Perform action(s)
	--

	BEGIN WORK;

	LET in_trans = 't';

	INSERT INTO states VALUES
	(
		0,                 -- state_id
		USER,              -- owner
		CURRENT,           -- created
		NULL,              -- closed
		s_state_id,        -- parent_id
		s_state_desc,      -- description
		NULL               -- lineage
	);

	LET new_state_id = DBINFO('sqlca.sqlerrd1');

	UPDATE states
	SET    lineage =  s_lineage||','||new_state_id
	WHERE  state_id = new_state_id;

	--
	-- Commit work if explicit transaction started in this routine
	--

	IF NOT already_in_trans THEN
		COMMIT WORK;
	END IF

	--
	-- Return result(s)
	--

	RETURN new_state_id;

END FUNCTION ; -- state_create

-- -----------------------------------------------------------------------------
-- state_delete
-- -----------------------------------------------------------------------------

CREATE PROCEDURE state_delete ( s_state_id INTEGER ) ;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_owner CHAR(32);

	ON EXCEPTION SET esql, eisam, etext

		IF esql = -692 THEN
			RAISE EXCEPTION -746, 0,
			'State ('||s_state_id||') is being referenced';
		ELSE
			RAISE EXCEPTION esql, eisam, etext;
		END IF

	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT owner INTO s_owner
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	ELIF s_owner != USER THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not owned by current user';
	END IF

	SELECT count(*) INTO nrows
	FROM   states
	WHERE  parent_id = s_state_id;

	IF nrows > 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not a leaf node';
	END IF

	--
	-- Perform action(s)
	--

	DELETE FROM states WHERE state_id = s_state_id;

END PROCEDURE ; -- state_delete

-- -----------------------------------------------------------------------------
-- state_close
-- -----------------------------------------------------------------------------

CREATE PROCEDURE state_close ( s_state_id INTEGER ) ;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_closed DATETIME YEAR TO SECOND;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT closed INTO s_closed
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	ELIF s_closed IS NOT NULL THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not open';
	END IF

	--
	-- Perform action(s)
	--

	UPDATE states
	SET    closed = CURRENT
	WHERE  state_id = s_state_id;

END PROCEDURE ; -- state_close

-- -----------------------------------------------------------------------------
-- state_open
-- -----------------------------------------------------------------------------

CREATE PROCEDURE state_open ( s_state_id INTEGER ) ;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_owner CHAR(32);
	DEFINE s_closed DATETIME YEAR TO SECOND;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT owner, closed INTO s_owner, s_closed
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	ELIF s_closed IS NULL THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not closed';
	ELIF s_owner != USER THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not owned by current user';
	END IF

	SELECT count(*) INTO nrows
	FROM   states
	WHERE  parent_id = s_state_id;

	IF nrows > 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') is not a leaf node';
	END IF

	--
	-- Perform action(s)
	--

	UPDATE states
	SET    closed = NULL
	WHERE  state_id = s_state_id;

END PROCEDURE ;  -- state_open

-- -----------------------------------------------------------------------------
-- state_is_open
-- -----------------------------------------------------------------------------

CREATE FUNCTION state_is_open ( s_state_id INTEGER )
RETURNING BOOLEAN;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_closed DATETIME YEAR TO SECOND;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT closed INTO s_closed
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	END IF

	--
	-- Return result(s)
	--

	IF s_closed IS NULL THEN
		RETURN 't';
	ELSE
		RETURN 'f';
	END IF

END FUNCTION ; -- state_is_open

-- -----------------------------------------------------------------------------
-- state_is_closed
-- -----------------------------------------------------------------------------

CREATE FUNCTION state_is_closed ( s_state_id INTEGER )
RETURNING BOOLEAN;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_closed DATETIME YEAR TO SECOND;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT closed INTO s_closed
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	END IF

	--
	-- Return result(s)
	--

	IF s_closed IS NULL THEN
		RETURN 'f';
	ELSE
		RETURN 't';
	END IF

END FUNCTION ; -- state_is_closed

-- -----------------------------------------------------------------------------
-- col_typ_text
-- -----------------------------------------------------------------------------

CREATE FUNCTION col_typ_text ( t_coltype SMALLINT,
                                t_collength SMALLINT ,
                                t_extended_id INTEGER )
RETURNING LVARCHAR;

	DEFINE result LVARCHAR;
	DEFINE vc_max, vc_min SMALLINT;
	DEFINE pr_tot, pr_dec SMALLINT;
	DEFINE tu_start, tu_end, tu_len, tu_flen SMALLINT;

	LET t_coltype = MOD(t_coltype,256);

	IF (t_coltype = 0) THEN
		LET result = 'char';

		LET result = result || '('||t_collength||')';

	ELIF (t_coltype = 1) THEN
		LET result = 'smallint';

	ELIF (t_coltype = 2) THEN
		LET result = 'integer';

	ELIF (t_coltype = 3) THEN
		LET result = 'float';

	ELIF (t_coltype = 4) THEN
		LET result = 'smallfloat';

	ELIF (t_coltype = 5) THEN
		LET result = 'decimal';

		LET pr_dec = MOD(t_collength,256);
		LET pr_tot = (t_collength - MOD(t_collength,256)) / 256;

		IF pr_dec = 255 THEN
			LET result = result || '('||pr_tot||')';
		ELSE
			LET result = result || '('||pr_tot||','||pr_dec||')';
		END IF

	ELIF (t_coltype = 6) THEN
		RAISE EXCEPTION -746, 0,
		'Cannot version table with serial column';

	ELIF (t_coltype = 7) THEN
		LET result = 'date';

	ELIF (t_coltype = 8) THEN
		LET result = 'money';

		LET pr_dec = MOD(t_collength,256);
		LET pr_tot = (t_collength - MOD(t_collength,256)) / 256;

		IF pr_dec = 255 THEN
			LET result = result || '('||pr_tot||')';
		ELSE
			LET result = result || '('||pr_tot||','||pr_dec||')';
		END IF

	ELIF (t_coltype = 9) THEN
		LET result = 'null';

	ELIF (t_coltype = 10) THEN
		LET result = 'datetime';

		LET tu_start = (MOD(t_collength,256)-MOD(t_collength,16)) / 16;
		LET tu_end = MOD(t_collength,16);

		--
		-- Add time unit start to datetime specification
		--

		IF tu_start = 0 THEN
			LET result = result || ' year';
		ELIF tu_start = 2 THEN
			LET result = result || ' month';
		ELIF tu_start = 4 THEN
			LET result = result || ' day';
		ELIF tu_start = 6 THEN
			LET result = result || ' hour';
		ELIF tu_start = 8 THEN
			LET result = result || ' minute';
		ELIF tu_start = 10 THEN
			LET result = result || ' second';
		ELIF tu_start = 11 THEN
			LET result = result || ' fraction(1)';
		ELIF tu_start = 12 THEN
			LET result = result || ' fraction(2)';
		ELIF tu_start = 13 THEN
			LET result = result || ' fraction(3)';
		ELIF tu_start = 14 THEN
			LET result = result || ' fraction(4)';
		ELIF tu_start = 15 THEN
			LET result = result || ' fraction(5)';
		END IF

		--
		-- Add time unit end to datetime specification
		--

		IF tu_end = 0 THEN
			LET result = result || ' to year';
		ELIF tu_end = 2 THEN
			LET result = result || ' to month';
		ELIF tu_end = 4 THEN
			LET result = result || ' to day';
		ELIF tu_end = 6 THEN
			LET result = result || ' to hour';
		ELIF tu_end = 8 THEN
			LET result = result || ' to minute';
		ELIF tu_end = 10 THEN
			LET result = result || ' to second';
		ELIF tu_end = 11 THEN
			LET result = result || ' to fraction(1)';
		ELIF tu_end = 12 THEN
			LET result = result || ' to fraction(2)';
		ELIF tu_end = 13 THEN
			LET result = result || ' to fraction(3)';
		ELIF tu_end = 14 THEN
			LET result = result || ' to fraction(4)';
		ELIF tu_end = 15 THEN
			LET result = result || ' to fraction(5)';
		END IF

	ELIF (t_coltype = 11) THEN
		LET result = 'byte';

	ELIF (t_coltype = 12) THEN
		LET result = 'text';

	ELIF (t_coltype = 13) THEN
		LET result = 'varchar';

		LET vc_max = MOD(t_collength,256);
		LET vc_min = (t_collength - MOD(t_collength,256)) / 256;

		IF vc_min = 0 THEN
			LET result = result || '('||vc_max||')';
		ELSE
			LET result = result || '('||vc_max||','||vc_min||')';
		END IF

	ELIF (t_coltype = 14) THEN
		LET result = 'interval';

		LET tu_start = (MOD(t_collength,256)-MOD(t_collength,16)) / 16;
		LET tu_end = MOD(t_collength,16);
		LET tu_len = (t_collength-MOD(t_collength,256)) / 256;
		LET tu_flen = tu_len - (tu_end - tu_start);

		--
		-- Add time unit start to interval specification
		--

		IF tu_start = 0 THEN
			LET result = result || ' year';
		ELIF tu_start = 2 THEN
			LET result = result || ' month';
		ELIF tu_start = 4 THEN
			LET result = result || ' day';
		ELIF tu_start = 6 THEN
			LET result = result || ' hour';
		ELIF tu_start = 8 THEN
			LET result = result || ' minute';
		ELIF tu_start = 10 THEN
			LET result = result || ' second';
		ELIF tu_start = 11 THEN
			LET result = result || ' fraction(1)';
		ELIF tu_start = 12 THEN
			LET result = result || ' fraction(2)';
		ELIF tu_start = 13 THEN
			LET result = result || ' fraction(3)';
		ELIF tu_start = 14 THEN
			LET result = result || ' fraction(4)';
		ELIF tu_start = 15 THEN
			LET result = result || ' fraction(5)';
		END IF

		--
		-- Add non-default precision qualifier to time unit start
		--

		IF tu_start = 0 AND tu_flen != 4 THEN
			LET result = result || '('||tu_flen||')';
		ELIF tu_flen != 2 THEN
			LET result = result || '('||tu_flen||')';
		END IF

		--
		-- Add time unit end to interval specification
		--

		IF tu_end = 0 THEN
			LET result = result || ' to year';
		ELIF tu_end = 2 THEN
			LET result = result || ' to month';
		ELIF tu_end = 4 THEN
			LET result = result || ' to day';
		ELIF tu_end = 6 THEN
			LET result = result || ' to hour';
		ELIF tu_end = 8 THEN
			LET result = result || ' to minute';
		ELIF tu_end = 10 THEN
			LET result = result || ' to second';
		ELIF tu_end = 11 THEN
			LET result = result || ' to fraction(1)';
		ELIF tu_end = 12 THEN
			LET result = result || ' to fraction(2)';
		ELIF tu_end = 13 THEN
			LET result = result || ' to fraction(3)';
		ELIF tu_end = 14 THEN
			LET result = result || ' to fraction(4)';
		ELIF tu_end = 15 THEN
			LET result = result || ' to fraction(5)';
		END IF

	ELIF (t_coltype = 15) THEN
		LET result = 'nchar';

		LET result = result || '('||t_collength||')';

	ELIF (t_coltype = 16) THEN
		LET result = 'nvarchar';

		LET vc_max = MOD(t_collength,256);
		LET vc_min = (t_collength - MOD(t_collength,256)) / 256;

		IF vc_min = 0 THEN
			LET result = result || '('||vc_max||')';
		ELSE
			LET result = result || '('||vc_max||','||vc_min||')';
		END IF

	ELIF (t_coltype = 17) THEN
		LET result = 'int8';

	ELIF (t_coltype = 18) THEN
		RAISE EXCEPTION -746, 0,
		'Cannot version table with serial8 column';

	ELIF (t_coltype = 19) THEN
		LET result = 'set';

		RAISE EXCEPTION -746, 0,
		'Versioning table with SET column not implemented';

	ELIF (t_coltype = 20) THEN
		LET result = 'multiset';

		RAISE EXCEPTION -746, 0,
		'Versioning table with MULTISET column not implemented';

	ELIF (t_coltype = 21) THEN
		LET result = 'list';

		RAISE EXCEPTION -746, 0,
		'Versioning table with LIST column not implemented';

	ELIF (t_coltype = 22) THEN
		LET result = 'row';

		RAISE EXCEPTION -746, 0,
		'Versioning table with ROW column not implemented';

	ELIF (t_coltype = 23) THEN
		LET result = 'collection';

		RAISE EXCEPTION -746, 0,
		'Versioning table with COLLECTION column not implemented';

	ELIF (t_coltype = 24) THEN
		LET result = 'rowref';

		RAISE EXCEPTION -746, 0,
		'Versioning table with ROWREF column not implemented';

	ELIF (t_coltype = 40 OR t_coltype = 41) THEN
		SELECT name INTO result
		FROM sysxtdtypes
		WHERE  extended_id = t_extended_id;

	ELSE
		RAISE EXCEPTION -746, 0,
		'Unknown column type (coltype=' || t_coltype ||
		', extended_id=' || t_extended_id || ')' ;

	END IF;

	--
	-- Return result(s)
	--

	RETURN result;

END FUNCTION; -- col_typ_text

-- -----------------------------------------------------------------------------
-- col_con_text
-- -----------------------------------------------------------------------------

CREATE FUNCTION col_con_text ( t_coltype SMALLINT )
RETURNING LVARCHAR;

	DEFINE result LVARCHAR;

	LET result = ' ';

	IF t_coltype > 255 THEN
		LET result = result || 'NOT NULL';
	END IF

	--
	-- Return result(s)
	--

	RETURN result;

END FUNCTION; -- col_con_text

-- -----------------------------------------------------------------------------
-- add_tab_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION add_tab_ddl ( t_tabid INTEGER, t_tabname VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE a_tabname VARCHAR(128);
	DEFINE t_colname VARCHAR(128);
	DEFINE t_coltype SMALLINT;
	DEFINE t_collength SMALLINT;
	DEFINE t_extended_id INTEGER;
	DEFINE t_col_def, t_col_defs LVARCHAR;
	DEFINE ddl LVARCHAR;

	LET ddl =
	'
	CREATE TABLE v_<tabname>_add
	(
	state_id INTEGER NOT NULL, <col_defs>
	)
	' ;

	LET t_col_defs = '';

	FOREACH SELECT sc.colname, sc.coltype, sc.collength, sc.extended_id
		INTO   t_colname, t_coltype, t_collength, t_extended_id
		FROM   syscolumns sc
		WHERE  sc.tabid = t_tabid

		LET t_col_def = '<name> <type> <constraint>';

		LET t_col_def = REPLACE(t_col_def, '<name>', t_colname);

		LET t_col_def = REPLACE(t_col_def, '<type>',
			col_typ_text(t_coltype,t_collength,t_extended_id) );

		LET t_col_def = REPLACE(t_col_def, '<constraint>',
			col_con_text(t_coltype) );

		LET t_col_defs = t_col_defs || ', ' || t_col_def ;

	END FOREACH

	LET t_col_defs = SUBSTR(t_col_defs,2);

	LET ddl = REPLACE(ddl, '<tabname>', t_tabname);
	LET ddl = REPLACE(ddl, '<col_defs>', t_col_defs);

	RETURN ddl;


END FUNCTION; -- add_tab_ddl

-- -----------------------------------------------------------------------------
-- del_tab_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION del_tab_ddl ( t_tabname VARCHAR(128) ,
                            t_pkcolname VARCHAR(128) ,
                            t_pkcoltype VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE ddl LVARCHAR;

	LET ddl =
	'
	CREATE TABLE v_<tabname>_del
	(
	state_id INTEGER NOT NULL ,
	<pkcolname> <pkcoltype> NOT NULL ,
	deleted_at INTEGER NOT NULL
	)
	' ;

	Let ddl = REPLACE(ddl, '<tabname>', t_tabname);
	Let ddl = REPLACE(ddl, '<pkcolname>', t_pkcolname);
	Let ddl = REPLACE(ddl, '<pkcoltype>', t_pkcoltype);

	RETURN ddl;

END FUNCTION; -- del_tab_ddl

-- -----------------------------------------------------------------------------
-- vir_tab_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION vir_tab_ddl ( t_tabid INTEGER ,
                            t_tabname VARCHAR(128) ,
                            t_pkcolname VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE v_tabname VARCHAR(128);
	DEFINE t_colname VARCHAR(128);
	DEFINE t_coltype SMALLINT;
	DEFINE t_collength SMALLINT;
	DEFINE t_extended_id INTEGER;
	DEFINE t_col_defs LVARCHAR;
	DEFINE ddl LVARCHAR;


	LET ddl =
	'
	CREATE TABLE v_<tabname>
	(
		state_id INTEGER, <col_defs>
	)
	USING vtable (pkcolname = ''<pkcolname>'')
	' ;

	LET t_col_defs = '';

	FOREACH SELECT sc.colname, sc.coltype, sc.collength, sc.extended_id
		INTO   t_colname, t_coltype, t_collength, t_extended_id
		FROM   syscolumns sc
		WHERE  sc.tabid = t_tabid

		LET t_col_defs = t_col_defs ||
			', ' || t_colname || ' ' ||
			col_typ_text(t_coltype,t_collength,t_extended_id) ;

	END FOREACH

	LET t_col_defs = SUBSTR(t_col_defs,2);

	LET ddl = REPLACE(ddl, '<tabname>', t_tabname);
	LET ddl = REPLACE(ddl, '<col_defs>', t_col_defs);
	LET ddl = REPLACE(ddl, '<pkcolname>', t_pkcolname);

	RETURN ddl;

END FUNCTION; -- vir_tab_ddl

-- -----------------------------------------------------------------------------
-- ins_trig_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION ins_trig_ddl ( t_tabid INTEGER ,
                               t_tabname VARCHAR(128) ,
                               t_pkcolname VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE t_colname VARCHAR(128);
	DEFINE t_cols LVARCHAR;
	DEFINE t_postcols LVARCHAR;
	DEFINE ddl LVARCHAR;

	LET ddl =

	'
	CREATE TRIGGER v_<tabname>_ins INSERT ON v_<tabname>
	REFERENCING NEW AS POST
	BEFORE
	WHEN (state_is_closed(current_state()))
	(
		EXECUTE PROCEDURE raise_error(''VT01'')
	)
	FOR EACH ROW
	WHEN (EXISTS
		(SELECT <pkcolname> FROM v_<tabname>
		 WHERE <pkcolname> = POST.<pkcolname>))
	(
		EXECUTE PROCEDURE raise_error(''VT04'')
	)
	,
	(
		INSERT INTO v_<tabname>_add (state_id, <colnames>)
		VALUES (current_state(), <postcolnames>)
	)
	' ;

	LET t_cols = '';
	LET t_postcols = '';

	FOREACH SELECT sc.colname INTO t_colname
		FROM   syscolumns sc
		WHERE  sc.tabid = t_tabid

		LET t_cols = t_cols ||
			', ' || t_colname;

		LET t_postcols = t_postcols ||
			', POST.' || t_colname;

	END FOREACH

	LET t_cols = SUBSTR(t_cols,2);
	LET t_postcols = SUBSTR(t_postcols,2);

	LET ddl = REPLACE(ddl, '<tabname>', t_tabname);
	LET ddl = REPLACE(ddl, '<pkcolname>', t_pkcolname);
	LET ddl = REPLACE(ddl, '<colnames>', t_cols);
	LET ddl = REPLACE(ddl, '<postcolnames>', t_postcols);


	RETURN ddl;

END FUNCTION; -- ins_trig_ddl

-- -----------------------------------------------------------------------------
-- upd_trig_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION upd_trig_ddl ( t_tabid INTEGER ,
                               t_tabname VARCHAR(128) ,
                               t_pkcolname VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE t_colname VARCHAR(128);
	DEFINE t_cols LVARCHAR;
	DEFINE t_postcols LVARCHAR;
	DEFINE t_cols_eq_postcols LVARCHAR;
	DEFINE ddl LVARCHAR;

	LET ddl =

	'
	CREATE TRIGGER v_<tabname>_upd UPDATE ON v_<tabname>
	REFERENCING OLD AS PRE NEW AS POST
	BEFORE
	WHEN (state_is_closed(current_state()))
	(
		EXECUTE PROCEDURE raise_error(''VT02'')
	)
	FOR EACH ROW
	WHEN (PRE.state_id = current_state())
	(
		UPDATE v_<tabname>_add
		SET    <cols_eq_postcols>
		WHERE  state_id = PRE.state_id AND <pkcolname> = PRE.<pkcolname>
	)
	,
	WHEN (PRE.state_id != current_state())
	(
		INSERT INTO v_<tabname>_add (state_id, <colnames>)
		VALUES (current_state(), <postcolnames>)
		,
		INSERT INTO v_<tabname>_del (state_id, <pkcolname>, deleted_at)
		VALUES (PRE.state_id, PRE.<pkcolname>, current_state())
	)
	' ;

	LET t_cols = '';
	LET t_postcols = '';
	LET t_cols_eq_postcols = '';

	FOREACH SELECT sc.colname INTO t_colname
		FROM   syscolumns sc
		WHERE  sc.tabid = t_tabid

		LET t_cols = t_cols ||
			', ' || t_colname;

		LET t_postcols = t_postcols ||
			', POST.' || t_colname;

		LET t_cols_eq_postcols = t_cols_eq_postcols ||
			', ' || t_colname || ' = POST.' || t_colname;

	END FOREACH

	LET t_cols = SUBSTR(t_cols,2);
	LET t_postcols = SUBSTR(t_postcols,2);
	LET t_cols_eq_postcols = SUBSTR(t_cols_eq_postcols,2);

	LET ddl = REPLACE(ddl, '<tabname>', t_tabname);
	LET ddl = REPLACE(ddl, '<pkcolname>', t_pkcolname);
	LET ddl = REPLACE(ddl, '<colnames>', t_cols);
	LET ddl = REPLACE(ddl, '<postcolnames>', t_postcols);
	LET ddl = REPLACE(ddl, '<cols_eq_postcols>', t_cols_eq_postcols);


	RETURN ddl;

END FUNCTION; -- upd_trig_ddl

-- -----------------------------------------------------------------------------
-- del_trig_ddl
-- -----------------------------------------------------------------------------

CREATE FUNCTION del_trig_ddl ( t_tabid INTEGER ,
                               t_tabname VARCHAR(128) ,
                               t_pkcolname VARCHAR(128) )
RETURNING LVARCHAR;

	DEFINE t_colname VARCHAR(128);
	DEFINE t_cols LVARCHAR;
	DEFINE t_postcols LVARCHAR;
	DEFINE t_cols_eq_postcols LVARCHAR;
	DEFINE ddl LVARCHAR;

	LET ddl =

	'
	CREATE TRIGGER v_<tabname>_del DELETE ON v_<tabname>
	REFERENCING OLD AS PRE
	BEFORE
	WHEN (state_is_closed(current_state()))
	(
		EXECUTE PROCEDURE raise_error(''VT03'')
	)
	FOR EACH ROW
	WHEN (PRE.state_id = current_state())
	(
		DELETE FROM v_<tabname>_add
		WHERE  state_id = PRE.state_id AND <pkcolname> = PRE.<pkcolname>
	)
	,
	WHEN (PRE.state_id != current_state())
	(
		INSERT INTO v_<tabname>_del (state_id, <pkcolname>, deleted_at)
		VALUES (PRE.state_id, PRE.<pkcolname>, current_state())
	)

	' ;

	LET ddl = REPLACE(ddl, '<tabname>', t_tabname);
	LET ddl = REPLACE(ddl, '<pkcolname>', t_pkcolname);

	RETURN ddl;

END FUNCTION; -- del_trig_ddl

-- -----------------------------------------------------------------------------
-- versioned_table_create
-- -----------------------------------------------------------------------------

CREATE FUNCTION versioned_table_create ( t_tabname VARCHAR(128) )
RETURNING INTEGER;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE v_tabname VARCHAR(128);
	DEFINE a_tabname VARCHAR(128);
	DEFINE d_tabname VARCHAR(128);
	DEFINE t_owner CHAR(32);
	DEFINE t_tabid INTEGER;
	DEFINE t_idxname VARCHAR(128);
	DEFINE t_idxpart1 INTEGER;
	DEFINE t_pkcolname VARCHAR(128);
	DEFINE t_pkcoltype VARCHAR(128);
	DEFINE v_tabid INTEGER;
	DEFINE t_colname VARCHAR(128);
	DEFINE t_coltype SMALLINT;
	DEFINE t_collength SMALLINT;
	DEFINE t_extended_id INTEGER;
	DEFINE ddl LVARCHAR;
	DEFINE in_trans, already_in_trans BOOLEAN;

	ON EXCEPTION SET esql, eisam, etext

		IF esql = -535 THEN

			LET already_in_trans = 't';

		ELIF (in_trans AND NOT already_in_trans) THEN

			ROLLBACK WORK;
			RAISE EXCEPTION esql, eisam, etext;

		ELSE

			RAISE EXCEPTION esql, eisam, etext;

		END IF

	END EXCEPTION WITH RESUME

	LET in_trans = 'f';
	LET already_in_trans = 'f';

	--
	-- Check preconditions
	--

	LET t_owner = USER;

	SELECT count(*) INTO nrows
	FROM   versioned_tables
	WHERE  owner = t_owner and tabname = t_tabname ;

	IF nrows > 0 THEN
		RAISE EXCEPTION -746, 0,
		'Table (' || TRIM(t_owner) || '.' || t_tabname ||
		') is already versioned';
	END IF

	SELECT tabid INTO t_tabid
	FROM   systables
	WHERE  owner = t_owner and tabname = t_tabname ;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'Table (' || TRIM(t_owner) || '.' || t_tabname ||
		') does not exist';
	END IF

	SELECT idxname INTO t_idxname
	FROM   sysconstraints
	WHERE  tabid = t_tabid and constrtype = 'P' ;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'Table (' || TRIM(t_owner) || '.' || t_tabname ||
		') does not have a primary key constraint';
	END IF

	SELECT part1 INTO t_idxpart1
	FROM   sysindexes
	WHERE  idxname = t_idxname and part2 = 0 ;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'Cannot version table (' ||
		TRIM(t_owner) || '.' || t_tabname ||
		') with composite primary key';
	END IF

	SELECT colname INTO t_colname
	FROM   syscolumns sc
	WHERE  sc.tabid = t_tabid AND sc.colname = 'state_id' ;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows != 0 THEN
		RAISE EXCEPTION -746, 0,
		'Cannot version table (' ||
		TRIM(t_owner) || '.' || t_tabname ||
		') with column named ''state_id''';
	END IF

	SELECT sc.colname, sc.coltype, sc.collength, sc.extended_id
	INTO   t_colname, t_coltype, t_collength, t_extended_id
	FROM   syscolumns sc
	WHERE  sc.tabid = t_tabid AND
	       sc.colno = t_idxpart1 ;

	LET t_pkcolname = t_colname;
	LET t_pkcoltype = col_typ_text(t_coltype,t_collength,t_extended_id);

	--
	-- Perform action(s)
	--

	BEGIN WORK;

	LET in_trans = 't';

	INSERT INTO versioned_tables VALUES
	(
		0,                 -- vtabid
		t_owner,           -- owner
		t_tabname,         -- tabname
		t_idxpart1         -- pkcolid
	);

	LET v_tabid = DBINFO('sqlca.sqlerrd1');

	--
	-- Create additions table
	--

	LET ddl = add_tab_ddl(t_tabid, t_tabname);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Create deletions table
	--

	LET ddl = del_tab_ddl(t_tabname, t_pkcolname, t_pkcoltype);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Create virtual versioned table
	--

	LET ddl = vir_tab_ddl(t_tabid, t_tabname, t_pkcolname);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Create INSERT trigger on virtual table
	--

	LET ddl = ins_trig_ddl(t_tabid, t_tabname, t_pkcolname);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Create UPDATE trigger on virtual table
	--

	LET ddl = upd_trig_ddl(t_tabid, t_tabname, t_pkcolname);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Create DELETE trigger on virtual table
	--

	LET ddl = del_trig_ddl(t_tabid, t_tabname, t_pkcolname);
	EXECUTE PROCEDURE ddl_exec(ddl);

	--
	-- Commit work if explicit transaction started in this routine
	--

	IF NOT already_in_trans THEN
		COMMIT WORK;
	END IF

	--
	-- Return result(s)
	--

	RETURN v_tabid;

END FUNCTION ; -- versioned_table_create

-- -----------------------------------------------------------------------------
-- current_state
-- -----------------------------------------------------------------------------

CREATE FUNCTION current_state ( )
RETURNING INTEGER;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE result INTEGER;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	SELECT sessions.source_state INTO result
	FROM   sessions, sysmaster:syssessions ss
	WHERE  ss.sid = DBINFO('sessionid') AND
	       ss.sid = sessions.sid AND
	       ss.connected = sessions.connected;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		LET result = 1;
	END IF

	--
	-- Return result(s)
	--

	RETURN result;

END FUNCTION; -- current_state

-- -----------------------------------------------------------------------------
-- current_lineage
-- -----------------------------------------------------------------------------

CREATE FUNCTION current_lineage ( )
RETURNING VARCHAR(255);

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE result VARCHAR(255);

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	SELECT states.lineage INTO result
	FROM   sysmaster:syssessions ss, sessions, states
	WHERE  ss.sid = DBINFO('sessionid') AND
	       ss.sid = sessions.sid AND
	       ss.connected = sessions.connected AND
	       sessions.source_state = states.state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		SELECT lineage INTO result
		FROM   states
		WHERE  state_id = 1;
	END IF

	--
	-- Return result(s)
	--

	RETURN result;

END FUNCTION; -- current_lineage

-- -----------------------------------------------------------------------------
-- set_state
-- -----------------------------------------------------------------------------

CREATE PROCEDURE set_state ( s_source_state INTEGER ) ;

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_connected INTEGER;

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	SELECT s.connected
	INTO   s_connected
	FROM   sessions s, sysmaster:syssessions ss
	WHERE  s.sid = DBINFO('sessionid') AND
	       s.sid = ss.sid AND
	       s.connected = ss.connected;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		INSERT INTO sessions(sid,connected,source_state)
		SELECT ss.sid, ss.connected, s_source_state
		FROM   sysmaster:syssessions ss
		WHERE  ss.sid = DBINFO('sessionid');
	ELSE
		UPDATE sessions
		SET    source_state = s_source_state,
		       diff_state = NULL
		WHERE  sid = DBINFO('sessionid');
	END IF

END PROCEDURE; -- set_state

-- -----------------------------------------------------------------------------
-- state_description
-- -----------------------------------------------------------------------------

CREATE FUNCTION state_description ( s_state_id INTEGER )
RETURNING VARCHAR(255);

	DEFINE esql, eisam INTEGER;
	DEFINE etext VARCHAR(255);
	DEFINE nrows INTEGER;
	DEFINE s_description VARCHAR(255);

	ON EXCEPTION SET esql, eisam, etext
		RAISE EXCEPTION esql, eisam, etext;
	END EXCEPTION

	--
	-- Check preconditions
	--

	SELECT description INTO s_description
	FROM   states
	WHERE  state_id = s_state_id;

	LET nrows = DBINFO('sqlca.sqlerrd2');

	IF nrows = 0 THEN
		RAISE EXCEPTION -746, 0,
		'State ('||s_state_id||') does not exist';
	END IF

	--
	-- Return result(s)
	--

	RETURN s_description;

END FUNCTION ; -- state_description

-- -----------------------------------------------------------------------------
-- VTRelease
-- -----------------------------------------------------------------------------

CREATE FUNCTION vtrelease ( )
RETURNING VARCHAR(255);

	DEFINE rel VARCHAR(255);

	LET rel = "VTable version 1.1";

	RETURN rel;

END FUNCTION ; -- vtrelease

