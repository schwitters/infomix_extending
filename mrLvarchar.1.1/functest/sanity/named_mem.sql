-- ---------------------------------------------------------------
-- 
-- Module          : Functional Test for idn_mrLvarchar UDT
-- Filename        : named_mem.sql
-- PURPOSE
-- 
--    Tests mrLvarcharTemp() UDR.
--
-- negative.sh tests passing a bad option.
--

--
-- Turn tracing on.
--
execute procedure TraceSet_mrLvarchar ("/tmp/named_mem.trc", 20);

--
-- mrLvarcharTemp() without an argument initializes the storage
-- for intermediate objects to LO_TEMP
--

execute function mrLvarcharTemp();

--
-- Set each of the options supported
--
execute function mrLvarcharTemp("PER_ROUTINE");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("PER_COMMAND");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("PER_STATEMENT");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("PER_TRANSACTION");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("PER_SESSION");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("LO");
execute function mrLvarcharTemp();

execute function mrLvarcharTemp("LO_TEMP");
execute function mrLvarcharTemp();
