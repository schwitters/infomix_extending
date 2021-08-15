#
# Simple "do everything" script. Create a test database, register the VTable
# bladelet. Create a test table, seed it with a small amount of data, version
# it, and then call external SQL scripts.
#
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee  run.out
echo "Step 1: Create the vtabdb database"          | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess - - <<END_CREATE >> run.out 2>&1
create database vtabdb with log;
END_CREATE

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 2: Register the VTable bladelet"        | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb < $INFORMIXDIR/extend/VTable/register.sql > register.out 2>&1

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 3: Create a test table"                 | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb <<END_TABLE_INIT >> run.out 2>&1
create table itinerary 
(
  sequence          integer,
  city              varchar(30),
  objective         lvarchar,
  primary key (sequence)
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  1, "Boston", "Meet with executive staff"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  2, "New York", "Present paper to membership"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  3, "Washington", "Lobby congressman"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  4, "Orlando", "Consult with Mickey"
);

END_TABLE_INIT

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 4: Create a versioned table"            | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb << END_CREATE_V_TAB >> run.out 2>&1
execute procedure versioned_table_create("itinerary");
END_CREATE_V_TAB

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 5: Run the VTable routines"             | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb < run.sql > run1.out

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 6: Unregister the VTable bladelet"      | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb < $INFORMIXDIR/extend/VTable/unregister.sql > unregister.out 2>&1

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 7: Remove the itinerary table"        | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb << DROPIT >> run.out 2>&1
drop table itinerary;
DROPIT

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 8: Register the VTable bladelet again"  | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb < $INFORMIXDIR/extend/VTable/register.sql >> register.out 2>&1

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 9: Recreate the test table"             | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb <<END_TABLE_INIT2 >> run.out 2>&1
create table itinerary 
(
  sequence          integer,
  city              varchar(30),
  objective         lvarchar,
  primary key (sequence)
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  1, "Boston", "Meet with executive staff"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  2, "New York", "Present paper to membership"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  3, "Washington", "Lobby congressman"
);

insert into itinerary
(
  sequence, city, objective
)
values
(
  4, "Orlando", "Consult with Mickey"
);

END_TABLE_INIT2

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 10: Recreate the versioned table"       | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb << END_CREATE_V_TAB2 >> run.out 2>&1
execute procedure versioned_table_create("itinerary");

END_CREATE_V_TAB2

echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
echo "Step 11: Rerun the VTable routines"          | tee -a run.out
echo "- - - - - - - - - - - - - - - - - - - - - -" | tee -a run.out
dbaccess -e vtabdb < run.sql > run2.out

echo "- - - " | tee -a run.out
echo "Done. " | tee -a run.out
echo "- - - " | tee -a run.out
