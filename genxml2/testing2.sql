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
DROP TABLE employee;
DROP ROW TYPE address_t RESTRICT;

CREATE ROW TYPE address_t (
  address1 varchar(20),
  address2 varchar(20),
  city     varchar(15),
  state    char(2),
  zipcode  char(5)
);

CREATE TABLE employee (
  name     varchar(20),
  address  address_t,
  phone    varchar(18)
);
INSERT INTO employee
VALUES("Roy",
       ROW("123 first street", NULL, "Denver", "CO", "80111")::address_t,
       "303-555-1212");
-- EXECUTE FUNCTION set_tracing("myclass", 60, "/tmp/trace.pl");

SELECT genxml("employee", employee) FROM employee;
