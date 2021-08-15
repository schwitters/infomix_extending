-- run the to_decimal() routine against several valid and invalid values
--
-- should return 0:
execute function to_decimal('0');
--
-- should return 0:
execute function to_decimal('0.0');
--
-- should return 1:
execute function to_decimal('1');
--
-- should return -1:
execute function to_decimal('-1');
--
-- should return 100.123:
execute function to_decimal('100.123');
--
-- should return -100.123:
execute function to_decimal('-100.123');
--
-- should return 2147483647999:
execute function to_decimal('2147483647999');
--
-- should error:
execute function to_decimal('13A');

-- Should return 204:
execute function to_decimal('314', '%o');

-- Should return 314:
execute function to_decimal('13A', '%x');
