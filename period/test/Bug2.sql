--
--   File:  Bug2.sql
--
--  About:
--
--        Run this too.
--
SET EXPLAIN ON;
--
SELECT 'There should be 5 of these, and there are ' || COUNT(*) 
  FROM Reservations V
 WHERE Overlap ( Period ( V.Check_In, V.Check_Out ),
                 Period ( DATE('06/05/1998'),
                                 DATE('06/10/1998')
                               )
               );
--
