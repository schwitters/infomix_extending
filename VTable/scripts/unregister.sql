-- ----------------------------------
-- unregister the VTable Bladelet
-- ----------------------------------
--
-- Drop the versioned tables:
--
CREATE FUNCTION rmvtabs()
  RETURNING lvarchar;

  DEFINE retval   LVARCHAR;
  DEFINE tabcount INTEGER;
  DEFINE btabname VARCHAR(128);
  DEFINE vtabname VARCHAR(128);

  LET tabcount = 0;
  FOREACH SELECT tabname
          INTO btabname
          FROM versioned_tables

    LET vtabname = "v_" || btabname;
    EXECUTE PROCEDURE ddl_exec(
      "DROP TABLE " || vtabname) ;

    LET vtabname = "v_" || btabname || "_add";
    EXECUTE PROCEDURE ddl_exec(
      "DROP TABLE " || vtabname) ;

    LET vtabname = "v_" || btabname || "_del";
    EXECUTE PROCEDURE ddl_exec(
      "DROP TABLE " || vtabname) ;

    LET tabcount = tabcount + 1;

  END FOREACH

  LET retval = "Versioned tables removed: " || tabcount;

  RETURN retval;

END FUNCTION;

EXECUTE PROCEDURE rmvtabs();

DROP PROCEDURE rmvtabs();

--
-- Drop the supporting tables:
--
DROP TABLE versioned_tables;
DROP TABLE states;
DROP TABLE sessions;

--
-- Drop the supporting functions and procedures
--
DROP PROCEDURE raise_error ;
DROP FUNCTION  state_create ;
DROP PROCEDURE state_delete ;
DROP PROCEDURE state_close ;
DROP PROCEDURE state_open ;
DROP FUNCTION  state_is_open ;
DROP FUNCTION  state_is_closed ;
DROP FUNCTION  col_typ_text ;
DROP FUNCTION  col_con_text ;
DROP FUNCTION  add_tab_ddl ;
DROP FUNCTION  del_tab_ddl ;
DROP FUNCTION  vir_tab_ddl ;
DROP FUNCTION  ins_trig_ddl ;
DROP FUNCTION  upd_trig_ddl ;
DROP FUNCTION  del_trig_ddl ;
DROP FUNCTION  versioned_table_create ;
DROP FUNCTION  current_state ;
DROP FUNCTION  current_lineage ;
DROP PROCEDURE set_state ;
DROP FUNCTION  state_description ;
DROP FUNCTION  vtrelease ;

--
-- Drop the access method:
--
DROP ACCESS_METHOD vtable RESTRICT;

--
-- Drop each of the purpose functions:
--
DROP PROCEDURE ddl_exec (LVARCHAR) ;
DROP FUNCTION vt_beginscan (POINTER) ;
DROP FUNCTION vt_getnext (POINTER,POINTER,POINTER) ;
DROP FUNCTION vt_getbyid (POINTER,POINTER,POINTER) ;
DROP FUNCTION vt_endscan (POINTER) ;
DROP FUNCTION vt_rescan (POINTER) ;
DROP FUNCTION vt_close (POINTER) ;
DROP FUNCTION vt_stats (POINTER,POINTER) ;
DROP FUNCTION vt_check (POINTER,INTEGER) ;
DROP FUNCTION vt_scancost (POINTER,POINTER) ;
DROP FUNCTION vt_open (POINTER) ;
DROP FUNCTION vt_insert (POINTER,POINTER,POINTER) ;
DROP FUNCTION vt_update (POINTER,POINTER,INTEGER) ;
DROP FUNCTION vt_delete (POINTER,INTEGER) ;

