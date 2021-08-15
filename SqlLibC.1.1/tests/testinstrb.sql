-- -------------------------------------------------------------------------
-- Tests for the instrb() function.
-- -------------------------------------------------------------------------
--
-- Test with just two arguments:
--
-- Should return "6":
execute function instrb("The Mississippi flows north to south.", "i");
execute function instrb("The Mississippi flows north to south.", "is");
execute function instrb("The Mississippi flows north to south.", "issi");

-- Should return "1":
execute function instrb("The Mississippi flows north to south.", "T");
execute function instrb("The Mississippi flows north to south.", "The");

-- Should return "37":
execute function instrb("The Mississippi flows north to south.", ".");

-- Should return "0" (no match):
execute function instrb("The Mississippi flows north to south.", "east");

-- -------------------------------------------------------------------------
-- Test with three arguments:
-- -------------------------------------------------------------------------
-- Should return "6":
execute function instrb("The Mississippi flows north to south.", "i", 0);
execute function instrb("The Mississippi flows north to south.", "is", 0);
execute function instrb("The Mississippi flows north to south.", "issi", 0);
execute function instrb("The Mississippi flows north to south.", "i", 1);
execute function instrb("The Mississippi flows north to south.", "is", 1);
execute function instrb("The Mississippi flows north to south.", "issi", 1);
execute function instrb("The Mississippi flows north to south.", "issi", 6);

-- Should return "9":
execute function instrb("The Mississippi flows north to south.", "i", 7);
execute function instrb("The Mississippi flows north to south.", "is", 7);
execute function instrb("The Mississippi flows north to south.", "issi", 7);
execute function instrb("The Mississippi flows north to south.", "issi", 8);
execute function instrb("The Mississippi flows north to south.", "issi", 9);

-- Should return "35":
execute function instrb("The Mississippi flows north to south.", "th", -1);
execute function instrb("The Mississippi flows north to south.", "th", -2);
execute function instrb("The Mississippi flows north to south.", "th", -3);

-- Should return "26":
execute function instrb("The Mississippi flows north to south.", "th", -4);
execute function instrb("The Mississippi flows north to south.", "th", -5);

-- Should return "9":
execute function instrb("The Mississippi flows north to south.", "issi", -1);
execute function instrb("The Mississippi flows north to south.", "issi", -26);
execute function instrb("The Mississippi flows north to south.", "issi", -27);
execute function instrb("The Mississippi flows north to south.", "issi", -29);

-- Should return "6":
execute function instrb("The Mississippi flows north to south.", "issi", -30);
execute function instrb("The Mississippi flows north to south.", "issi", -31);
execute function instrb("The Mississippi flows north to south.", "issi", -32);

-- Should return "0" (not found):
execute function instrb("The Mississippi flows north to south.", "issi", -33);


-- -------------------------------------------------------------------------
-- Test with four arguments:
-- -------------------------------------------------------------------------
-- Should return "6":
execute function instrb("The Mississippi flows north to south.", "i", 0, 1);
execute function instrb("The Mississippi flows north to south.", "is", 0, 1);
execute function instrb("The Mississippi flows north to south.", "issi", 0, 1);
execute function instrb("The Mississippi flows north to south.", "i", 1, 1);
execute function instrb("The Mississippi flows north to south.", "is", 1, 1);
execute function instrb("The Mississippi flows north to south.", "issi", 1, 1);

-- Should return "9":
execute function instrb("The Mississippi flows north to south.", "i", 0, 2);
execute function instrb("The Mississippi flows north to south.", "is", 0, 2);
execute function instrb("The Mississippi flows north to south.", "issi", 0, 2);
execute function instrb("The Mississippi flows north to south.", "i", 1, 2);
execute function instrb("The Mississippi flows north to south.", "is", 1, 2);
execute function instrb("The Mississippi flows north to south.", "issi", 1, 2);

-- Should return "0" (no match):
execute function instrb("The Mississippi flows north to south.", "issi", 10, 1);

-- Should return "26":
execute function instrb("The Mississippi flows north to south.", "th", 1, 1);
execute function instrb("The Mississippi flows north to south.", "th", 10, 1);
execute function instrb("The Mississippi flows north to south.", "th", 26, 1);

-- Should return "35":
execute function instrb("The Mississippi flows north to south.", "th", 1, 2);
execute function instrb("The Mississippi flows north to south.", "th", 10, 2);
execute function instrb("The Mississippi flows north to south.", "th", 26, 2);
execute function instrb("The Mississippi flows north to south.", "th", 27, 1);

-- Should return "15":
execute function instrb("The Mississippi flows north to south.", "i", -1, 1);
execute function instrb("The Mississippi flows north to south.", "i", -22, 1);
execute function instrb("The Mississippi flows north to south.", "i", -23, 1);

-- Should return "12":
execute function instrb("The Mississippi flows north to south.", "i", -24, 1);

-- Should return "9":
execute function instrb("The Mississippi flows north to south.", "issi", -1, 1);
execute function instrb("The Mississippi flows north to south.", "issi", -28, 1);
execute function instrb("The Mississippi flows north to south.", "issi", -29, 1);

-- Should return "6":
execute function instrb("The Mississippi flows north to south.", "issi", -1, 2);
execute function instrb("The Mississippi flows north to south.", "issi", -30, 1);

-- Specify a negative occurrence number (should error):
execute function instrb("The Mississippi flows north to south.", "issi", -1, -1);


