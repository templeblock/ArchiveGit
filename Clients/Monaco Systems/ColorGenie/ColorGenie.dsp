# Microsoft Developer Studio Project File - Name="ColorGenie" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ColorGenie - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ColorGenie.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ColorGenie.mak" CFG="ColorGenie - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ColorGenie - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ColorGenie - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ColorGenie - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\Common\Core\Source" /I "..\Common\File&Mem\Source" /I "..\Common\Pc\Source" /I "..\Common\Cmm\Source" /I "..\Common\Sentinel\Source" /I "..\Common\Polaroid\Source" /I "..\Qd3dSdk\Interfaces" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 3dViewer.lib Qd3d.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /out:"Release/Pressto! Proof.exe" /libpath:"..\Qd3dSdk\Lib"

!ELSEIF  "$(CFG)" == "ColorGenie - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /O2 /I "..\Common\Sentinel\Source" /I "..\Common\Core\Source" /I "..\Common\File&Mem\Source" /I "..\Common\Pc\Source" /I "..\Common\Cmm\Source" /I "..\Common\Polaroid\Source" /I "..\Qd3dSdk\Interfaces" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 3dViewer.lib Qd3d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Pressto! Proof.exe" /pdbtype:sept /libpath:"..\Qd3dSdk\Lib"

!ENDIF 

# Begin Target

# Name "ColorGenie - Win32 Release"
# Name "ColorGenie - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\BuildProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorGenie.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorGenie.rc

!IF  "$(CFG)" == "ColorGenie - Win32 Release"

!ELSEIF  "$(CFG)" == "ColorGenie - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\CommDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Curve.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CurveWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceSetupPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ErrorMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FirstDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GamutData.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GretagScan.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GretagScanDensity.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MeasureDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ModifyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MyMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PatchFormatPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PositionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ProfilerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ProfileSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ProfileView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PropertyPages.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dExtras.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutTweak.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Spline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\stringfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tablepc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TechkonTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ViewDataDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ViewGamut.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ViewGamutFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Winmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Xrite408.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\BuildProgress.h
# End Source File
# Begin Source File

SOURCE=.\Source\ChildFrame.h
# End Source File
# Begin Source File

SOURCE=.\Source\ColorGenie.h
# End Source File
# Begin Source File

SOURCE=.\Source\CommDevice.h
# End Source File
# Begin Source File

SOURCE=.\Source\Curve.h
# End Source File
# Begin Source File

SOURCE=.\Source\CurveWindow.h
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceControl.h
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceSetupPage.h
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceSheet.h
# End Source File
# Begin Source File

SOURCE=.\Source\ErrorMessage.h
# End Source File
# Begin Source File

SOURCE=.\Source\FirstDialog.h
# End Source File
# Begin Source File

SOURCE=.\Source\GamutData.h
# End Source File
# Begin Source File

SOURCE=.\Source\GretagScan.h
# End Source File
# Begin Source File

SOURCE=.\Source\GretagScanDensity.h
# End Source File
# Begin Source File

SOURCE=.\Source\grid.h
# End Source File
# Begin Source File

SOURCE=.\Source\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Source\MeasureDialog.h
# End Source File
# Begin Source File

SOURCE=.\Source\ModifyDialog.h
# End Source File
# Begin Source File

SOURCE=.\Source\MyMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\Source\PatchFormatPage.h
# End Source File
# Begin Source File

SOURCE=.\Source\PositionDialog.h
# End Source File
# Begin Source File

SOURCE=.\Source\ProfilerDoc.h
# End Source File
# Begin Source File

SOURCE=.\Source\ProfileSheet.h
# End Source File
# Begin Source File

SOURCE=.\Source\ProfileView.h
# End Source File
# Begin Source File

SOURCE=.\Source\PropertyPages.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dAll.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dControl.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dExtras.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutBase.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutTweak.h
# End Source File
# Begin Source File

SOURCE=.\Source\Qd3dGamutView.h
# End Source File
# Begin Source File

SOURCE=.\Source\resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Source\Spline.h
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Source\stringfunc.h
# End Source File
# Begin Source File

SOURCE=.\Source\tablepc.h
# End Source File
# Begin Source File

SOURCE=.\Source\TechkonTable.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tweak.h
# End Source File
# Begin Source File

SOURCE=.\Source\ViewDataDialog.h
# End Source File
# Begin Source File

SOURCE=.\Source\ViewGamut.h
# End Source File
# Begin Source File

SOURCE=.\Source\ViewGamutFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\Winmisc.h
# End Source File
# Begin Source File

SOURCE=.\Source\Xrite408.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\bm_tweak.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\calibration.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Camera.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\check2.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\ColorGenie.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\ColorGenie.rc2
# End Source File
# Begin Source File

SOURCE=.\Source\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Source\res\data.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\dryjet.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Home.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\lab1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\MoveDepth.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\MovePlane.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\nexclama.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\Point1.BMP
# End Source File
# Begin Source File

SOURCE=.\Source\res\Point2.BMP
# End Source File
# Begin Source File

SOURCE=.\Source\res\Point3.BMP
# End Source File
# Begin Source File

SOURCE=.\Source\res\ProfileDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Source\res\Rotate.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\setting.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\source.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash24.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\Splash4.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tab1.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tab2.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tab3.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tab4.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tab5.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tone.bmp
# End Source File
# Begin Source File

SOURCE=.\Source\res\tuning.bmp
# End Source File
# End Group
# End Target
# End Project
# Section ColorGenie : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:CGridFont
# 	2:10:HeaderFile:gridfont.h
# 	2:8:ImplFile:gridfont.cpp
# End Section
# Section ColorGenie : {6262D3A0-531B-11CF-91F6-C2863C385E30}
# 	2:21:DefaultSinkHeaderFile:grid.h
# 	2:16:DefaultSinkClass:CGrid
# End Section
# Section ColorGenie : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CGridPicture
# 	2:10:HeaderFile:gridpicture.h
# 	2:8:ImplFile:gridpicture.cpp
# End Section
# Section ColorGenie : {D4A97620-8E8F-11CF-93CD-00AA00C08FDF}
# 	2:21:DefaultSinkHeaderFile:image.h
# 	2:16:DefaultSinkClass:CImage
# End Section
# Section ColorGenie : {007ED5FC-D5E4-007E-60D6-7E0008D67E00}
# 	1:10:IDB_SPLASH:110
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section ColorGenie : {00000000-0000-0000-0000-000000000000}
# 	2:10:CMainFrame:Palette support added
# End Section
# Section ColorGenie : {9F6AA700-D188-11CD-AD48-00AA003C9CB6}
# 	2:5:Class:CGridCursor
# 	2:10:HeaderFile:gridcursor.h
# 	2:8:ImplFile:gridcursor.cpp
# End Section
# Section ColorGenie : {50C1A530-964A-11CF-93DD-00AA00C08FDF}
# 	2:5:Class:CImage
# 	2:10:HeaderFile:image.h
# 	2:8:ImplFile:image.cpp
# End Section
# Section ColorGenie : {5F4DF280-531B-11CF-91F6-C2863C385E30}
# 	2:5:Class:CGrid
# 	2:10:HeaderFile:grid.h
# 	2:8:ImplFile:grid.cpp
# End Section
