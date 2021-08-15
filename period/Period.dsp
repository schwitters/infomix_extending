# Microsoft Developer Studio Project File - Name="Period" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Period - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Period.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Period.mak" CFG="Period - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Period - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Period - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Period - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "Period - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT" /D "MI_SERVBUILD" /D "GL_NT_STATIC" /D "NT_MI_SAPI" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sapi.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"./bin/Period.bld" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Set Period.bld to writeable
PreLink_Cmds=attrib  -r ./bin/Period.bld
PostBuild_Desc=Seting Period.bld to read-only
PostBuild_Cmds=attrib +r ./bin/Period.bld
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Period - Win32 Release"
# Name "Period - Win32 Debug"
# Begin Group "SQL_Register_Scripts"

# PROP Default_Filter "sql"
# Begin Source File

SOURCE=.\install\reg_d_period.sql
# End Source File
# Begin Source File

SOURCE=.\install\reg_dt_period.sql
# End Source File
# Begin Source File

SOURCE=.\install\unregister.sql
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\Notes.txt
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ".html"
# Begin Source File

SOURCE=.\Period.html
# End Source File
# End Group
# Begin Group "SQL Test Files"

# PROP Default_Filter "sql"
# Begin Source File

SOURCE=.\install\d_regression_min.sql
# End Source File
# Begin Source File

SOURCE=.\install\dt_regression_min.sql
# End Source File
# Begin Source File

SOURCE=.\install\dt_stats.sql
# End Source File
# Begin Source File

SOURCE=.\install\reg_index.sql
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\DT_Period.c
# End Source File
# Begin Source File

SOURCE=.\src\Period.c
# End Source File
# Begin Source File

SOURCE=.\src\Period.def
# End Source File
# Begin Source File

SOURCE=.\src\Period.h
# End Source File
# Begin Source File

SOURCE=.\src\support.c
# End Source File
# End Target
# End Project
