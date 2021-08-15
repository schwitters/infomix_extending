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

SELECT genxml2("customer", customer)
FROM customer WHERE customer_num = 101;

SELECT genxml2("customer", ROW(customer_num, fname, lname))
FROM customer WHERE customer_num = 101;

SELECT genxml2("customer",
        ROW(A.customer_num, fname, lname, call_dtime, call_code,
            call_descr, res_dtime, res_dtime, res_descr)
       )
FROM customer a, cust_calls b
WHERE a.customer_num = b.customer_num;

SELECT genXML2("stats", ROW(customer_num, COUNT(*)))
FROM cust_calls
GROUP BY customer_num;

CREATE PROCEDURE xmlcustomerset()
RETURNING LVARCHAR
DEFINE result LVARCHAR;
DEFINE ressql LVARCHAR;

LET result = '<?xml version="1.0" encoding="ISO-8859-1" ?>';
LET result = result ||
    '<!DOCTYPE customer_set SYSTEM "/home/dtd/customer_set.dtd">';
LET result = result ||
    '<?xml-stylesheet type="text/xsl" href="/home/xsl/customer_set.xsl" ?>';
LET result = result || "<customer_set>";


FOREACH SELECT genxml2("customer", customer) INTO ressql FROM customer
        WHERE customer_num = 101
  LET result = result || ressql;
END FOREACH;
LET result = result || "</customer_set>";
RETURN result;
END PROCEDURE;

EXECUTE PROCEDURE xmlcustomerset();
DROP PROCEDURE xmlcustomerset();

EXECUTE FUNCTION
  addxmlhdr2("customer_set", 
  "SELECT genxml2('customer', customer) FROM customer WHERE customer_num = 101");

EXECUTE FUNCTION
  genxml2("customer_set", "SELECT * FROM customer WHERE customer_num = 101");

EXECUTE FUNCTION
  genxmlhdr2("customer_set", "SELECT * FROM customer WHERE customer_num = 101");

SELECT aggrxml2(customer, "customer_set")
FROM customer
WHERE customer_num = 101;

