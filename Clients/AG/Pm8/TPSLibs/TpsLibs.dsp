# Microsoft Developer Studio Project File - Name="TpsLibs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TpsLibs - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TpsLibs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TpsLibs.mak" CFG="TpsLibs - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TpsLibs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TpsLibs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PrintShop/Development", NQZAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TpsLibs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I ".\Include" /I "..\framework\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN" /D "FRAMEWORK" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winRel/Pmw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TpsLibs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I ".\Include" /I "..\framework\include" /D "_DEBUG" /D "TPSDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN" /D "FRAMEWORK" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winDebug/Pmw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bins\TpsLibsDebug.lib"

!ENDIF 

# Begin Target

# Name "TpsLibs - Win32 Release"
# Name "TpsLibs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\dbgglbl.c

!IF  "$(CFG)" == "TpsLibs - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TpsLibs - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\tpsdebug.c

!IF  "$(CFG)" == "TpsLibs - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TpsLibs - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\tpsint.c
# End Source File
# Begin Source File

SOURCE=.\Source\TPSRECT.C
# End Source File
# Begin Source File

SOURCE=.\Source\WINBIT.C
# End Source File
# Begin Source File

SOURCE=.\Source\windev.c

!IF  "$(CFG)" == "TpsLibs - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TpsLibs - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\windlg.c

!IF  "$(CFG)" == "TpsLibs - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TpsLibs - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\XBIT.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\DBDEVICE.H
# End Source File
# Begin Source File

SOURCE=.\Include\DBGDLG.H
# End Source File
# Begin Source File

SOURCE=.\Include\DBGGLBL.H
# End Source File
# Begin Source File

SOURCE=.\Include\STACKCRA.H
# End Source File
# Begin Source File

SOURCE=.\Include\TERRDEFS.H
# End Source File
# Begin Source File

SOURCE=.\Include\tpsbit.h
# End Source File
# Begin Source File

SOURCE=.\Include\TPSDEBUG.H
# End Source File
# Begin Source File

SOURCE=.\Include\tpsdefs.h
# End Source File
# Begin Source File

SOURCE=.\Include\tpsint.h
# End Source File
# Begin Source File

SOURCE=.\Include\tpsmem.h
# End Source File
# Begin Source File

SOURCE=.\Include\tpsrect.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
