# Microsoft Developer Studio Project File - Name="FileMem4ByteMThread" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FileMem4ByteMThread - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FileMem4ByteMThread.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FileMem4ByteMThread.mak"\
 CFG="FileMem4ByteMThread - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FileMem4ByteMThread - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "FileMem4ByteMThread - Win32 Debug" (based on\
 "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "FileMem4ByteMThread - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Zp4 /MT /W3 /GX /O2 /I "..\Core\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FileMem4ByteMThread - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /Zp4 /MTd /W3 /GX /Z7 /Od /I "..\Core\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "FileMem4ByteMThread - Win32 Release"
# Name "FileMem4ByteMThread - Win32 Debug"
# Begin Source File

SOURCE=.\Source\cmiccprofile.h
# End Source File
# Begin Source File

SOURCE=.\Source\confileformat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\confileformat.h
# End Source File
# Begin Source File

SOURCE=.\Source\csprofile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\csprofile.h
# End Source File
# Begin Source File

SOURCE=.\Source\fileformat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\fileformat.h
# End Source File
# Begin Source File

SOURCE=.\Source\mlfileio.h
# End Source File
# Begin Source File

SOURCE=.\Source\mpfileio.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mpfileio.h
# End Source File
# Begin Source File

SOURCE=.\Source\printertype.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\printertype.h
# End Source File
# Begin Source File

SOURCE=.\Source\profileDocfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\profiledocfiles.h
# End Source File
# Begin Source File

SOURCE=.\Source\swab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\swab.h
# End Source File
# End Target
# End Project