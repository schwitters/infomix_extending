-- Generated by BladeSmith 4.00.TC1B2DO NOT MODIFY. --

execute procedure ifx_allow_newline('t');

insert into sysbldobjects
	(bld_id, obj_kind, obj_signature, obj_owner, sequence, create_sql,
	    create_can_fail, drop_sql, drop_can_fail)
values
(
	"%SYSBLDNAME%", 5, "UpOne (integer)",
	"%SYSBLDUSER%", 0,
	"create function UpOne (integer)
returns integer
external name ""$INFORMIXDIR/extend/%SYSBLDDIR%/TraceDemo.bld(UpOne)"" language c;

",
	"f",
	"drop function UpOne (integer);

",
	"f"
);

insert into sysbldobjects
	(bld_id, obj_kind, obj_signature, obj_owner, sequence, create_sql,
	    create_can_fail, drop_sql, drop_can_fail)
values
(
	"%SYSBLDNAME%", 181, "Alter-ExternalName-UpOne (integer)",
	"%SYSBLDUSER%", 0,
	"alter function UpOne (integer)
	with (Modify External Name = ""$INFORMIXDIR/extend/%SYSBLDDIR%/TraceDemo.bld(UpOne)"");

",
	"f",
	"alter function UpOne (integer)
	with (Modify External Name = ""UpOne"");

",
	"f"
);

insert into sysbldobjdepends
(
	bld_id,
	obj_kind,
	obj_signature,
	obj_owner,
	need_obj_kind,
	need_obj_signature,
	need_obj_owner)
	values
(
	"%SYSBLDNAME%",
	181, "Alter-ExternalName-UpOne (integer)",
	"%SYSBLDUSER%",
	5, "UpOne (integer)",
	"%SYSBLDUSER%"
);

insert into sysbldobjects
	(bld_id, obj_kind, obj_signature, obj_owner, sequence, create_sql,
	    create_can_fail, drop_sql, drop_can_fail)
values
(
	"%SYSBLDNAME%", 85, "Alter-Variant-UpOne (integer)",
	"%SYSBLDUSER%", 0,
	"alter function UpOne (integer)
	with (add not variant);

",
	"f",
	"alter function UpOne (integer)
	with (drop not variant);

",
	"f"
);

insert into sysbldobjdepends
(
	bld_id,
	obj_kind,
	obj_signature,
	obj_owner,
	need_obj_kind,
	need_obj_signature,
	need_obj_owner)
	values
(
	"%SYSBLDNAME%",
	85, "Alter-Variant-UpOne (integer)",
	"%SYSBLDUSER%",
	5, "UpOne (integer)",
	"%SYSBLDUSER%"
);

insert into sysbldobjects
	(bld_id, obj_kind, obj_signature, obj_owner, sequence, create_sql,
	    create_can_fail, drop_sql, drop_can_fail)
values
(
	"%SYSBLDNAME%", 69, "UpOne (integer)",
	"%SYSBLDUSER%", 0,
	"grant execute on function UpOne (integer) to public;

",
	"f",
	"",
	"f"
);

insert into sysbldobjdepends
(
	bld_id,
	obj_kind,
	obj_signature,
	obj_owner,
	need_obj_kind,
	need_obj_signature,
	need_obj_owner)
	values
(
	"%SYSBLDNAME%",
	69, "UpOne (integer)",
	"%SYSBLDUSER%",
	5, "UpOne (integer)",
	"%SYSBLDUSER%"
);

insert into sysbldobjects
	(bld_id, obj_kind, obj_signature, obj_owner, sequence, create_sql,
	    create_can_fail, drop_sql, drop_can_fail)
values
(
	"%SYSBLDNAME%", 0, "sysbldregistered",
	"%SYSBLDUSER%", 0,
	"insert into sysbldregistered
	(bld_id)
values
(
	""%SYSBLDNAME%""
);

",
	"f",
	"delete from sysbldregistered
	where bld_id = ""%SYSBLDNAME%"";

",
	"f"
);
