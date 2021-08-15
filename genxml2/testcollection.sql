-- (c) Copyright IBM Corp. 2003  All rights reserved.                 */
--                                                                    */
-- This sample program is owned by International Business Machines    */
-- Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
-- and licensed, not sold.                                            */
--                                                                    */
-- You may copy, modify, and distribute this sample program in any    */
-- form without payment to IBM,  for any purpose including developing,*/
-- using, marketing or distributing programs that include or are      */
-- derivative works of the sample program.                            */
--                                                                    */
-- The sample program is provided to you on an "AS IS" basis, without */
-- warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
-- WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
-- ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
-- limitation of implied warranties, so the above limitations or      */
-- exclusions may not apply to you.  IBM shall not be liable for any  */
-- damages you suffer as a result of using, modifying or distributing */
-- the sample program or its derivatives.                             */
--                                                                    */
-- Each copy of any portion of this sample program or any derivative  */
-- work,  must include a the above copyright notice and disclaimer of */
-- warranty.                                                          */
--                                                                    */
-- ********************************************************************/

CREATE TABLE mytab (
 col1 int,
 col2 multiset(int not null),
 col3 multiset(row(a int, b varchar(10)) not null)
);
INSERT INTO mytab
  VALUES(1, "multiset{2, 3, 4}", "multiset{ROW(2, 'two'), ROW(3, 'three')}" );
SELECT * FROM mytab;
SELECT genxml2("mytab", mytab) FROM mytab;
SELECT genxsl2("mytab_set", "mytab", mytab) FROM mytab
  WHERE col1 = 1;
SELECT gendtd2("mytab_set", "mytab", mytab) FROM mytab
  WHERE col1 = 1;
DROP TABLE mytab;

SELECT genxml2("cust_call" , ROW(fname, lname,
  MULTISET(SELECT user_id, call_code
           FROM cust_calls cc
           WHERE cc.customer_num = c.customer_num)))
FROM customer c
WHERE lname = "Parmelee";
