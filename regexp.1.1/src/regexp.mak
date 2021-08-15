# Microsoft Developer Studio Generated NMAKE File, Based on regexp.dsp
!IF "$(CFG)" == ""
CFG=regexp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to regexp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "regexp - Win32 Release" && "$(CFG)" != "regexp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "regexp.mak" CFG="regexp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "regexp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "regexp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "regexp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\regexp.dll"


CLEAN :
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\regsub.obj"
	-@erase "$(INTDIR)\sqlfuncs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\regexp.dll"
	-@erase "$(OUTDIR)\regexp.exp"
	-@erase "$(OUTDIR)\regexp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "REGEXP_EXPORTS" /Fp"$(INTDIR)\regexp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\regexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\regexp.pdb" /machine:I386 /def:"c\regexp.def" /out:"$(OUTDIR)\regexp.dll" /implib:"$(OUTDIR)\regexp.lib" 
DEF_FILE= \
	"c\regexp.def"
LINK32_OBJS= \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\regsub.obj" \
	"$(INTDIR)\sqlfuncs.obj"

"$(OUTDIR)\regexp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "regexp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\regexp.bld"


CLEAN :
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\regsub.obj"
	-@erase "$(INTDIR)\sqlfuncs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\regexp.bld"
	-@erase "$(OUTDIR)\regexp.exp"
	-@erase "$(OUTDIR)\regexp.ilk"
	-@erase "$(OUTDIR)\regexp.lib"
	-@erase "$(OUTDIR)\regexp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "%INFORMIXDIR%\incl\public %INFORMIXDIR\incl\esqlc ." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "REGEXP_EXPORTS" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /Fp"$(INTDIR)\regexp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\regexp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib ifxintfc.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\regexp.pdb" /debug /machine:I386 /def:"c\regexp.def" /out:"$(OUTDIR)\regexp.bld" /implib:"$(OUTDIR)\regexp.lib" /pdbtype:sept /libpath:"%INFORMIXDIR%\lib" 
DEF_FILE= \
	"c\regexp.def"
LINK32_OBJS= \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\regsub.obj" \
	"$(INTDIR)\sqlfuncs.obj"

"$(OUTDIR)\regexp.bld" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("regexp.dep")
!INCLUDE "regexp.dep"
!ELSE 
!MESSAGE Warning: cannot find "regexp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "regexp - Win32 Release" || "$(CFG)" == "regexp - Win32 Debug"
SOURCE=c\regerror.c

"$(INTDIR)\regerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=c\regexp.c

"$(INTDIR)\regexp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=c\regsub.c

"$(INTDIR)\regsub.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=c\sqlfuncs.c

"$(INTDIR)\sqlfuncs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

