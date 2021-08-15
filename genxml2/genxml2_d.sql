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
DROP FUNCTION genxmlversion();
DROP FUNCTION genxml2(LVARCHAR, ROW);
DROP FUNCTION genxml2(ROW);
DROP FUNCTION genxml2(LVARCHAR, LVARCHAR);
DROP FUNCTION genxmlhdr2(LVARCHAR, LVARCHAR);
DROP FUNCTION addxmlhdr2(LVARCHAR, LVARCHAR);
DROP AGGREGATE aggrxml2;
DROP FUNCTION init_aggrxml(ROW, LVARCHAR);
DROP FUNCTION iter_aggrxml(aggrxml_t, ROW);
DROP FUNCTION comb_aggrxml(aggrxml_t, aggrxml_t);
DROP FUNCTION final_aggrxml(aggrxml_t);
DROP TYPE aggrxml_t RESTRICT;
DROP CAST (genxml_t AS lvarchar);
DROP FUNCTION castgenxml_t(genxml_t);
DROP TYPE genxml_t RESTRICT;
DROP TABLE genxmlinfo;
DROP FUNCTION genxsl2(LVARCHAR, LVARCHAR, ROW);
DROP FUNCTION gendtd2(LVARCHAR, LVARCHAR, ROW);

{
DROP FUNCTION set_tracing(lvarchar, int, lvarchar);
DELETE FROM systraceclasses WHERE name = "myclass";
}
