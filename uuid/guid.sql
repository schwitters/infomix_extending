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
CREATE OPAQUE TYPE guid (INTERNALLENGTH=16, ALIGNMENT=4);

CREATE FUNCTION guid_in(lvarchar)
RETURNING guid
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_in)"
LANGUAGE C;

CREATE FUNCTION guid_out(guid)
RETURNING lvarchar
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_out)"
LANGUAGE C;

CREATE IMPLICIT CAST (lvarchar AS GUID WITH guid_in);
CREATE CAST (guid AS lvarchar WITH guid_out);

CREATE FUNCTION guid_imp(impexp)
RETURNING guid
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_in)"
LANGUAGE C;

CREATE FUNCTION guid_exp(guid)
RETURNING impexp
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_out)"
LANGUAGE C;

CREATE IMPLICIT CAST (impexp AS GUID WITH guid_imp);
CREATE CAST (guid AS impexp WITH guid_exp);

CREATE FUNCTION guid_send(guid)
RETURNING sendrecv
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_send)"
LANGUAGE C;

CREATE FUNCTION guid_recv(sendrecv)
RETURNING guid
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_recv)"
LANGUAGE C;

CREATE CAST (guid AS sendrecv WITH guid_send);
CREATE IMPLICIT CAST (sendrecv AS guid WITH guid_recv);

CREATE FUNCTION guid_expbin(guid)
RETURNING impexpbin
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_send)"
LANGUAGE C;

CREATE FUNCTION guid_impbin(impexpbin)
RETURNING guid
WITH(NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(guid_recv)"
LANGUAGE C;

CREATE CAST (guid AS impexpbin WITH guid_expbin);
CREATE IMPLICIT CAST (impexpbin AS guid WITH guid_impbin);

CREATE FUNCTION compare(guid, guid)
RETURNING integer
WITH (NOT VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(compare)"
LANGUAGE C;

CREATE FUNCTION lessthanorequal(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=greaterthanorequal,
      NEGATOR=greaterthan)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(lessthanorequal)"
LANGUAGE C;

CREATE FUNCTION lessthan(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=greaterthan,
      NEGATOR=greaterthanorequal)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(lessthan)"
LANGUAGE C;

CREATE FUNCTION equal(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=equal,
      NEGATOR=notequal)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(equal)"
LANGUAGE C;

CREATE FUNCTION notequal(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=notequal,
      NEGATOR=equal)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(notequal)"
LANGUAGE C;

CREATE FUNCTION greaterthan(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=lessthan,
      NEGATOR=lessthanorequal)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(greaterthan)"
LANGUAGE C;

CREATE FUNCTION greaterthanorequal(guid, guid)
RETURNING boolean
WITH (NOT VARIANT, PARALLELIZABLE, COMMUTATOR=lessthanorequal,
      NEGATOR=lessthan)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(greaterthanorequal)"
LANGUAGE C;

CREATE FUNCTION makeguid()
RETURNING guid
WITH(VARIANT, PARALLELIZABLE)
EXTERNAL NAME "$INFORMIXDIR/extend/guid/guid.bld(makeguid)"
LANGUAGE C;

