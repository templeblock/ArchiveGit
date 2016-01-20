# Microsoft Developer Studio Project File - Name="Stonehnd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Stonehnd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Stonehnd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Stonehnd.mak" CFG="Stonehnd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Stonehnd - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Stonehnd - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Stonehnd", CGAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Stonehnd - Win32 Release"

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
# ADD CPP /nologo /Zp2 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Z<none> /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Stonehnd - Win32 Debug"

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
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Gf /Gy /Fr /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Stonehnd - Win32 Release"
# Name "Stonehnd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Sc_chmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Sc_spchg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sc_sysco.cpp
# End Source File
# Begin Source File

SOURCE=.\Sc_utlwi.cpp
# End Source File
# Begin Source File

SOURCE=.\Scapi.cpp
# End Source File
# Begin Source File

SOURCE=.\Scapptex.cpp
# End Source File
# Begin Source File

SOURCE=.\Scarray.cpp
# End Source File
# Begin Source File

SOURCE=.\Scbezble.cpp
# End Source File
# Begin Source File

SOURCE=.\Scbezier.cpp
# End Source File
# Begin Source File

SOURCE=.\Scbreak.cpp
# End Source File
# Begin Source File

SOURCE=.\Sccolinf.cpp
# End Source File
# Begin Source File

SOURCE=.\Sccolum2.cpp
# End Source File
# Begin Source File

SOURCE=.\Sccolum3.cpp
# End Source File
# Begin Source File

SOURCE=.\Sccolumn.cpp
# End Source File
# Begin Source File

SOURCE=.\Sccspecl.cpp
# End Source File
# Begin Source File

SOURCE=.\Scctype.cpp
# End Source File
# Begin Source File

SOURCE=.\Scdbcsdt.cpp
# End Source File
# Begin Source File

SOURCE=.\Scdebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Scdeftmp.cpp
# End Source File
# Begin Source File

SOURCE=.\Scexcept.cpp
# End Source File
# Begin Source File

SOURCE=.\Scfileio.cpp
# End Source File
# Begin Source File

SOURCE=.\Scglobda.cpp
# End Source File
# Begin Source File

SOURCE=.\Schrect.cpp
# End Source File
# Begin Source File

SOURCE=.\Scmemarr.cpp
# End Source File
# Begin Source File

SOURCE=.\Scnshmem.cpp
# End Source File
# Begin Source File

SOURCE=.\Scobject.cpp
# End Source File
# Begin Source File

SOURCE=.\Scparag2.cpp
# End Source File
# Begin Source File

SOURCE=.\Scparag3.cpp
# End Source File
# Begin Source File

SOURCE=.\Scparagr.cpp
# End Source File
# Begin Source File

SOURCE=.\Scpolygo.cpp
# End Source File
# Begin Source File

SOURCE=.\Scregion.cpp
# End Source File
# Begin Source File

SOURCE=.\scrubi.cpp
# End Source File
# Begin Source File

SOURCE=.\Scselec2.cpp
# End Source File
# Begin Source File

SOURCE=.\Scselect.cpp
# End Source File
# Begin Source File

SOURCE=.\Scset.cpp
# End Source File
# Begin Source File

SOURCE=.\Scspcrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Scstcach.cpp
# End Source File
# Begin Source File

SOURCE=.\Scstiter.cpp
# End Source File
# Begin Source File

SOURCE=.\Scstream.cpp
# End Source File
# Begin Source File

SOURCE=.\Scstyle.cpp
# End Source File
# Begin Source File

SOURCE=.\Sctbobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Sctextch.cpp
# End Source File
# Begin Source File

SOURCE=.\Sctextli.cpp
# End Source File
# Begin Source File

SOURCE=.\Sctxtlnm.cpp
# End Source File
# Begin Source File

SOURCE=.\Ufont.cpp
# End Source File
# Begin Source File

SOURCE=.\Univstr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Refcnt.h
# End Source File
# Begin Source File

SOURCE=.\Scannota.h
# End Source File
# Begin Source File

SOURCE=.\Scappint.h
# End Source File
# Begin Source File

SOURCE=.\Scapptex.h
# End Source File
# Begin Source File

SOURCE=.\Scapptyp.h
# End Source File
# Begin Source File

SOURCE=.\Scarray.h
# End Source File
# Begin Source File

SOURCE=.\Scbezier.h
# End Source File
# Begin Source File

SOURCE=.\Scbreak.h
# End Source File
# Begin Source File

SOURCE=.\Sccallbk.h
# End Source File
# Begin Source File

SOURCE=.\Sccharex.h
# End Source File
# Begin Source File

SOURCE=.\Scchflag.h
# End Source File
# Begin Source File

SOURCE=.\Sccolumn.h
# End Source File
# Begin Source File

SOURCE=.\Scctype.h
# End Source File
# Begin Source File

SOURCE=.\Scdbcsdt.h
# End Source File
# Begin Source File

SOURCE=.\Scexcept.h
# End Source File
# Begin Source File

SOURCE=.\Scfileio.h
# End Source File
# Begin Source File

SOURCE=.\Scglobda.h
# End Source File
# Begin Source File

SOURCE=.\Scmacint.h
# End Source File
# Begin Source File

SOURCE=.\Scmem.h
# End Source File
# Begin Source File

SOURCE=.\Scmemarr.h
# End Source File
# Begin Source File

SOURCE=.\Scobject.h
# End Source File
# Begin Source File

SOURCE=.\Scparagr.h
# End Source File
# Begin Source File

SOURCE=.\Scpolygo.h
# End Source File
# Begin Source File

SOURCE=.\Scpubobj.h
# End Source File
# Begin Source File

SOURCE=.\Scrange.h
# End Source File
# Begin Source File

SOURCE=.\Screfdat.h
# End Source File
# Begin Source File

SOURCE=.\Scregion.h
# End Source File
# Begin Source File

SOURCE=.\scrubi.h
# End Source File
# Begin Source File

SOURCE=.\Scselect.h
# End Source File
# Begin Source File

SOURCE=.\Scset.h
# End Source File
# Begin Source File

SOURCE=.\Scsetjmp.h
# End Source File
# Begin Source File

SOURCE=.\Scspcrec.h
# End Source File
# Begin Source File

SOURCE=.\Scstcach.h
# End Source File
# Begin Source File

SOURCE=.\Scstream.h
# End Source File
# Begin Source File

SOURCE=.\Scstyle.h
# End Source File
# Begin Source File

SOURCE=.\Sctbobj.h
# End Source File
# Begin Source File

SOURCE=.\Sctextli.h
# End Source File
# Begin Source File

SOURCE=.\Sctypes.h
# End Source File
# Begin Source File

SOURCE=.\Ufont.h
# End Source File
# Begin Source File

SOURCE=.\Univstr.h
# End Source File
# End Group
# End Target
# End Project
