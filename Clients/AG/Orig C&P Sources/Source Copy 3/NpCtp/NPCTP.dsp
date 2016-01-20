# Microsoft Developer Studio Project File - Name="NpCtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NpCtp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NPCTP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NPCTP.mak" CFG="NpCtp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NpCtp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NpCtp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/NpCtp", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NpCtp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /O2 /I "..\ZLib" /I "..\Stonehnd" /I "..\AgDoc" /I "..\CommonUI" /I ".\IncludeNpSdk" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NETSCAPE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Z<none> /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\CommonUI" /i "..\CommonUI\res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\Stonehnd\Release\Stonehnd.lib ..\ZLib\Release\ZLib.lib comdlg32.lib comctl32.lib winspool.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386
# SUBTRACT LINK32 /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "NpCtp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /Zi /Od /I "..\ZLib" /I "..\Stonehnd" /I "..\AgDoc" /I "..\CommonUI" /I ".\IncludeNpSdk" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NETSCAPE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Gf /Gy /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\CommonUI" /i "..\CommonUI\res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Stonehnd\Debug\Stonehnd.lib ..\ZLib\Debug\ZLib.lib comdlg32.lib comctl32.lib winspool.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NpCtp - Win32 Release"
# Name "NpCtp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;rc;def"
# Begin Source File

SOURCE=.\NpCtp.rc
# End Source File
# Begin Source File

SOURCE=.\NpShell.cpp
# End Source File
# Begin Source File

SOURCE=.\NpWin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CommonUI\Res\1up.bmp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\1up2down.bmp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\2down.bmp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\2up.bmp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\3up.bmp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\AGLogo.agi
# End Source File
# Begin Source File

SOURCE="..\CommonUI\Res\C&PLogo.agi"
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Res\cacfc___.ttz
# End Source File
# Begin Source File

SOURCE=.\Version.rc2
# End Source File
# End Group
# Begin Group "AgDoc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AgDoc\AGDC.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGDC.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGDib.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGDoc.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGLayer.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGLayer.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGMatrix.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGPage.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGPage.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGSym.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGSym.h
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGText.cpp
# End Source File
# Begin Source File

SOURCE=..\AgDoc\AGText.h
# End Source File
# End Group
# Begin Group "CommonUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CommonUI\CtlPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\CtlPanel.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Ctp.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Ctp.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\DblSide.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\DblSide.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Font.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\PropSht.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\resource.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\Version.h
# End Source File
# Begin Source File

SOURCE=..\CommonUI\WaitDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUI\WaitDlg.h
# End Source File
# End Group
# End Target
# End Project
