# Microsoft Developer Studio Project File - Name="GraphicComp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GraphicComp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GraphicComp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GraphicComp.mak" CFG="GraphicComp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GraphicComp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GraphicComp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PrintShop/Development", NQZAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GraphicComp - Win32 Release"

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
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O1 /I ".\Include" /I "..\TpsLibs\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Framework\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "WIN" /D "COMPONENT" /Yu"GraphicIncludes.h" /FD /c
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
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"..\CalendarComp\Bins\CalendarComp.pdb" /debug /machine:I386 /nodefaultlib:"libc" /implib:"..\CalendarComp\Bins\CalendarComp.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GraphicComp - Win32 Debug"

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
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I ".\Include" /I "..\TpsLibs\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Framework\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "WIN" /D "COMPONENT" /D "TPSDEBUG" /D "MEM_DEBUG" /Fr /Yu"GraphicIncludes.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winDebug/Pmw.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcd" /out:"..\winDebug\Components\GraphicCompDebug.dll"

!ENDIF 

# Begin Target

# Name "GraphicComp - Win32 Release"
# Name "GraphicComp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\BorderInterfaceImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\BorderView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CgmSingleGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChangeColorAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditGraphicAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\Graphic.rc
# ADD BASE RSC /l 0x409 /i "Resource"
# ADD RSC /l 0x409 /i "Resource" /i "..\Resource"
# End Source File
# Begin Source File

SOURCE=.\Source\GraphicApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GraphicDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GraphicIncludes.cpp

!IF  "$(CFG)" == "GraphicComp - Win32 Release"

# ADD CPP /W3 /Yc"GraphicIncludes.h"

!ELSEIF  "$(CFG)" == "GraphicComp - Win32 Debug"

# ADD CPP /W3 /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\GraphicInterfaceImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GraphicView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\LineView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3BannerGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3BorderGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3Graphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3GraphicInterfaceImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3GraphicLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3RuledLineGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3SingleGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Psd3TimepieceGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PsdClipboardViewerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SelectGraphicDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TimePlaceInterfaceImp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\BorderInterfaceImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\BorderView.h
# End Source File
# Begin Source File

SOURCE=.\Include\CgmSingleGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChangeColorAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditGraphicAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\GraphicApplication.h
# End Source File
# Begin Source File

SOURCE=.\Include\GraphicDocument.h
# End Source File
# Begin Source File

SOURCE=.\Include\GraphicIncludes.h
# End Source File
# Begin Source File

SOURCE=.\Include\GraphicInterfaceImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\GraphicView.h
# End Source File
# Begin Source File

SOURCE=.\Include\LineView.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3BackdropGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3BannerGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3BorderGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3Graphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3GraphicFileFormat.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3GraphicInterfaceImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3GraphicLibrary.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3RuledLineGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3SingleGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Psd3TimepieceGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\PsdClipboardViewerDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\RailGraphicUtility.h
# End Source File
# Begin Source File

SOURCE=.\Include\SelectGraphicDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\TimepiecePlacementInterfaceImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\TimePlaceInterfaceImp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
