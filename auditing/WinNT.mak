# This Makefile builds the shared library
# (c) Copyright IBM Corp. 2004  All rights reserved.                 */
#                                                                    */
# This sample program is owned by International Business Machines    */
# Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
# and licensed, not sold.                                            */
#                                                                    */
# You may copy, modify, and distribute this sample program in any    */
# form without payment to IBM,  for any purpose including developing,*/
# using, marketing or distributing programs that include or are      */
# derivative works of the sample program.                            */
#                                                                    */
# The sample program is provided to you on an "AS IS" basis, without */
# warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
# WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
# ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
# limitation of implied warranties, so the above limitations or      */
# exclusions may not apply to you.  IBM shall not be liable for any  */
# damages you suffer as a result of using, modifying or distributing */
# the sample program or its derivatives.                             */
#                                                                    */
# Each copy of any portion of this sample program or any derivative  */
# work,  must include a the above copyright notice and disclaimer of */
# warranty.                                                          */
#                                                                    */
# ********************************************************************/

# This is the project title.
PROJECT_TITLE = auditing

# Insure that INFORMIXDIR is set.
!IF "$(INFORMIXDIR)" == ""
!MESSAGE
!MESSAGE The INFORMIXDIR environment variable is not set.
!MESSAGE This variable must be be set to the location of
!MESSAGE the Informix installation.  This is typically
!MESSAGE X:\informix  where "X" is the drive letter.
!MESSAGE Use "set INFORMIXDIR X:\informix" to set this
!MESSAGE variable from a DOS command prompt or the System
!MESSAGE entry in the Control Panel to set it permanently.
!MESSAGE
!MESSAGE INFORMIXDIR has defaulted to $(HOMEDRIVE)\informix.
!MESSAGE
INFORMIXDIR=$(HOMEDRIVE)\informix
!ENDIF

# NT-specific code goes here.
MI_INCLUDE="$(INFORMIXDIR)"\incl
RM= del
RMFLAGS= /q
CC= cl
CSRVRFLAGS= -DNT -DMI_SERVBUILD	-DGL_NT_STATIC -DNT_MI_SAPI
COPTS= -O -DMITRACE_OFF=1
# COPTS= -O 
CFLAGS=$(CSRVRFLAGS) -DWIN32  -I$(MI_INCLUDE)\public -I$(MI_INCLUDE) $(COPTS)
LINK= link
LINKFLAGS= /DLL /nologo $(LOPTS)
LIBS= "$(INFORMIXDIR)"\lib\sapi.lib
BINDIR= WinNT-i386
PRODDIR= .

# Platform independent code goes here.
# The following code was generated by BladeSmith.

all: $(BINDIR) RecordAudit.jar
	$(MAKE) -$(MAKEFLAGS) -f WinNT.mak server

.c.obj:
	$(CC) $(CFLAGS) /Fo$*.obj /c $?

# List all object files.
OBJ1=$(BINDIR)/auditing1.obj $(BINDIR)/auditing2.obj $(BINDIR)/audit_util.obj
OBJ2=$(BINDIR)/auditing3.obj

OBJS_ALL=$(OBJ1) $(OBJ2)

# Construct the object file.
$(BINDIR)/auditing1.obj: auditing1.c
	$(CC) $(CFLAGS) /Fo$*.obj /c $?

$(BINDIR)/auditing2.obj: auditing2.c
	$(CC) $(CFLAGS) /Fo$*.obj /c $?

$(BINDIR)/auditing3.obj: auditing3.c
	$(CC) $(CFLAGS) /Fo$*.obj /c $?

$(BINDIR)/audit_util.obj: audit_util.c
	$(CC) $(CFLAGS) /Fo$*.obj /c $?

RecordAudit.class: RecordAudit.java
	javac RecordAudit.java

RecordAudit.jar: RecordAudit.class
	jar cf RecordAudit.jar RecordAudit.class

# Construct the DLL library.
$(PRODDIR)\\$(PROJECT_TITLE).bld: $(OBJS_ALL)
	attrib -R $(PRODDIR)\\$(PROJECT_TITLE).bld > nul
	$(LINK) $(LINKFLAGS) /out:$(PRODDIR)\\$(PROJECT_TITLE).bld /def:$(PROJECT_TITLE).def $(OBJS_ALL) $(LIBS)
	attrib +R $(PRODDIR)\\$(PROJECT_TITLE).bld

server: $(PRODDIR)\\$(PROJECT_TITLE).bld

clean:
	attrib -R $(PRODDIR)\\$(PROJECT_TITLE).bld > nul
	$(RM) $(RMFLAGS) $(PRODDIR)\\$(PROJECT_TITLE).bld
	$(RM) $(RMFLAGS) $(PRODDIR)\\$(PROJECT_TITLE).exp
	$(RM) $(RMFLAGS) $(PRODDIR)\\$(PROJECT_TITLE).lib
	$(RM) $(RMFLAGS) $(BINDIR)\\*.obj

$(BINDIR):
	-mkdir $(BINDIR)

INSTALL:
	attrib -R "$(INFORMIXDIR)"\extend\auditing\$(PROJECT_TITLE).bld
	COPY $(PROJECT_TITLE).bld "$(INFORMIXDIR)"\extend\$(PROJECT_TITLE)
	attrib +R "$(INFORMIXDIR)"\extend\auditing\$(PROJECT_TITLE).bld
	COPY RecordAudit.jar "$(INFORMIXDIR)"\extend\auditing\RecordAudit.jar
