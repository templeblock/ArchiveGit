# Microsoft Developer Studio Project File - Name="Core4ByteMThread" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Core4ByteMThread - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Core4ByteMThread.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Core4ByteMThread.mak" CFG="Core4ByteMThread - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Core4ByteMThread - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "Core4ByteMThread - Win32 Debug" (based on\
 "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "Core4ByteMThread - Win32 Release"

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
# ADD CPP /nologo /G6 /Zp4 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Core4ByteMThread - Win32 Debug"

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
# ADD CPP /nologo /G6 /Zp4 /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Core4ByteMThread - Win32 Release"
# Name "Core4ByteMThread - Win32 Debug"
# Begin Source File

SOURCE=..\Core\Source\add_black.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\add_black.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\assort.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\assort.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\buildtableindex.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\buildtableindex.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calcurves.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calcurves.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calib_multi.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calib_multi.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calibrate.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\calibrate.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\cmysurface.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\cmysurface.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\codetable.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\codetable.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\colortran.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\colortran.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\condata.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\document.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\find_black.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\find_black.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\find_blacklab.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\find_blacklab.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\gammut_comp.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\gammut_comp.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\icctypes.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\imagetrans.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\imagetrans.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\interpolate.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\interpolate.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\james_v.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\linearize_data.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\linearize_data.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mathutilities.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\matrix.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcoicc.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcolyout.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcomem.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcomem.PC.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcomem.PC.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcostat.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mcotypes.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\monaco.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\monacoprint.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\monitor.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\mpdata.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\patchformat.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\picelin.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\picelin.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\picelin3.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\picelin3.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\profileDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\profiledoc.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\rawdata.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\rawdata.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\recipes.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\safe_inter.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\safe_inter.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\scancal.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\scantarg.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\scantarg.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\scram_array.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\scram_array.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\SLFunctions.c
# End Source File
# Begin Source File

SOURCE=..\Core\Source\SLFunctions.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\sort2.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\sort2.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline2.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline2.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline3.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\spline3.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\splinesegment.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\splinesegment.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\splint.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\splint.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\Thermobject.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\thermobject.PC.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\thermobject.PC.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\tweak_patch.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\tweak_patch.h
# End Source File
# Begin Source File

SOURCE=..\Core\Source\xyztorgb.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\Source\xyztorgb.h
# End Source File
# End Target
# End Project
