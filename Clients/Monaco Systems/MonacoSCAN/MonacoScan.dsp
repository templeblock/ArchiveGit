# Microsoft Developer Studio Project File - Name="MonacoScan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MonacoScan - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MonacoScan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MonacoScan.mak" CFG="MonacoScan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MonacoScan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MonacoScan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MonacoScan - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /I "..\Common\Sentinel\Source" /I "..\Common\ScanCore\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WIN32" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /o NUL /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "MonacoScan - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /I "..\Common\Sentinel\Source" /I "..\Common\ScanCore\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WIN32" /FR /YX /FD /c
# SUBTRACT CPP /Gy
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /o NUL /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "MonacoScan - Win32 Release"
# Name "MonacoScan - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\Crop.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Dibapi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\imagn.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\label.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoScan.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoScan.rc
# End Source File
# Begin Source File

SOURCE=.\Source\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Target.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\Crop.h
# End Source File
# Begin Source File

SOURCE=.\Source\Dibapi.h
# End Source File
# Begin Source File

SOURCE=.\Source\imagen.h
# End Source File
# Begin Source File

SOURCE=.\Source\label.h
# End Source File
# Begin Source File

SOURCE=.\Source\MainDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\MonacoScan.h
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

SOURCE=.\Source\Target.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\res\1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon_ll.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon_lr.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon_mou.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon_ul.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon_ur.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\LOGO.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\MonacoScan.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\MonacoScan.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash24.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash4.bmp
# End Source File
# End Group
# End Target
# End Project
# Section MonacoScan : {5DA9D7E1-5A57-11CF-9E36-00C0930198C0}
# 	2:5:Class:CImageN
# 	2:10:HeaderFile:imagen.h
# 	2:8:ImplFile:imagn.cpp
# End Section
# Section MonacoScan : {04598FC1-866C-11CF-AB7C-00AA00C08FCF}
# 	2:5:Class:CLabel
# 	2:10:HeaderFile:label.h
# 	2:8:ImplFile:label.cpp
# End Section
# Section MonacoScan : {5DA9D7E0-5A57-11CF-9E36-00C0930198C0}
# 	2:21:DefaultSinkHeaderFile:imagen.h
# 	2:16:DefaultSinkClass:CImageN
# End Section
# Section MonacoScan : {978C9E23-D4B0-11CE-BF2D-00AA003F40D0}
# 	2:21:DefaultSinkHeaderFile:label.h
# 	2:16:DefaultSinkClass:CLabel
# End Section
