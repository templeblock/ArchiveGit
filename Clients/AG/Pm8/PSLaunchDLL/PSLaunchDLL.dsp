# Microsoft Developer Studio Project File - Name="PSLaunchDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PSLaunchDLL - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PSLaunchDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PSLaunchDLL.mak" CFG="PSLaunchDLL - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PSLaunchDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PSLaunchDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PrintShop/Development", WKDEAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PSLaunchDLL - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GR /GX /ZI /I "..\Framework\Include" /I "..\TPSLibs\Include" /I "..\Renaissance\Include" /I "..\Accusoft" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "TPSDEBUG" /D "MEM_DEBUG" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Custom Build
InputPath=.\Debug\PSLaunchDLL.dll
SOURCE="$(InputPath)"

"..\windebug\PSLaunchDLL.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\windebug

# End Custom Build

!ELSEIF  "$(CFG)" == "PSLaunchDLL - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PSLaunchDLL___Win32_Release"
# PROP BASE Intermediate_Dir "PSLaunchDLL___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Framework\Include" /I "..\TPSLibs\Include" /I "..\Renaissance\Include" /I "..\Accusoft" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /machine:I386
# SUBTRACT LINK32 /debug
# Begin Custom Build - Custom Build
InputPath=.\Release\PSLaunchDLL.dll
SOURCE="$(InputPath)"

"..\windebug\PSLaunchDLL.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\windebug

# End Custom Build

!ENDIF 

# Begin Target

# Name "PSLaunchDLL - Win32 Debug"
# Name "PSLaunchDLL - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\calendar.cpp
# End Source File
# Begin Source File

SOURCE=.\DblSpin.cpp
# End Source File
# Begin Source File

SOURCE=.\FileFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectCalendar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectCategoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectQSLDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PsBaseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PSButton.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLaunchCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLaunchData.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDLL.def
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDLL.rc
# End Source File
# Begin Source File

SOURCE=.\QSLItemList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Framework\Bins\Framework.lib

!IF  "$(CFG)" == "PSLaunchDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PSLaunchDLL - Win32 Release"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Framework\Bins\FrameworkDebug.lib

!IF  "$(CFG)" == "PSLaunchDLL - Win32 Debug"

!ELSEIF  "$(CFG)" == "PSLaunchDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\calendar.h
# End Source File
# Begin Source File

SOURCE=.\CommonTypes.h
# End Source File
# Begin Source File

SOURCE=.\Docsetup.h
# End Source File
# Begin Source File

SOURCE=.\FileFinder.h
# End Source File
# Begin Source File

SOURCE=.\ProjectCalendar.h
# End Source File
# Begin Source File

SOURCE=.\ProjectCategoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProjectDialog.h
# End Source File
# Begin Source File

SOURCE=.\ProjectInfo.h
# End Source File
# Begin Source File

SOURCE=.\ProjectQSLDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProjectSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\PsBaseDialog.h
# End Source File
# Begin Source File

SOURCE=.\PSButton.h
# End Source File
# Begin Source File

SOURCE=.\PSLaunchCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PSLaunchData.h
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDlg.h
# End Source File
# Begin Source File

SOURCE=.\PSLaunchDLL.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CalendarDaily.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CalendarMonthly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CalendarWeekly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CalendarYearly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CardShop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CardShopFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryAll.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryAllFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryClassroom.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryClassroomFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryCreative.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryCreativeFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryInternet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryInternetFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryOpenFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryPhoto.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryPhotoFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryWorkplace.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CategoryWorkplaceFocus.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Envelope6x4p75.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Envelope9x6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EnvelopeBusiness.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EnvelopeNoteCard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EnvelopePrintShop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EnvelopePS.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EnvelopeStock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label3261Large.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label3261Small.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5160.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5161.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5162.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5163.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5164.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Label5196.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ListEnvelope.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ListGreetingCard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\NewsLetter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlaceHolderButton.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlaceHolderPaper.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlaceHolderQSL.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PlaceHolderScratch.bmp
# End Source File
# Begin Source File

SOURCE=.\res\preview_.bmp
# End Source File
# Begin Source File

SOURCE=.\res\preview_placeholder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\preview_tall.bmp
# End Source File
# Begin Source File

SOURCE=.\res\preview_wide.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewCalendarDaily.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewCalendarMonthly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewCalendarWeekly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewCalendarYearly.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewLabels.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewNewsletter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PreviewWebsite.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectBlankPages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectMethod.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectPhotoAlbums.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectPhotoCollages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectPhotoPages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectPhotoQuickPrint.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectWebSites.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProjectWorkspace.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PSCardShop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PSGreetingCard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PSGreetingCardMethod.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PSLaunch.ico
# End Source File
# Begin Source File

SOURCE=.\res\PSLaunchDLL.ico
# End Source File
# Begin Source File

SOURCE=.\res\PSLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\QFoldTall.bmp
# End Source File
# Begin Source File

SOURCE=.\res\QFoldWide.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Side Bar.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\TextCalendar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextEnvelope.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextGreetingCard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextLabel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextNewsletter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextPhotoProject.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextSign.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TextWebsite.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Top PS Bar.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\WorkspaceSide.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WorkspaceSplash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WorkspaceTop.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\PSLaunchCtrl.rgs
# End Source File
# Begin Source File

SOURCE=.\res\PSLaunchData.rgs
# End Source File
# End Target
# End Project
# Section PSLaunchDLL : {8E27C92C-1264-101C-8A2F-040224009C02}
# 	2:5:Class:CCalendar
# 	2:10:HeaderFile:calendar.h
# 	2:8:ImplFile:calendar.cpp
# End Section
# Section PSLaunchDLL : {8E27C92B-1264-101C-8A2F-040224009C02}
# 	2:21:DefaultSinkHeaderFile:calendar.h
# 	2:16:DefaultSinkClass:CCalendar
# End Section
