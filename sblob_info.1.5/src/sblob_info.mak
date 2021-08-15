# Microsoft Developer Studio Generated NMAKE File, Based on sblob_info.dsp
!IF "$(CFG)" == ""
CFG=sblob_info - Win32 DataBlade Debug
!MESSAGE No configuration specified. Defaulting to sblob_info - Win32 DataBlade Debug.
!ENDIF 

!IF "$(CFG)" != "sblob_info - Win32 DataBlade Release" && "$(CFG)" != "sblob_info - Win32 DataBlade Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sblob_info.mak" CFG="sblob_info - Win32 DataBlade Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sblob_info - Win32 DataBlade Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sblob_info - Win32 DataBlade Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "sblob_info - Win32 DataBlade Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\sblob_info.bld" ".\WinNT-i386\sblob_info.bld"


CLEAN :
	-@erase "$(INTDIR)\support.obj"
	-@erase "$(INTDIR)\udr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sblob_info.bld"
	-@erase "$(OUTDIR)\sblob_info.exp"
	-@erase "$(OUTDIR)\sblob_info.lib"
	-@erase ".\WinNT-i386\sblob_info.bld"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "$(INFORMIXDIR)\incl\public" -I "d:\Microsoft Visual Studio\VC98\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SBLOB_INFO_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\sblob_info.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sblob_info.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\sblob_info.pdb" /machine:I386 /def:".\c\sblob_info.def" /out:"$(OUTDIR)\sblob_info.bld" /implib:"$(OUTDIR)\sblob_info.lib" /libpath:"$(INFORMIXDIR)\lib" /libpath:"d:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\sblob_info.def"
LINK32_OBJS= \
	"$(INTDIR)\udr.obj" \
	"$(INTDIR)\support.obj"

"$(OUTDIR)\sblob_info.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Release\sblob_info.bld
TargetName=sblob_info
InputPath=.\Release\sblob_info.bld
SOURCE="$(InputPath)"

".\WinNT-i386\sblob_info.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ELSEIF  "$(CFG)" == "sblob_info - Win32 DataBlade Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\sblob_info.bld" ".\WinNT-i386\sblob_info.bld"


CLEAN :
	-@erase "$(INTDIR)\support.obj"
	-@erase "$(INTDIR)\udr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sblob_info.bld"
	-@erase "$(OUTDIR)\sblob_info.exp"
	-@erase "$(OUTDIR)\sblob_info.ilk"
	-@erase "$(OUTDIR)\sblob_info.lib"
	-@erase "$(OUTDIR)\sblob_info.pdb"
	-@erase ".\WinNT-i386\sblob_info.bld"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(INFORMIXDIR)\incl\public" /I "D:\Microsoft Visual Studio\VC98\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SBLOB_INFO_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\sblob_info.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sblob_info.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\sblob_info.pdb" /debug /machine:I386 /def:".\c\sblob_info.def" /out:"$(OUTDIR)\sblob_info.bld" /implib:"$(OUTDIR)\sblob_info.lib" /pdbtype:sept /libpath:"$(INFORMIXDIR)\lib" /libpath:"d:\Microsoft Visual Studio\VC98\Lib"
DEF_FILE= \
	".\c\sblob_info.def"
LINK32_OBJS= \
	"$(INTDIR)\udr.obj" \
	"$(INTDIR)\support.obj"

"$(OUTDIR)\sblob_info.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

ProjDir=.
TargetPath=.\Debug\sblob_info.bld
TargetName=sblob_info
InputPath=.\Debug\sblob_info.bld
SOURCE="$(InputPath)"

".\WinNT-i386\sblob_info.bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386" 
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("sblob_info.dep")
!INCLUDE "sblob_info.dep"
!ELSE 
!MESSAGE Warning: cannot find "sblob_info.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sblob_info - Win32 DataBlade Release" || "$(CFG)" == "sblob_info - Win32 DataBlade Debug"
SOURCE=.\c\support.c

"$(INTDIR)\support.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\c\udr.c

"$(INTDIR)\udr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

