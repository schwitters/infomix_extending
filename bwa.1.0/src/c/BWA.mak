# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=BWA - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to BWA - Win32 Debug.
!ENDIF

!IF "$(CFG)" != "BWA - Win32 Release" && "$(CFG)" != "BWA - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE
!MESSAGE NMAKE /f "BWA.mak" CFG="BWA - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "BWA - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BWA - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
################################################################################
# Begin Project
# PROP Target_Last_Scanned "BWA - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "BWA - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\BWA.bld" "WinNT-i386\BWA.bld"

CLEAN :
	-@erase "$(OUTDIR)\support.obj"
	-@erase "$(OUTDIR)\udr.obj"
	-@erase "$(OUTDIR)\BWA.obj"
	-@erase "$(OUTDIR)\BWA.bld"
	-@erase "$(OUTDIR)\BWA.exp"
	-@erase "$(OUTDIR)\BWA.lib"
	-@erase "$(OUTDIR)\BWA.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"WinNT-i386" :
    if not exist "WinNT-i386/$(NULL)" mkdir "WinNT-i386"

# ADD BASE CPP /nologo /MT /W3  /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3  /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /YX /c
CPP_PROJ=/nologo /MT /W3  /GX /O2 \
 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MI_SERVBUILD"\
 /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)/BWA.pch" /YX\
 /Fo"$(INTDIR)/" /c
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BWA.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib msvcrt.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib /out:"Release/BWA.bld"
# SUBTRACT LINK32 /incremental:yes /debug
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib sapi.lib msvcrt.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)/BWA.pdb" /map:"$(INTDIR)/BWA.map"\
 /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib /def:".\BWA.def"\
 /out:"$(OUTDIR)/BWA.bld" /implib:"$(OUTDIR)/BWA.lib"
DEF_FILE= \
	".\BWA.def"
LINK32_OBJS= \
	"$(INTDIR)\support.obj" \
	"$(INTDIR)\udr.obj" \
	"$(INTDIR)\BWA.obj"
"$(OUTDIR)\BWA.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Duplicating DataBlade module in WinNt-i386 directory.
InputPath=.\WinNT-i386\BWA.bld
SOURCE=$(InputPath)

"WinNT-i386\BWA.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   attrib -R WinNT-i386\BWA.bld > nul
   copy .\Release\BWA.bld WinNT-i386
   attrib +R WinNT-i386\BWA.bld > nul
   echo BWA.bld is the Release version of the BWA DataBlade module. > WinNT-i386\buildinfo.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "BWA - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\BWA.bld" "WinNT-i386\BWA.bld"

CLEAN :
	-@erase "$(OUTDIR)\support.obj"
	-@erase "$(OUTDIR)\udr.obj"
	-@erase "$(OUTDIR)\BWA.obj"
	-@erase "$(OUTDIR)\BWA.bld"
	-@erase "$(OUTDIR)\BWA.exp"
	-@erase "$(OUTDIR)\BWA.lib"
	-@erase "$(OUTDIR)\BWA.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

".\WinNT-i386" :
    if not exist "WinNT-i386/$(NULL)" mkdir "WinNT-i386"

# ADD BASE CPP /nologo /MTd /W3 /Gm  /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3  /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3  /GX /Z7 /Od \
/D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MI_SERVBUILD"\
 /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)/BWA.pch" /YX\
 /Fo"$(INTDIR)/" /c
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/BWA.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib msvcrt.lib /nologo /subsystem:windows /dll /pdb:none /map /debug /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib /out:"Debug/BWA.bld"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib sapi.lib msvcrt.lib /nologo /subsystem:windows /dll /pdb:none\
 /map:"$(INTDIR)/BWA.map" /debug /machine:I386 /nodefaultlib:"libcmt.lib"\
 /nodefaultlib /def:".\BWA.def" /out:"$(OUTDIR)/BWA.bld" /implib:"$(OUTDIR)/BWA.lib"
DEF_FILE= \
	".\BWA.def"
LINK32_OBJS= \
	"$(INTDIR)\support.obj" \
	"$(INTDIR)\udr.obj" \
	"$(INTDIR)\BWA.obj"
"$(OUTDIR)\BWA.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

# Begin Custom Build - Duplicating DataBlade module in WinNt-i386 directory.
InputPath=.\WinNT-i386\BWA.bld
SOURCE=$(InputPath)

"WinNT-i386\BWA.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   attrib -R WinNT-i386\BWA.bld > nul
   copy .\Debug\BWA.bld WinNT-i386
   attrib +R WinNT-i386\BWA.bld > nul
   echo BWA.bld is the Debug version of the BWA DataBlade module. > WinNT-i386\buildinfo.txt

# End Custom Build

!ENDIF

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

################################################################################
# Begin Target

