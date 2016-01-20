# Microsoft Developer Studio Project File - Name="MonacoView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MonacoView - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MonacoView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MonacoView.mak" CFG="MonacoView - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MonacoView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MonacoView - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MonacoView - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\Sentinel\Source" /I "..\Common\SipCal\Source" /I "..\Common\ViewCore\Source" /I "..\Common\File&Mem\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\Common\SipCal\Source\SipCal32d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "MonacoView - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\Sentinel\Source" /I "..\Common\SipCal\Source" /I "..\Common\ViewCore\Source" /I "..\Common\File&Mem\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /U "File&Mem" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Common\SipCal\Source\SipCal32d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MonacoView - Win32 Release"
# Name "MonacoView - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CalibrateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorGamut.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MeasureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoView.rc

!IF  "$(CFG)" == "MonacoView - Win32 Release"

!ELSEIF  "$(CFG)" == "MonacoView - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\MonacoViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PatchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PreferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Winmisc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\CalibrateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\ColorGamut.h
# End Source File
# Begin Source File

SOURCE=.\Source\MeasureDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoView.h
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoViewDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\PatchDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\PreferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Source\Winmisc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\res\ColorMap.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\DarkPatches.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\LitePatches.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Logo1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\MonacoSensor.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\MonacoView.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\MonacoView.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Warning.bmp
# End Source File
# End Group
# End Target
# End Project
