--(c) Copyright IBM Corp. 2003  All rights reserved.                 */
--                                                                   */
--This sample program is owned by International Business Machines    */
--Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
--and licensed, not sold.                                            */
--                                                                   */
--You may copy, modify, and distribute this sample program in any    */
--form without payment to IBM,  for any purpose including developing,*/
--using, marketing or distributing programs that include or are      */
--derivative works of the sample program.                            */
--                                                                   */
--The sample program is provided to you on an "AS IS" basis, without */
--warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
--WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
--THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
--ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
--limitation of implied warranties, so the above limitations or      */
--exclusions may not apply to you.  IBM shall not be liable for any  */
--damages you suffer as a result of using, modifying or distributing */
--the sample program or its derivatives.                             */
--                                                                   */
--Each copy of any portion of this sample program or any derivative  */
--work,  must include a the above copyright notice and disclaimer of */
--warranty.                                                          */
--                                                                   */
--********************************************************************/
CREATE TABLE tab( col1 guid);

INSERT INTO tab VALUES("2ac07282-184e-4103-ad2a-ad8abdab8f0b");
-- The next 2 generate format errors
INSERT INTO tab VALUES("2ac07282.184e-4103-ad2a-ad8abdab8f0b");
INSERT INTO tab VALUES("2ac07282-184g-4103-ad2a-ad8abdab8f0b");
-- the following generates an error on Windows
INSERT INTO tab VALUES(makeguid());

SELECT col1::lvarchar::CHAR(36) FROM tab;

SELECT * FROM  tab
WHERE col1 > "0ac07282-184e-4103-ad2a-ad8abdab8f0b";

-- DROP TABLE tab;
