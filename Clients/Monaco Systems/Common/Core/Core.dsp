# Microsoft Developer Studio Project File - Name="Core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Core.mak" CFG="Core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "Core - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Core - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Common\Core\Source" /I "..\Common\Pc\Source" /I "..\Common\File&Mem\Source" /I "..\Common\ScanCal\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Core - Win32 Release"
# Name "Core - Win32 Debug"
# Begin Source File

SOURCE=.\Source\add_black.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\add_black.h
# End Source File
# Begin Source File

SOURCE=.\Source\assort.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\assort.h
# End Source File
# Begin Source File

SOURCE=.\Source\buildtableindex.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\buildtableindex.h
# End Source File
# Begin Source File

SOURCE=.\Source\calcurves.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\calcurves.h
# End Source File
# Begin Source File

SOURCE=.\Source\calib_multi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\calib_multi.h
# End Source File
# Begin Source File

SOURCE=.\Source\calibrate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\calibrate.h
# End Source File
# Begin Source File

SOURCE=.\Source\cmysurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\cmysurface.h
# End Source File
# Begin Source File

SOURCE=.\Source\codetable.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\codetable.h
# End Source File
# Begin Source File

SOURCE=.\Source\colortran.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\colortran.h
# End Source File
# Begin Source File

SOURCE=.\Source\condata.h
# End Source File
# Begin Source File

SOURCE=.\Source\document.h
# End Source File
# Begin Source File

SOURCE=.\Source\find_black.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\find_black.h
# End Source File
# Begin Source File

SOURCE=.\Source\find_blacklab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\find_blacklab.h
# End Source File
# Begin Source File

SOURCE=.\Source\gammut_comp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\gammut_comp.h
# End Source File
# Begin Source File

SOURCE=.\Source\icctypes.h
# End Source File
# Begin Source File

SOURCE=.\Source\imagetrans.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\imagetrans.h
# End Source File
# Begin Source File

SOURCE=.\Source\interpolate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\interpolate.h
# End Source File
# Begin Source File

SOURCE=.\Source\james_v.h
# End Source File
# Begin Source File

SOURCE=.\Source\linearize_data.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\linearize_data.h
# End Source File
# Begin Source File

SOURCE=.\Source\mathutilities.h
# End Source File
# Begin Source File

SOURCE=.\Source\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\matrix.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcoicc.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcolyout.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcomem.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcomem.PC.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mcomem.PC.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcostat.h
# End Source File
# Begin Source File

SOURCE=.\Source\mcotypes.h
# End Source File
# Begin Source File

SOURCE=.\Source\monaco.h
# End Source File
# Begin Source File

SOURCE=.\Source\monacoprint.h
# End Source File
# Begin Source File

SOURCE=.\Source\monitor.h
# End Source File
# Begin Source File

SOURCE=.\Source\mpdata.h
# End Source File
# Begin Source File

SOURCE=.\Source\patchformat.h
# End Source File
# Begin Source File

SOURCE=.\Source\picelin.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\picelin.h
# End Source File
# Begin Source File

SOURCE=.\Source\picelin3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\picelin3.h
# End Source File
# Begin Source File

SOURCE=.\Source\profiledoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\profiledoc.h
# End Source File
# Begin Source File

SOURCE=.\Source\rawdata.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\rawdata.h
# End Source File
# Begin Source File

SOURCE=.\Source\recipes.h
# End Source File
# Begin Source File

SOURCE=.\Source\safe_inter.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\safe_inter.h
# End Source File
# Begin Source File

SOURCE=.\Source\scancal.h
# End Source File
# Begin Source File

SOURCE=.\Source\scantarg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\scantarg.h
# End Source File
# Begin Source File

SOURCE=.\Source\scram_array.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\scram_array.h
# End Source File
# Begin Source File

SOURCE=.\Source\setupctype.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\setupctype.h
# End Source File
# Begin Source File

SOURCE=.\Source\SLFunctions.c
# End Source File
# Begin Source File

SOURCE=.\Source\SLFunctions.h
# End Source File
# Begin Source File

SOURCE=.\Source\sort2.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\sort2.h
# End Source File
# Begin Source File

SOURCE=.\Source\spline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\spline.h
# End Source File
# Begin Source File

SOURCE=.\Source\spline2.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\spline2.h
# End Source File
# Begin Source File

SOURCE=.\Source\spline3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\spline3.h
# End Source File
# Begin Source File

SOURCE=.\Source\splinesegment.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\splinesegment.h
# End Source File
# Begin Source File

SOURCE=.\Source\splint.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\splint.h
# End Source File
# Begin Source File

SOURCE=.\Source\Thermobject.h
# End Source File
# Begin Source File

SOURCE=.\Source\thermobject.PC.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\thermobject.PC.h
# End Source File
# Begin Source File

SOURCE=.\Source\tweak_patch.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tweak_patch.h
# End Source File
# Begin Source File

SOURCE=.\Source\xyztorgb.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\xyztorgb.h
# End Source File
# End Target
# End Project
