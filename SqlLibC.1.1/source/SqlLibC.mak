# Microsoft Developer Studio Generated NMAKE File, Based on SqlLibC.dsp
!IF "$(CFG)" == ""
CFG=SqlLibC - Win32 Debug
!MESSAGE No configuration specified. Defaulting to SqlLibC - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SqlLibC - Win32 Release" && "$(CFG)" != "SqlLibC - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SqlLibC.mak" CFG="SqlLibC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SqlLibC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SqlLibC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SqlLibC - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\SqlLibC.bld"


CLEAN :
	-@erase "$(INTDIR)\SqlLibC.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SqlLibC.bld"
	-@erase "$(OUTDIR)\SqlLibC.exp"
	-@erase "$(OUTDIR)\SqlLibC.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SQLLIBC_EXPORTS" /Fp"$(INTDIR)\SqlLibC.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SqlLibC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\SqlLibC.pdb" /machine:I386 /def:"SqlLibC.def" /out:"$(OUTDIR)\SqlLibC.bld" /implib:"$(OUTDIR)\SqlLibC.lib" 
DEF_FILE= \
	"SqlLibC.def"
LINK32_OBJS= \
	"$(INTDIR)\SqlLibC.obj"

"$(OUTDIR)\SqlLibC.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SqlLibC - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\SqlLibC.bld"


CLEAN :
	-@erase "$(INTDIR)\SqlLibC.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SqlLibC.bld"
	-@erase "$(OUTDIR)\SqlLibC.exp"
	-@erase "$(OUTDIR)\SqlLibC.ilk"
	-@erase "$(OUTDIR)\SqlLibC.lib"
	-@erase "$(OUTDIR)\SqlLibC.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "%INFORMIXDIR%\incl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SQLLIBC_EXPORTS" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\SqlLibC.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SqlLibC.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib ifxintfc.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\SqlLibC.pdb" /debug /machine:I386 /def:"SqlLibC.def" /out:"$(OUTDIR)\SqlLibC.bld" /implib:"$(OUTDIR)\SqlLibC.lib" /pdbtype:sept /libpath:"%INFORMIXDIR%\lib" 
DEF_FILE= \
	"SqlLibC.def"
LINK32_OBJS= \
	"$(INTDIR)\SqlLibC.obj"

"$(OUTDIR)\SqlLibC.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("SqlLibC.dep")
!INCLUDE "SqlLibC.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SqlLibC - Win32 Release" || "$(CFG)" == "SqlLibC - Win32 Debug"
SOURCE=SqlLibC.c

"$(INTDIR)\SqlLibC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

