!onstat -

-- The following command sets the trace level for the trace class to 30.
-- DPRINTF statements in the source code will get output to the
-- tracefile if threshold specified in the DPRINTF statement is < 30.

execute function IDN_Debug(30);

-- We can also get the level back.

execute function IDN_Debug();

-- IDN_SessionId()  gets your client session id.
-- Since the session id may be different each time this script is
-- run, new results won't always match the baseline file. So it's
-- commented out below.

-- execute function IDN_SessionId();

-- The tracefile with debug output with be in /tmp/{SessionId}.trc.
-- While you do queries, tail -f that file.

-- Test IDN_EasyTrace()  
--
-- Remember: statements get output if trace level >= threshold
--
-- Most IDN Sampler DPRINTF statements specify a threshold of 20.
-- Since we set the level to 30 above using IDN_Debug(), all those 
-- statements would be output to the tracefile.
--
-- IDN_EasyTrace() lets you specify threshold as an argument.

execute procedure IDN_EasyTrace("This is my first message", 20);

execute procedure IDN_EasyTrace("This will not get output", 31);

execute function IDN_debug(35);

-- Since we increased the level, threshold=31 now means it
-- will get output.
execute procedure IDN_EasyTrace("Now this will get output", 31);

--
-- Test IDN_FuncExec() and IDN_FuncCache
--
create table func_test (description lvarchar, arg1 integer, arg2 integer);

insert into func_test values ( "test 1", 1, 5 );
insert into func_test values ( "test 2", 2, 5 );
insert into func_test values ( "test 3", 3, 3 );
insert into func_test values ( "test 4", 4, 5 );
insert into func_test values ( "test 5", 5, 5 );
insert into func_test values ( "test 6", 6, 5 );
insert into func_test values ( "test 7", 7, 7 );
insert into func_test values ( "test 8", 8, 5 );
insert into func_test values ( "test 9", 9, 5 );

select arg1, arg2, IDN_FuncExec(arg1, arg2) as Equal
from func_test;

select arg1, arg2, IDN_FuncExec(arg1, arg2) as Equal
from func_test
where IDN_FuncExec(arg1, arg2);


-- When you check the trace file, notice that the function
-- is "got" only once for each UDR instance in the query.

select arg1, arg2, IDN_FuncCache(arg1, arg2) as Equal
from func_test;

select arg1, arg2, IDN_FuncCache(arg1, arg2) as Equal
from func_test
where IDN_FuncCache(arg1, arg2);

drop table func_test;

--
-- Test IDN_LogMsg
--

-- create a file and insert the first message
execute procedure IDN_LogMsg('/tmp/test.log', 'This is a test.');
!cat /tmp/test.log

-- append to the file
execute procedure IDN_LogMsg('/tmp/test.log', 'This is another test.');
!cat /tmp/test.log

-- This should fail because /tmp is a directory.
execute procedure IDN_LogMsg('/tmp', 'This should fail.');


-- This should fail because path doesn't exist.
execute procedure IDN_LogMsg('/tmp/no_such_dir/foobar.txt', 'This should fail.');


--
-- Test IDN_Now() 
--

execute function idn_now(); 

--
-- Test IDN_SetNow()  
--

execute function idn_setnow("1998-01-28 11:47:05"); 

-- This should fail because "00" isn't a valid month
execute function idn_setnow("1998-00-28 11:47:05"); 

--
-- Test IDN_EOT_Reg() 
--

-- An initial select from idn_cb_test fails if that table has 
-- not been created yet. 
select * from idn_cb_test;

-- Start a transaction, register the end-of-transaction callback.
begin work;
execute function IDN_eot_reg();

-- Now commit the transaction.
commit work;

-- The table now exists and there is 1 row.
select * from idn_cb_test;

-- Do the same test, but roll the transaction back
begin work;
execute function IDN_eot_reg();
rollback work;

-- The tracefile will show that the abort entry was inserted -- but it was
-- rolled back along with all the other changes made by the transaction.
-- ==> Don't track abort states in the database.
select * from idn_cb_test;

drop table idn_cb_test;

