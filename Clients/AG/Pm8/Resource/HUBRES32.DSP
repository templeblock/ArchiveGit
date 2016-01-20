# Microsoft Developer Studio Project File - Name="hubres32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=hubres32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HUBRES32.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HUBRES32.MAK" CFG="hubres32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hubres32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hubres32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hubres32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\hubres32\Release"
# PROP BASE Intermediate_Dir ".\hubres32\Release"
# PROP BASE Target_Dir ".\hubres32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\winRel\PMHub"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\hubres32"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Ot /Oi /Oy /Ob1 /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Ox /Oa /Ow
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\winRel/PMW.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:".\winRel\hubres32.pdb" /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "hubres32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\hubres32\Debug"
# PROP BASE Intermediate_Dir ".\hubres32\Debug"
# PROP BASE Target_Dir ".\hubres32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\winDebug\PMHub"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\hubres32"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\winDebug\PMW.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:".\winDebug\hubres32.pdb" /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "hubres32 - Win32 Release"
# Name "hubres32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Hubres.c
# End Source File
# Begin Source File

SOURCE=.\Hubres.rc
# End Source File
# Begin Source File

SOURCE=.\Hubres32.def
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Shared\hubaddw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubadup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubagdw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubagup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubamdw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubamdwn.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubamup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubamupn.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubback.BMP
# End Source File
# Begin Source File

SOURCE=.\Shared\hubbndw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubbnup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubcadw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubcaup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubindw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubinup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubrmdw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubrmup.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubwhdw.bmp
# End Source File
# Begin Source File

SOURCE=.\Shared\hubwhup.bmp
# End Source File
# End Group
# End Target
# End Project
