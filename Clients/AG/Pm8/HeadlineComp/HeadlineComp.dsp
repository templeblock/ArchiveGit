# Microsoft Developer Studio Project File - Name="HeadlineComp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HeadlineComp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HeadlineComp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HeadlineComp.mak" CFG="HeadlineComp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HeadlineComp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HeadlineComp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PrintShop/Development", NQZAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HeadlineComp - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\winRel\Components"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O1 /I ".\Resource" /I "..\Accusoft" /I ".\Include" /I "..\TpsLibs\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Framework\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "WIN" /D "COMPONENT" /Yu"HeadlineIncludes.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winRel/Pmw.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "HeadlineComp - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\winDebug\Components"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I ".\Include" /I "..\Accusoft" /I "..\TpsLibs\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Framework\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "WIN" /D "COMPONENT" /D "TPSDEBUG" /D "MEM_DEBUG" /Fr /Yu"HeadlineIncludes.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winDebug/Pmw.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcd" /out:"..\winDebug\Components\HeadlineCompDebug.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "HeadlineComp - Win32 Release"
# Name "HeadlineComp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\AttribTab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\BehindEffectsTab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CompDegree.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditHeadlineAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditHeadlineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EffectsTab.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\Headline.rc
# ADD BASE RSC /l 0x409 /i "Resource"
# ADD RSC /l 0x409 /i "Resource" /i "..\Resource"
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineAlgorithms.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineColor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineIncludes.cpp

!IF  "$(CFG)" == "HeadlineComp - Win32 Release"

# ADD CPP /W3 /Yc"HeadlineIncludes.h"

!ELSEIF  "$(CFG)" == "HeadlineComp - Win32 Debug"

# ADD CPP /W3 /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineInterfaceImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineInterfaces.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlinePersistantObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlinesCanned.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HeadlineView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HLFindReplaceInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HLSpellCheckInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\OutlineTab.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PushlikeCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PushlikeRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SetHeadlineTextAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ShapeTab.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\AttribTab.h
# End Source File
# Begin Source File

SOURCE=.\Include\BehindEffectsTab.h
# End Source File
# Begin Source File

SOURCE=.\Include\CompDegree.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditHeadlineAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditHeadlineDlg.h
# End Source File
# Begin Source File

SOURCE=.\Include\EffectsTab.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineAlgorithms.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineApplication.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineColor.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineDocument.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineIncludes.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineInterfaceImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlinePersistantObject.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlinesCanned.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineSheet.h
# End Source File
# Begin Source File

SOURCE=.\Include\HeadlineView.h
# End Source File
# Begin Source File

SOURCE=.\Include\HLFindReplaceInterface.h
# End Source File
# Begin Source File

SOURCE=.\Include\HLSpellCheckInterface.h
# End Source File
# Begin Source File

SOURCE=.\Include\OutlineTab.h
# End Source File
# Begin Source File

SOURCE=.\Include\PushlikeCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\Include\PushlikeRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\Include\SetHeadlineTextAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ShapeTab.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
