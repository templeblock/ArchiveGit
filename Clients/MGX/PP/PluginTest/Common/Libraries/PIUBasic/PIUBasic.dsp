# Microsoft Developer Studio Project File - Name="PIUBasic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PIUBasic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PIUBasic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PIUBasic.mak" CFG="PIUBasic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PIUBasic - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PIUBasic - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "PIUBasic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /WX /GR /GX /Ob2 /I "..\..\Headers\Photoshop\PICA" /I "..\..\Headers\Photoshop\PS-Suites" /I "..\..\Headers\Photoshop" /I "..\..\Headers\Photoshop\ADM" /I "..\..\Headers\Photoshop\AS" /I "..\..\Headers\SDK" /I "..\..\Rez-files\Photoshop" /I "..\..\Rez-files\SDK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D MSWindows=1 /YX /FD /c
# SUBTRACT CPP /O<none> /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PIUBasic-release.lib"

!ELSEIF  "$(CFG)" == "PIUBasic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MDd /W3 /WX /GR /GX /Zi /Od /I "..\..\Headers\Photoshop\PICA" /I "..\..\Headers\Photoshop\PS-Suites" /I "..\..\Headers\Photoshop" /I "..\..\Headers\Photoshop\ADM" /I "..\..\Headers\Photoshop\AS" /I "..\..\Headers\SDK" /I "..\..\Rez-files\Photoshop" /I "..\..\Rez-files\SDK" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D MSWindows=1 /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PIUBasic-debug.lib"

!ENDIF 

# Begin Target

# Name "PIUBasic - Win32 Release"
# Name "PIUBasic - Win32 Debug"
# Begin Group "Sources"

# PROP Default_Filter ".cpp, .c"
# Begin Source File

SOURCE=..\..\Sources\PIUActionControlUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUActionParams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUBasic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUCore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUDispatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUNew.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUNotifyUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUReferenceUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUSuites.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUTools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUUIParams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Sources\PIUWindowsEntry.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ",h, .hpp"
# Begin Source File

SOURCE=..\..\Headers\SDK\PICA2PSErrorMap.h

!IF  "$(CFG)" == "PIUBasic - Win32 Release"

!ELSEIF  "$(CFG)" == "PIUBasic - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIDefines.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUActionControlUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUActionParams.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUBasic.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUCore.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUDispatch.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUNew.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUNotifyUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUReferenceUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUSuitePointer.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUSuites.h
# End Source File
# Begin Source File

SOURCE=..\..\Headers\SDK\PIUTools.h
# End Source File
# End Group
# End Target
# End Project
