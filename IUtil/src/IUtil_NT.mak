!IF "$(CFG)" == ""
CFG=IUtil - Win32 DataBlade Debug
!MESSAGE No configuration specified. Defaulting to IUtil - Win32 DataBlade Debug.
!ENDIF 

!IF "$(CFG)" != "IUtil - Win32 DataBlade Release" && "$(CFG)" != "IUtil - Win32 DataBlade Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IUtil_NT.mak" CFG="IUtil - Win32 DataBlade Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IUtil - Win32 DataBlade Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IUtil - Win32 DataBlade Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "IUtil - Win32 DataBlade Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\IUtil.bld" ".\WinNT-i386\IUtil.bld"


CLEAN :
	-@erase "$(INTDIR)\ascii.obj"
	-@erase "$(INTDIR)\day.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\strings.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "$(INFORMIXDIR)\incl\public" /I "D:\Microsoft Visual Studio\VC98\Include" /I "." /I ".\c" /I "$(INFORMIXDIR)/extend/LLD.1.20.tc2/incl" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETCLIENTLOCALE_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\IUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IUtil.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib lldsapi.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\IUtil.pdb" /machine:I386 /def:".\c\IUtil.def" /out:"$(OUTDIR)\IUtil.bld" /implib:"$(OUTDIR)\IUtil.lib" /libpath:"$(INFORMIXDIR)\lib" /libpath:"$(INFORMIXDIR)\extend\lld.1.20.tc2\lib" /libpath:"D:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\IUtil.def"
LINK32_OBJS= \
	"$(INTDIR)\ascii.obj" \
	"$(INTDIR)\day.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\strings.obj"

"$(OUTDIR)\IUtil.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS) "$(OUTDIR)"
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Release\IUtil.bld
TargetName=IUtil
InputPath=.\Release\IUtil.bld
SOURCE="$(InputPath)"

".\WinNT-i386\IUtil.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ELSEIF  "$(CFG)" == "IUtil - Win32 DataBlade Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\IUtil.bld" ".\WinNT-i386\IUtil.bld"


CLEAN :
	-@erase "$(INTDIR)\ascii.obj"
	-@erase "$(INTDIR)\day.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\strings.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "D:\Microsoft Visual Studio\VC98\Include" /I "$(INFORMIXDIR)\incl\public" /I "." /I ".\c" /I "$(INFORMIXDIR)/extend/LLD.1.20.tc2/incl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETCLIENTLOCALE_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\IUtil.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IUtil.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib lldsapi.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\IUtil.pdb" /debug /machine:I386 /def:".\c\IUtil.def" /out:"$(OUTDIR)\IUtil.bld" /implib:"$(OUTDIR)\IUtil.lib" /pdbtype:sept /libpath:"$(INFORMIXDIR)\lib" /libpath:"$(INFORMIXDIR)\extend\lld.1.20.tc2\lib" /libpath:"D:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\IUtil.def"
LINK32_OBJS= \
	"$(INTDIR)\ascii.obj" \
	"$(INTDIR)\day.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\strings.obj"

"$(OUTDIR)\IUtil.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Debug\IUtil.bld
TargetName=IUtil
InputPath=.\Debug\IUtil.bld
SOURCE="$(InputPath)"

".\WinNT-i386\IUtil.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("IUtil.dep")
!INCLUDE "IUtil.dep"
!ELSE 
!MESSAGE Warning: cannot find "IUtil.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "IUtil - Win32 DataBlade Release" || "$(CFG)" == "IUtil - Win32 DataBlade Debug"
SOURCE=.\c\ascii.c

"$(INTDIR)\ascii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\day.c

"$(INTDIR)\day.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\math.c

"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=.\c\strings.c

"$(INTDIR)\strings.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)




!ENDIF 

