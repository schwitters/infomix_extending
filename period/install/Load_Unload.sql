--
--   File:  Load_Unload.sql
--
--  About:
--
--       This script is designed to test the LOAD/UNLOAD and the back-up and
--   recovery scripts. 
--
UNLOAD TO "D:\tmp\DT_Test_1.out" 
SELECT * FROM Test_DT_Period WHERE MOD(ID,2) = 0;
--
UNLOAD TO "D:\tmp\DT_Test_2.out" 
SELECT * FROM Test_DT_Period WHERE MOD(ID,2) = 1;
--
UNLOAD TO "D:\tmp\D_Test.out" SELECT * FROM Test_D_Period;
--
DROP INDEX Test_Index_1;
DROP INDEX Test_Index_2;
--
DELETE FROM Test_DT_Period WHERE 1 = 1;
DELETE FROM Test_D_Period WHERE 1 = 1;
--
LOAD FROM "D:\tmp\DT_Test_1.out" INSERT INTO Test_DT_Period;
LOAD FROM "D:\tmp\DT_Test_2.out" INSERT INTO Test_DT_Period;
--
LOAD FROM "D:\tmp\D_Test.out" INSERT INTO Test_D_Period;
--
