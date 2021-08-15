# Microsoft Developer Studio Project File - Name="idn_fifo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=idn_fifo - Win32 DataBlade Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "idn_fifo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "idn_fifo.mak" CFG="idn_fifo - Win32 DataBlade Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "idn_fifo - Win32 DataBlade Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "idn_fifo - Win32 DataBlade Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "idn_fifo - Win32 DataBlade Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IDN_FIFO_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(INFORMIXDIR)\incl\public" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IDN_FIFO_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib /nologo /dll /machine:I386 /out:".\Release\idn_fifo.bld" /libpath:"$(INFORMIXDIR)\lib"
# Begin Custom Build - Copying files...
ProjDir=.
TargetPath=.\Release\idn_fifo.bld
TargetName=idn_fifo
InputPath=.\Release\idn_fifo.bld
SOURCE="$(InputPath)"

"$(ProjDir)\WinNT-i386\$(TargetName).bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386"
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"

# End Custom Build

!ELSEIF  "$(CFG)" == "idn_fifo - Win32 DataBlade Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IDN_FIFO_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(INFORMIXDIR)\incl\public" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IDN_FIFO_EXPORTS" /D "DK_SERVER" /D "MI_SERVBUILD" /D "NT" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /YX /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /out:".\Debug\idn_fifo.bld" /libpath:"$(INFORMIXDIR)\lib"
# Begin Custom Build - Copying files...
ProjDir=.
TargetPath=.\Debug\idn_fifo.bld
TargetName=idn_fifo
InputPath=.\Debug\idn_fifo.bld
SOURCE="$(InputPath)"

"$(ProjDir)\WinNT-i386\$(TargetName).bld" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(ProjDir)\WinNT-i386\nul" mkdir "$(ProjDir)\WinNT-i386"
	copy "$(TargetPath)" "$(ProjDir)\WinNT-i386\$(TargetName).bld"

# End Custom Build

!ENDIF 

# Begin Target

# Name "idn_fifo - Win32 DataBlade Release"
# Name "idn_fifo - Win32 DataBlade Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"

# Begin Source File

SOURCE=.\c\udr.c
# End Source File
# Begin Source File

SOURCE=.\c\support.c
# End Source File
# Begin Source File

SOURCE=.\c\idn_fifo.def
# End Source File
# End Group

# Begin Group "Header Files"
# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group

# Begin Group "Document Files"
# PROP Default_Filter "txt;doc;xls;ppt;obd;mdb"
# Begin Source File

SOURCE=.\c\readme.txt
# End Source File
# Begin Source File

SOURCE=.\c\warnings.txt
# End Source File
# End Group

# Begin Group "Resource Files"
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Test Files"

# PROP Default_Filter "*.sql"
# Begin Source File

SOURCE=.\tests\Setup.sql
# End Source File
# Begin Source File

SOURCE=.\tests\Cleanup.sql
# End Source File
# Begin Source File

SOURCE=.\tests\idnCallFIFO.sql
# End Source File
# End Group

# End Target
# End Project
