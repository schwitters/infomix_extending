SET EXPLAIN ON;

-- SELECT COUNT(*)
-- SELECT *
--   FROM Test_D_Period T
--  WHERE Within( T.When,
--               '"10/10/1999" to "10/15/1999"');

-- SELECT COUNT(*)
SELECT *
  FROM Test_D_Period T
 WHERE WithinNotTouches( T.When,
               '"10/10/1999" to "10/15/1999"');

-- SELECT --+ EXPLAIN AVOID_INDEX (T test_index_1)
-- --       COUNT(*)
--        *
--   FROM Test_D_Period T
--  WHERE Within( T.When,
--                '"10/10/1999" to "10/15/1999"');
-- 
-- SELECT --+ EXPLAIN AVOID_INDEX (T test_index_1)
-- --       COUNT(*)
--        *
--   FROM Test_D_Period T
--  WHERE WithinNotTouches( T.When,
--                '"10/10/1999" to "10/15/1999"');
