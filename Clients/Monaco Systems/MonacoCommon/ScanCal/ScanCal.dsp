# Microsoft Developer Studio Project File - Name="ScanCal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ScanCal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ScanCal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScanCal.mak" CFG="ScanCal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScanCal - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ScanCal - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "ScanCal - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\File&Mem\Source" /I "..\Core\Source" /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ScanCal - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\File&Mem\Source" /I "..\Core\Source" /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ScanCal - Win32 Release"
# Name "ScanCal - Win32 Debug"
# Begin Source File

SOURCE=.\Source\calfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\calfunc.h
# End Source File
# Begin Source File

SOURCE=.\Source\linearcal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Linearcal.h
# End Source File
# Begin Source File

SOURCE=.\Source\multilcal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\multilcal.h
# End Source File
# Begin Source File

SOURCE=.\Source\pwlinear.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\pwlinear.h
# End Source File
# Begin Source File

SOURCE=.\Source\Scancal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Scancal.h
# End Source File
# End Target
# End Project
