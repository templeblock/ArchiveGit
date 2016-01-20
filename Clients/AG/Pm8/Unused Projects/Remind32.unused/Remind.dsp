# Microsoft Developer Studio Project File - Name="Remind" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Remind - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Remind.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Remind.mak" CFG="Remind - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Remind - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Remind - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Remind - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"STDAFX.H" /FD /c
# ADD CPP /nologo /G3 /Zp2 /MT /W3 /GX /O1 /D "REMIND" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"STDAFX.H" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /machine:IX86
# ADD LINK32 oldnames.lib mpr.lib /nologo /stack:0x2800 /subsystem:windows /machine:IX86 /out:"..\winRel/Pmremind.exe"

!ELSEIF  "$(CFG)" == "Remind - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\winDebug/Pmremind.exe"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"STDAFX.H" /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /D "REMIND" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"STDAFX.H" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /debug /machine:IX86 /pdbtype:sept
# ADD LINK32 oldnames.lib mpr.lib /nologo /stack:0x2800 /subsystem:windows /debug /machine:IX86 /out:"..\winDebug/Pmremind.exe" /pdbtype:sept
# SUBTRACT LINK32 /force

!ENDIF 

# Begin Target

# Name "Remind - Win32 Release"
# Name "Remind - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat"
# Begin Source File

SOURCE=.\ADDRBOOK.CPP
# End Source File
# Begin Source File

SOURCE=.\ARRAY.CPP
# End Source File
# Begin Source File

SOURCE=.\ARRAYREC.CPP
# End Source File
# Begin Source File

SOURCE=.\CCTL3D.CPP
# End Source File
# Begin Source File

SOURCE=.\CHECKLB.CPP
# End Source File
# Begin Source File

SOURCE=.\DATABASE.CPP
# End Source File
# Begin Source File

SOURCE=.\DAY.CPP
# End Source File
# Begin Source File

SOURCE=.\ERROR.CPP
# End Source File
# Begin Source File

SOURCE=.\EVENT.CPP
# End Source File
# Begin Source File

SOURCE=.\EVENTREM.CPP
# End Source File
# Begin Source File

SOURCE=.\EVENTWIZ.CPP
# End Source File
# Begin Source File

SOURCE=.\FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\FLATDB.CPP
# End Source File
# Begin Source File

SOURCE=.\HEAP.CPP
# End Source File
# Begin Source File

SOURCE=.\INIFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\Instdat.cpp
# End Source File
# Begin Source File

SOURCE=.\LIST.CPP
# End Source File
# Begin Source File

SOURCE=.\MEMDEV.CPP
# End Source File
# Begin Source File

SOURCE=.\NETID.CPP
# End Source File
# Begin Source File

SOURCE=.\NETWORK.CPP
# End Source File
# Begin Source File

SOURCE=.\OD.CPP
# End Source File
# Begin Source File

SOURCE=.\OWNERLB.CPP
# End Source File
# Begin Source File

SOURCE=.\PALETTE.CPP
# End Source File
# Begin Source File

SOURCE=.\Pmwcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwwiz.cpp
# End Source File
# Begin Source File

SOURCE=.\RECORD.CPP
# End Source File
# Begin Source File

SOURCE=.\REMIND.CPP
# End Source File
# Begin Source File

SOURCE=.\REMIND.RC
# End Source File
# Begin Source File

SOURCE=.\RWSEM.CPP
# End Source File
# Begin Source File

SOURCE=.\SORTARRY.CPP
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"
# End Source File
# Begin Source File

SOURCE=.\STOREMAN.CPP
# End Source File
# Begin Source File

SOURCE=.\Strclass.cpp
# End Source File
# Begin Source File

SOURCE=.\TABLEMAN.CPP
# End Source File
# Begin Source File

SOURCE=.\timeutil.cpp
# End Source File
# Begin Source File

SOURCE=.\UPCOMING.CPP
# End Source File
# Begin Source File

SOURCE=.\UTIL.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ADDRBOOK.H
# End Source File
# Begin Source File

SOURCE=.\ARRAY.H
# End Source File
# Begin Source File

SOURCE=.\ARRAYREC.H
# End Source File
# Begin Source File

SOURCE=.\CCTL3D.H
# End Source File
# Begin Source File

SOURCE=.\CHECKLB.H
# End Source File
# Begin Source File

SOURCE=.\CTL3D.H
# End Source File
# Begin Source File

SOURCE=.\DATABASE.H
# End Source File
# Begin Source File

SOURCE=.\DAY.H
# End Source File
# Begin Source File

SOURCE=.\DBSTORE.H
# End Source File
# Begin Source File

SOURCE=.\ERROR.H
# End Source File
# Begin Source File

SOURCE=.\EVENT.H
# End Source File
# Begin Source File

SOURCE=.\EVENTREM.H
# End Source File
# Begin Source File

SOURCE=.\EVENTWIZ.H
# End Source File
# Begin Source File

SOURCE=.\FILE.H
# End Source File
# Begin Source File

SOURCE=.\FLATDB.H
# End Source File
# Begin Source File

SOURCE=.\HEAP.H
# End Source File
# Begin Source File

SOURCE=.\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\Instdat.h
# End Source File
# Begin Source File

SOURCE=.\LIST.H
# End Source File
# Begin Source File

SOURCE=.\MEMDEV.H
# End Source File
# Begin Source File

SOURCE=.\MISC.H
# End Source File
# Begin Source File

SOURCE=.\NETCOUNT.H
# End Source File
# Begin Source File

SOURCE=.\NETID.H
# End Source File
# Begin Source File

SOURCE=.\OWNERLB.H
# End Source File
# Begin Source File

SOURCE=.\PALETTE.H
# End Source File
# Begin Source File

SOURCE=.\Pmwcfg.h
# End Source File
# Begin Source File

SOURCE=.\PMWWIZ.H
# End Source File
# Begin Source File

SOURCE=.\RECORD.H
# End Source File
# Begin Source File

SOURCE=.\REMIND.H
# End Source File
# Begin Source File

SOURCE=.\RWSEM.H
# End Source File
# Begin Source File

SOURCE=.\SORTARRY.H
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\STOREMAN.H
# End Source File
# Begin Source File

SOURCE=.\Strclass.h
# End Source File
# Begin Source File

SOURCE=.\SYSTEM.H
# End Source File
# Begin Source File

SOURCE=.\TABLEMAN.H
# End Source File
# Begin Source File

SOURCE=.\timeutil.h
# End Source File
# Begin Source File

SOURCE=.\UPCOMING.H
# End Source File
# Begin Source File

SOURCE=.\UTIL.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\EVENTWIZ.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\REMIND.ICO
# End Source File
# Begin Source File

SOURCE=.\res\remind.rc2
# End Source File
# End Group
# End Target
# End Project
