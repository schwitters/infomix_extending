--
-- demo.tpl
--
-- This is a template for the demo SQL script. We'll run it through sed
-- to convert the SERVER PORT variable to the correct number and generate
-- demo.sql
--
execute function 
idnCallSocket1(SERVER_PORT, "racecar");
--
execute function 
idnCallSocket1(SERVER_PORT, 
    "are we not drawn onward, we few, drawn onward to new era?");
--
execute function 
idnCallSocket1(SERVER_PORT, "Was it a rat I saw?");
--
execute function 
idnCallSocket1(SERVER_PORT, "pull up if I pull up");
--
-- This isn't working, is it? ;)
--
execute function
idnCallSocket1(SERVER_PORT, "Ok, this one should work!");
--