# Name "BWA - Win32 Release"
# Name "BWA - Win32 Debug"

!IF  "$(CFG)" == "BWA - Win32 Release"

!ELSEIF  "$(CFG)" == "BWA - Win32 Debug"

!ENDIF

################################################################################
# Begin Source File

SOURCE=.\support.c
DEP_CPP_support=\
	".\BWA.h"\
	{$(INCLUDE)}"blob.h"\
	{$(INCLUDE)}"collct.h"\
	{$(INCLUDE)}"datetime.h"\
	{$(INCLUDE)}"decimal.h"\
	{$(INCLUDE)}"gls.h"\
	{$(INCLUDE)}"ifxgls.h"\
	{$(INCLUDE)}"int8.h"\
	{$(INCLUDE)}"locator.h"\
	{$(INCLUDE)}"memdur.h"\
	{$(INCLUDE)}"mi.h"\
	{$(INCLUDE)}"miback.h"\
	{$(INCLUDE)}"miconv.h"\
	{$(INCLUDE)}"milib.h"\
	{$(INCLUDE)}"milo.h"\
	{$(INCLUDE)}"miloback.h"\
	{$(INCLUDE)}"mitrace.h"\
	{$(INCLUDE)}"mitypes.h"\
	{$(INCLUDE)}"sqlca.h"\
	{$(INCLUDE)}"sqlda.h"\
	{$(INCLUDE)}"sqlhdr.h"\
	{$(INCLUDE)}"value.h"\
	{$(INCLUDE)}"\sys\types.h"\


"$(INTDIR)\support.obj" : $(SOURCE) $(DEP_CPP_support) "$(INTDIR)"

# End Source File
################################################################################
# Begin Source File

SOURCE=.\udr.c
DEP_CPP_udr=\
	".\BWA.h"\
	{$(INCLUDE)}"blob.h"\
	{$(INCLUDE)}"collct.h"\
	{$(INCLUDE)}"datetime.h"\
	{$(INCLUDE)}"decimal.h"\
	{$(INCLUDE)}"gls.h"\
	{$(INCLUDE)}"ifxgls.h"\
	{$(INCLUDE)}"int8.h"\
	{$(INCLUDE)}"locator.h"\
	{$(INCLUDE)}"memdur.h"\
	{$(INCLUDE)}"mi.h"\
	{$(INCLUDE)}"miback.h"\
	{$(INCLUDE)}"miconv.h"\
	{$(INCLUDE)}"milib.h"\
	{$(INCLUDE)}"milo.h"\
	{$(INCLUDE)}"miloback.h"\
	{$(INCLUDE)}"mitrace.h"\
	{$(INCLUDE)}"mitypes.h"\
	{$(INCLUDE)}"sqlca.h"\
	{$(INCLUDE)}"sqlda.h"\
	{$(INCLUDE)}"sqlhdr.h"\
	{$(INCLUDE)}"value.h"\
	{$(INCLUDE)}"\sys\types.h"\


"$(INTDIR)\udr.obj" : $(SOURCE) $(DEP_CPP_udr) "$(INTDIR)"

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BWA.c
DEP_CPP_BWA=\
	".\BWA.h"\
	{$(INCLUDE)}"blob.h"\
	{$(INCLUDE)}"collct.h"\
	{$(INCLUDE)}"datetime.h"\
	{$(INCLUDE)}"decimal.h"\
	{$(INCLUDE)}"gls.h"\
	{$(INCLUDE)}"ifxgls.h"\
	{$(INCLUDE)}"int8.h"\
	{$(INCLUDE)}"locator.h"\
	{$(INCLUDE)}"memdur.h"\
	{$(INCLUDE)}"mi.h"\
	{$(INCLUDE)}"miback.h"\
	{$(INCLUDE)}"miconv.h"\
	{$(INCLUDE)}"milib.h"\
	{$(INCLUDE)}"milo.h"\
	{$(INCLUDE)}"miloback.h"\
	{$(INCLUDE)}"mitrace.h"\
	{$(INCLUDE)}"mitypes.h"\
	{$(INCLUDE)}"sqlca.h"\
	{$(INCLUDE)}"sqlda.h"\
	{$(INCLUDE)}"sqlhdr.h"\
	{$(INCLUDE)}"value.h"\
	{$(INCLUDE)}"\sys\types.h"\


"$(INTDIR)\BWA.obj" : $(SOURCE) $(DEP_CPP_BWA) "$(INTDIR)"

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "BWA - Win32 Release"

!ELSEIF  "$(CFG)" == "BWA - Win32 Debug"

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BWA.def

!IF  "$(CFG)" == "BWA - Win32 Release"

!ELSEIF  "$(CFG)" == "BWA - Win32 Debug"

!ENDIF

# End Source File
# End Target
# End Project
################################################################################
