create function idn_SubStr (idn_mrLvarchar,integer)
returns idn_mrLvarchar
external name "$INFORMIXDIR/extend/mrLvarchar.1.1/mrLvarchar.bld(idn_mrLvarchar_SubStr_2)" language c;

alter function idn_SubStr (idn_mrLvarchar,integer)
	with (Modify External Name = "$INFORMIXDIR/extend/mrLvarchar.1.1/mrLvarchar.bld(idn_mrLvarchar_SubStr_2)");

alter function idn_SubStr (idn_mrLvarchar,integer)
	with (add parallelizable);

alter function idn_SubStr (idn_mrLvarchar,integer)
	with (add not variant);

grant execute on function idn_SubStr (idn_mrLvarchar,integer) to public;

create function idn_SubStr (idn_mrLvarchar,integer,integer)
returns idn_mrLvarchar
external name "$INFORMIXDIR/extend/mrLvarchar.1.1/mrLvarchar.bld(idn_mrLvarchar_SubStr_3)" language c;

alter function idn_SubStr (idn_mrLvarchar,integer,integer)
	with (Modify External Name = "$INFORMIXDIR/extend/mrLvarchar.1.1/mrLvarchar.bld(idn_mrLvarchar_SubStr_3)");

alter function idn_SubStr (idn_mrLvarchar,integer,integer)
	with (add parallelizable);

alter function idn_SubStr (idn_mrLvarchar,integer,integer)
	with (add not variant);

grant execute on function idn_SubStr (idn_mrLvarchar,integer,integer) to public;
