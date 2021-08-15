# Microsoft Developer Studio Generated NMAKE File, Based on VTable.dsp
!IF "$(CFG)" == ""
CFG=VTable - Win32 DataBlade Debug
!MESSAGE No configuration specified. Defaulting to VTable - Win32 DataBlade Debug.
!ENDIF 

!IF "$(CFG)" != "VTable - Win32 DataBlade Release" && "$(CFG)" != "VTable - Win32 DataBlade Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VTable_NT.mak" CFG="VTable - Win32 DataBlade Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VTable - Win32 DataBlade Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VTable - Win32 DataBlade Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "VTable - Win32 DataBlade Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\VTable.bld" ".\WinNT-i386\VTable.bld"


CLEAN :
	-@erase "$(INTDIR)\ddl_exec.obj"
	-@erase "$(INTDIR)\vt_beginscan.obj"
	-@erase "$(INTDIR)\vt_endscan.obj"
	-@erase "$(INTDIR)\vt_getnext.obj"
	-@erase "$(INTDIR)\vt_rescan.obj"
	-@erase "$(INTDIR)\vt_scancost.obj"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(OUTDIR)\VTable.bld"
	-@erase "$(OUTDIR)\VTable.exp"
	-@erase "$(OUTDIR)\VTable.lib"
	-@erase ".\WinNT-i386\VTable.bld"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "$(INFORMIXDIR)\incl\public" /I "D:\Microsoft Visual Studio\VC98\Include" /I "." /I ".\c" /I "$(INFORMIXDIR)/extend/LLD.1.20.tc2/incl" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETCLIENTLOCALE_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\VTable.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VTable.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib lldsapi.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\VTable.pdb" /machine:I386 /def:".\c\VTable.def" /out:"$(OUTDIR)\VTable.bld" /implib:"$(OUTDIR)\VTable.lib" /libpath:"$(INFORMIXDIR)\lib" /libpath:"$(INFORMIXDIR)\extend\lld.1.20.tc2\lib" /libpath:"D:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\VTable.def"
LINK32_OBJS= \
	"$(INTDIR)\ddl_exec.obj" \
	"$(INTDIR)\vt_beginscan.obj" \
	"$(INTDIR)\vt_endscan.obj" \
	"$(INTDIR)\vt_getnext.obj" \
	"$(INTDIR)\vt_rescan.obj" \
	"$(INTDIR)\vt_scancost.obj" \
	"$(OUTDIR)\vtable.obj"

"$(OUTDIR)\VTable.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS) "$(OUTDIR)"
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Release\VTable.bld
TargetName=VTable
InputPath=.\Release\VTable.bld
SOURCE="$(InputPath)"

".\WinNT-i386\VTable.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ELSEIF  "$(CFG)" == "VTable - Win32 DataBlade Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\VTable.bld" ".\WinNT-i386\VTable.bld"


CLEAN :
	-@erase "$(INTDIR)\ddl_exec.obj"
	-@erase "$(INTDIR)\vt_beginscan.obj"
	-@erase "$(INTDIR)\vt_endscan.obj"
	-@erase "$(INTDIR)\vt_getnext.obj"
	-@erase "$(INTDIR)\vt_rescan.obj"
	-@erase "$(INTDIR)\vt_scancost.obj"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(OUTDIR)\VTable.bld"
	-@erase "$(OUTDIR)\VTable.exp"
	-@erase "$(OUTDIR)\VTable.lib"
	-@erase ".\WinNT-i386\VTable.bld"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "D:\Microsoft Visual Studio\VC98\Include" /I "$(INFORMIXDIR)\incl\public" /I "." /I ".\c" /I "$(INFORMIXDIR)/extend/LLD.1.20.tc2/incl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETCLIENTLOCALE_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\VTable.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\VTable.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib lldsapi.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\VTable.pdb" /debug /machine:I386 /def:".\c\VTable.def" /out:"$(OUTDIR)\VTable.bld" /implib:"$(OUTDIR)\VTable.lib" /pdbtype:sept /libpath:"$(INFORMIXDIR)\lib" /libpath:"$(INFORMIXDIR)\extend\lld.1.20.tc2\lib" /libpath:"D:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\VTable.def"
LINK32_OBJS= \
	"$(INTDIR)\ddl_exec.obj" \
	"$(INTDIR)\vt_beginscan.obj" \
	"$(INTDIR)\vt_endscan.obj" \
	"$(INTDIR)\vt_getnext.obj" \
	"$(INTDIR)\vt_rescan.obj" \
	"$(INTDIR)\vt_scancost.obj" \
	"$(OUTDIR)\vtable.obj"

"$(OUTDIR)\VTable.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Debug\VTable.bld
TargetName=VTable
InputPath=.\Debug\VTable.bld
SOURCE="$(InputPath)"

".\WinNT-i386\VTable.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("VTable.dep")
!INCLUDE "VTable.dep"
!ELSE 
!MESSAGE Warning: cannot find "VTable.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "VTable - Win32 DataBlade Release" || "$(CFG)" == "VTable - Win32 DataBlade Debug"
SOURCE=.\c\ddl_exec.c

"$(INTDIR)\ddl_exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=.\c\vt_beginscan.c

"$(INTDIR)\vt_beginscan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\vt_endscan.c

"$(INTDIR)\vt_endscan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\vt_getnext.c

"$(INTDIR)\vt_getnext.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=.\c\vt_rescan.c

"$(INTDIR)\vt_rescan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\vt_scancost.c

"$(INTDIR)\vt_scancost.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\vtable.c

"$(INTDIR)\vtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

