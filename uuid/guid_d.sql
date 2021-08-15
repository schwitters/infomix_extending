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
DROP CAST (lvarchar AS GUID );
DROP CAST (guid AS lvarchar );
DROP CAST (impexp AS GUID );
DROP CAST (guid AS impexp );
DROP CAST (guid AS sendrecv );
DROP CAST (sendrecv AS guid );
DROP CAST (guid AS impexpbin );
DROP CAST (impexpbin AS guid );

DROP FUNCTION compare(guid, guid);
DROP FUNCTION lessthanorequal(guid, guid);
DROP FUNCTION lessthan(guid, guid);
DROP FUNCTION equal(guid, guid);
DROP FUNCTION notequal(guid, guid);
DROP FUNCTION greaterthan(guid, guid);
DROP FUNCTION greaterthanorequal(guid, guid);

DROP FUNCTION makeguid();

DROP FUNCTION guid_in(lvarchar);
DROP FUNCTION guid_out(guid);
DROP FUNCTION guid_imp(impexp);
DROP FUNCTION guid_exp(guid);
DROP FUNCTION guid_send(guid);
DROP FUNCTION guid_recv(sendrecv);
DROP FUNCTION guid_expbin(guid);
DROP FUNCTION guid_impbin(impexpbin);

DROP TYPE guid RESTRICT;
