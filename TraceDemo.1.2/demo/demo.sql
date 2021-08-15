--
-- Turn on tracing for the current session, then call the UpOne function.
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 10);
--
-- execute the UpOne() function.
execute function upone(100);
--
-- Increase the trace threshhold and call UpOne again.
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 20);
--
-- execute the UpOne() function.
execute function upone(200);
--
-- Increase the trace threshhold yet again.
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 30);
--
-- execute the UpOne() function.
execute function upone(300);
--
-- Increase the trace threshhold to 40
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 40);
--
-- execute the UpOne() function.
execute function upone(400);
--
-- Increase the trace threshhold to 50
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 50);
--
-- execute the UpOne() function.
execute function upone(500);
--
-- Increase the trace threshhold to 60
--
execute procedure traceset_tracedemo("/tmp/TraceDemo.trc", 60);
--
-- execute the UpOne() function.
execute function upone(600);
