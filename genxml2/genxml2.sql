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
CREATE FUNCTION genxmlversion()
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxmlversion)"
LANGUAGE C;

CREATE FUNCTION genxml2(LVARCHAR, ROW)
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxml2)"
LANGUAGE C;

CREATE FUNCTION genxml2(ROW)
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxmlnamedrow)"
LANGUAGE C;

CREATE FUNCTION genxml2(LVARCHAR, LVARCHAR)
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxmlnhdr)"
LANGUAGE C;

CREATE FUNCTION genxmlhdr2(LVARCHAR, LVARCHAR)
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxmlhdr)"
LANGUAGE C;

CREATE FUNCTION addxmlhdr2(LVARCHAR, LVARCHAR)
RETURNING lvarchar
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(addxmlhdr)"
LANGUAGE C;

CREATE DISTINCT TYPE aggrxml_t AS POINTER;
CREATE OPAQUE TYPE genxml_t (INTERNALLENGTH=VARIABLE, MAXLEN=32730);

CREATE FUNCTION castgenxml_t(genxml_t)
RETURNING lvarchar
WITH (HANDLESNULLS, NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(castgenxml_t_lvar)"
LANGUAGE C;

CREATE IMPLICIT CAST (genxml_t AS lvarchar WITH castgenxml_t);

CREATE FUNCTION init_aggrxml(ROW, LVARCHAR default "Document")
RETURNING aggrxml_t
WITH (HANDLESNULLS, NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(init_aggrxml)"
LANGUAGE C;

CREATE FUNCTION iter_aggrxml(aggrxml_t, ROW)
RETURNING aggrxml_t
WITH (NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(iter_aggrxml)"
LANGUAGE C;

CREATE FUNCTION comb_aggrxml(aggrxml_t, aggrxml_t)
RETURNING aggrxml_t
WITH (NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(init_aggrxml)"
LANGUAGE C;

CREATE FUNCTION final_aggrxml(aggrxml_t)
RETURNING genxml_t
WITH (NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(final_aggrxml)"
LANGUAGE C;

CREATE AGGREGATE aggrxml2 WITH (
  INIT = init_aggrxml,
  ITER = iter_aggrxml,
  COMBINE = comb_aggrxml,
  FINAL = final_aggrxml
);

CREATE TABLE genxmlinfo (
  name       varchar(30) PRIMARY KEY,
  dtypepath  lvarchar,
  xslpath    lvarchar
);
INSERT INTO genxmlinfo
VALUES ("customer_set",
        "/usr/dtd/customer_set.dtd",
        "/usr/xls/customer_set.xls"
);
INSERT INTO genxmlinfo
VALUES ("manufact_set", "../manufact_set", "../manufact_set");

CREATE FUNCTION genxsl2(LVARCHAR, LVARCHAR, ROW)
RETURNING lvarchar(32000)
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(genxsl)"
LANGUAGE C;

CREATE FUNCTION gendtd2(LVARCHAR, LVARCHAR, ROW)
RETURNING lvarchar(32000)
WITH(NOT VARIANT)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(gendtd)"
LANGUAGE C;

{
CREATE FUNCTION set_tracing(lvarchar, int, lvarchar)
RETURNING int
WITH (not variant, handlesnulls)
EXTERNAL NAME "$INFORMIXDIR/extend/genxml2/bin/genxml.bld(set_tracing)"
LANGUAGE C;

INSERT INTO systraceclasses(name)
       VALUES("myclass");
}
