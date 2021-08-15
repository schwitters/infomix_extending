-- run the to_float() routine against several valid and invalid values
--
-- should return 0:
execute function to_float('0');
--
-- should return 0:
execute function to_float('0.0');
--
-- should return 1:
execute function to_float('1');
--
-- should return -1:
execute function to_float('-1');
--
-- should return 100.123:
execute function to_float('100.123');
--
-- should return -100.123:
execute function to_float('-100.123');
--
-- should return 2147483647999:
execute function to_float('2147483647999');
--
-- should error:
execute function to_float('13A');

-- Should return 204:
execute function to_float('314', '%o');

-- Should return 314:
execute function to_float('13A', '%x');
