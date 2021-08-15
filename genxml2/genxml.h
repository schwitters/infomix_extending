/*(c) Copyright IBM Corp. 2003  All rights reserved.                 */
/*                                                                   */
/*This sample program is owned by International Business Machines    */
/*Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
/*and licensed, not sold.                                            */
/*                                                                   */
/*You may copy, modify, and distribute this sample program in any    */
/*form without payment to IBM,  for any purpose including developing,*/
/*using, marketing or distributing programs that include or are      */
/*derivative works of the sample program.                            */
/*                                                                   */
/*The sample program is provided to you on an "AS IS" basis, without */
/*warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
/*WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
/*THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
/*ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
/*limitation of implied warranties, so the above limitations or      */
/*exclusions may not apply to you.  IBM shall not be liable for any  */
/*damages you suffer as a result of using, modifying or distributing */
/*the sample program or its derivatives.                             */
/*                                                                   */
/*Each copy of any portion of this sample program or any derivative  */
/*work,  must include a the above copyright notice and disclaimer of */
/*warranty.                                                          */
/*                                                                   */
/*********************************************************************/
#define HEADER  "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"
#define DOCTYPE "<!DOCTYPE %s SYSTEM \"%s.dtd\">\n"
#define STYLESHEET "<?xml-stylesheet type=\"text/xsl\" href=\"%s.xsl\" ?>\n"

#define GENXMLDTDTAB "genxmlinfo"
#define GENXMLCOL1   "name"
#define GENXMLCOL2   "dtypepath"
#define GENXMLCOL3   "xslpath"

#define XSLHEADER1 "<xsl:stylesheet version = '1.0'\n"
#define XSLHEADER2 " xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>\n"
#define XSLHEADER3 "<xsl:template match=\"/\">\n<HTML>\n<head/>\n"
#define XSLHEADER4 "<BODY STYLE=\"font-family:Arial, helvetica, sans-serif; font-size:12pt\" BGCOLOR=\"#FFFFFF\">\n<p/>\n"
#define XSLHEADER5 "<table border=\"1\" cellpadding=\"1\" align=\"left\" bgcolor=\"#99CCCC\" WIDTH=\"95%\">\n"

#define XSLEND "</table>\n</BODY>\n</HTML>\n</xsl:template>\n</xsl:stylesheet>"
