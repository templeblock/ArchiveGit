# Microsoft Developer Studio Project File - Name="Upgrade" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Upgrade - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Iupg32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Iupg32.mak" CFG="Upgrade - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Upgrade - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Upgrade - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Upgrade - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WINREL"
# PROP Intermediate_Dir ".\WINREL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Ot /Oi /Oy /Ob1 /Gf /Gy /I "..\..\gcpp300\h" /I "..\..\msreg\intl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ox /Oa /Ow
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\..\gcpp300\msvc2x\cp30fw.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCD"

!ELSEIF  "$(CFG)" == "Upgrade - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WINDEBUG"
# PROP Intermediate_Dir ".\WINDEBUG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\gcpp300\h" /I "..\..\msreg\intl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\..\gcpp300\msvc2x\cp30fw.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD"

!ENDIF 

# Begin Target

# Name "Upgrade - Win32 Release"
# Name "Upgrade - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Basedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Bmpctl.cpp
# End Source File
# Begin Source File

SOURCE=.\Callfail.cpp
# End Source File
# Begin Source File

SOURCE=.\Carray.cpp
# End Source File
# Begin Source File

SOURCE=.\Checklb.cpp
# End Source File
# Begin Source File

SOURCE=.\Cinstall.cpp
# End Source File
# Begin Source File

SOURCE=.\Comlink.cpp
# End Source File
# Begin Source File

SOURCE=.\Congrats.cpp
# End Source File
# Begin Source File

SOURCE=.\Cprogbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Credit.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwmf.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwmfctl.cpp
# End Source File
# Begin Source File

SOURCE=.\Ddb.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialopts.cpp
# End Source File
# Begin Source File

SOURCE=.\Expirat.cpp
# End Source File
# Begin Source File

SOURCE=.\Inifile.cpp
# End Source File
# Begin Source File

SOURCE=.\INSTDAT.CPP
# End Source File
# Begin Source File

SOURCE=.\INTLDLL.CPP
# End Source File
# Begin Source File

SOURCE=.\Mailord.cpp
# End Source File
# Begin Source File

SOURCE=.\Mlsprint.cpp
# End Source File
# Begin Source File

SOURCE=.\Msengine.cpp
# End Source File
# Begin Source File

SOURCE=.\Msmodem.cpp
# End Source File
# Begin Source File

SOURCE=.\Mspacket.cpp
# End Source File
# Begin Source File

SOURCE=.\Msserial.cpp
# End Source File
# Begin Source File

SOURCE=.\Mszmodem.cpp
# End Source File
# Begin Source File

SOURCE=.\Nomodem.cpp
# End Source File
# Begin Source File

SOURCE=.\Orderfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Orderlb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ordfail.cpp
# End Source File
# Begin Source File

SOURCE=.\Ownerlb.cpp
# End Source File
# Begin Source File

SOURCE=.\Phonelb.cpp
# End Source File
# Begin Source File

SOURCE=.\Phonenum.cpp
# End Source File
# Begin Source File

SOURCE=.\Phoneord.cpp
# End Source File
# Begin Source File

SOURCE=.\Prod.cpp
# End Source File
# Begin Source File

SOURCE=.\Product.cpp
# End Source File
# Begin Source File

SOURCE=.\Propctl.cpp
# End Source File
# Begin Source File

SOURCE=.\Propstr.cpp
# End Source File
# Begin Source File

SOURCE=.\Reginfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Required.cpp
# End Source File
# Begin Source File

SOURCE=.\salestax.cpp
# End Source File
# Begin Source File

SOURCE=.\Search.cpp
# End Source File
# Begin Source File

SOURCE=.\Send.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\upgdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgrade.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgrade.def
# End Source File
# Begin Source File

SOURCE=.\Upgrade.rc
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BASEDLG.H
# End Source File
# Begin Source File

SOURCE=.\BMPCTL.H
# End Source File
# Begin Source File

SOURCE=.\Callfail.h
# End Source File
# Begin Source File

SOURCE=.\CARRAY.H
# End Source File
# Begin Source File

SOURCE=.\CHECKLB.H
# End Source File
# Begin Source File

SOURCE=.\Cinstall.h
# End Source File
# Begin Source File

SOURCE=.\Comlink.h
# End Source File
# Begin Source File

SOURCE=.\Congrats.h
# End Source File
# Begin Source File

SOURCE=.\CPROGBAR.H
# End Source File
# Begin Source File

SOURCE=.\Credit.h
# End Source File
# Begin Source File

SOURCE=.\CWMF.H
# End Source File
# Begin Source File

SOURCE=.\CWMFCTL.H
# End Source File
# Begin Source File

SOURCE=.\ddb.h
# End Source File
# Begin Source File

SOURCE=.\Dialopts.h
# End Source File
# Begin Source File

SOURCE=.\Expirat.h
# End Source File
# Begin Source File

SOURCE=.\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\INSTDAT.H
# End Source File
# Begin Source File

SOURCE=.\intldll.h
# End Source File
# Begin Source File

SOURCE=.\IUPGRADE.H
# End Source File
# Begin Source File

SOURCE=.\Mailord.h
# End Source File
# Begin Source File

SOURCE=.\Mlsprint.h
# End Source File
# Begin Source File

SOURCE=.\MSENGINE.H
# End Source File
# Begin Source File

SOURCE=.\Msmodem.h
# End Source File
# Begin Source File

SOURCE=.\MSPACKET.H
# End Source File
# Begin Source File

SOURCE=.\MSREGINI.H
# End Source File
# Begin Source File

SOURCE=.\Msserial.h
# End Source File
# Begin Source File

SOURCE=.\MSZMODEM.H
# End Source File
# Begin Source File

SOURCE=.\Nomodem.h
# End Source File
# Begin Source File

SOURCE=.\Orderfrm.h
# End Source File
# Begin Source File

SOURCE=.\ORDERLB.H
# End Source File
# Begin Source File

SOURCE=.\Ordfail.h
# End Source File
# Begin Source File

SOURCE=.\OWNERLB.H
# End Source File
# Begin Source File

SOURCE=.\Phonelb.h
# End Source File
# Begin Source File

SOURCE=.\Phonenum.h
# End Source File
# Begin Source File

SOURCE=.\Phoneord.h
# End Source File
# Begin Source File

SOURCE=.\PROD.H
# End Source File
# Begin Source File

SOURCE=.\PRODUCT.H
# End Source File
# Begin Source File

SOURCE=.\Propctl.h
# End Source File
# Begin Source File

SOURCE=.\Propstr.h
# End Source File
# Begin Source File

SOURCE=.\REGINFO.H
# End Source File
# Begin Source File

SOURCE=.\Required.h
# End Source File
# Begin Source File

SOURCE=.\SALESTAX.H
# End Source File
# Begin Source File

SOURCE=.\Search.h
# End Source File
# Begin Source File

SOURCE=.\Send.h
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\UPGDLG.H
# End Source File
# Begin Source File

SOURCE=.\UPGRADE.H
# End Source File
# Begin Source File

SOURCE=.\UTIL.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\DELIVERY.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\HUH.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\ORDERFRM.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\SHERLOCK.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\THANKS.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Upgrade.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\WOMANCLP.BMP
# End Source File
# End Group
# End Target
# End Project
