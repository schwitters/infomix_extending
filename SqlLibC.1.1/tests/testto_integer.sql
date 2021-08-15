-- run the to_integer() routine against several valid and invalid values
--
-- should return 0:
execute function to_integer('0');
--
-- should return 0:
execute function to_integer('0.0');
--
-- should return 1:
execute function to_integer('1');
--
-- should return -1:
execute function to_integer('-1');
--
-- should return 100:
execute function to_integer('100.123');
--
-- should return -100:
execute function to_integer('-100.123');
--
-- should return return an error that value exceeds limit:
execute function to_integer('2147483647999');
--
-- should error:
execute function to_integer('13A');

-- Should return 204:
execute function to_integer('314', '%o');

-- Should return 314:
execute function to_integer('13A', '%x');
