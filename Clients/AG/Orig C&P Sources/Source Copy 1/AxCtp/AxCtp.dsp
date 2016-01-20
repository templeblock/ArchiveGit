# Microsoft Developer Studio Project File - Name="AxCtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AxCtp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AxCtp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AxCtp.mak" CFG="AxCtp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AxCtp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AxCtp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AxCtp", NDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AxCtp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /I "..\ZLib" /I "..\Stonehnd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ..\Stonehnd\Debug\Stonehnd.lib ..\ZLib\Debug\ZLib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Debug
TargetPath=.\Debug\AxCtp.dll
InputPath=.\Debug\AxCtp.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AxCtp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AxCtp___"
# PROP BASE Intermediate_Dir "AxCtp___"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /O1 /I "..\ZLib" /I "..\Stonehnd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ..\Stonehnd\Release\Stonehnd.lib ..\ZLib\Release\ZLib.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Registering ActiveX Control...
OutDir=.\Release
TargetPath=.\Release\AxCtp.dll
InputPath=.\Release\AxCtp.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AxCtp - Win32 Debug"
# Name "AxCtp - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AGDC.cpp
# End Source File
# Begin Source File

SOURCE=.\AGDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AGLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\AGMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\AGPage.cpp
# End Source File
# Begin Source File

SOURCE=.\AGSym.cpp
# End Source File
# Begin Source File

SOURCE=.\AGText.cpp
# End Source File
# Begin Source File

SOURCE=.\AxCtp.cpp
# End Source File
# Begin Source File

SOURCE=.\AxCtp.def
# End Source File
# Begin Source File

SOURCE=.\AxCtp.idl

!IF  "$(CFG)" == "AxCtp - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing MIDL step
InputPath=.\AxCtp.idl

BuildCmds= \
	midl /Oicf /h "AxCtp.h" /iid "AxCtp_i.c" "AxCtp.idl"

".\AxCtp.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\AxCtp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\AxCtp_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "AxCtp - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing MIDL step
InputPath=.\AxCtp.idl

BuildCmds= \
	midl /Oicf /h "AxCtp.h" /iid "AxCtp_i.c" "AxCtp.idl"

".\AxCtp.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\AxCtp.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\AxCtp_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AxCtp.rc
# End Source File
# Begin Source File

SOURCE=.\CtlPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctp.cpp
# End Source File
# Begin Source File

SOURCE=.\dblside.cpp
# End Source File
# Begin Source File

SOURCE=.\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WaitDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AGDC.h
# End Source File
# Begin Source File

SOURCE=.\AGDib.h
# End Source File
# Begin Source File

SOURCE=.\AGDoc.h
# End Source File
# Begin Source File

SOURCE=.\AGLayer.h
# End Source File
# Begin Source File

SOURCE=.\AGMatrix.h
# End Source File
# Begin Source File

SOURCE=.\AGPage.h
# End Source File
# Begin Source File

SOURCE=.\AGSym.h
# End Source File
# Begin Source File

SOURCE=.\AGText.h
# End Source File
# Begin Source File

SOURCE=.\Bsc2.h
# End Source File
# Begin Source File

SOURCE=.\CtlPanel.h
# End Source File
# Begin Source File

SOURCE=.\Ctp.h
# End Source File
# Begin Source File

SOURCE=.\dblside.h
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\propsht.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\WaitDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\1up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\1up2down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\2down.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\2up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\3up.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\AGLogo.agi
# End Source File
# Begin Source File

SOURCE=".\Res\C&PLogo.agi"
# End Source File
# Begin Source File

SOURCE=.\Res\Cacfc___.ttz
# End Source File
# Begin Source File

SOURCE=.\Ctp.rgs
# End Source File
# Begin Source File

SOURCE=.\Res\Version.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\Test.htm
# End Source File
# End Target
# End Project
