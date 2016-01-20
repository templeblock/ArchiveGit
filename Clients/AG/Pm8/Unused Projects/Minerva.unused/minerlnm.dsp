# Microsoft Developer Studio Project File - Name="minerlnm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=minerlnm - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "minerlnm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minerlnm.mak" CFG="minerlnm - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minerlnm - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "minerlnm - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minerlnm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\BIN"
# PROP Intermediate_Dir "MLNM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W4 /WX /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "SSM_RES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\lib\pnlib20m.lib ctl3d32s.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\BIN"
# PROP Intermediate_Dir "MLNMD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "SSM_RES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\..\lib\picn20m.lib ctl3d32s.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "minerlnm - Win32 Release"
# Name "minerlnm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\mabout.c
# End Source File
# Begin Source File

SOURCE=.\Mchild.c
# End Source File
# Begin Source File

SOURCE=.\mctrl.c
# End Source File
# Begin Source File

SOURCE=.\Mdlg.c
# End Source File
# Begin Source File

SOURCE=.\merror.c
# End Source File
# Begin Source File

SOURCE=.\mframe.c
# End Source File
# Begin Source File

SOURCE=.\minerva.c
# End Source File
# Begin Source File

SOURCE=.\minerva.rc
# End Source File
# Begin Source File

SOURCE=.\minervab.csm
# End Source File
# Begin Source File

SOURCE=.\Minervbl.csm
# End Source File
# Begin Source File

SOURCE=.\mmenu.c
# End Source File
# Begin Source File

SOURCE=.\mmisc.c
# End Source File
# Begin Source File

SOURCE=.\Mmru.c
# End Source File
# Begin Source File

SOURCE=.\Mobmp.c
# End Source File
# Begin Source File

SOURCE=.\Moim4.c
# End Source File
# Begin Source File

SOURCE=.\moimstar.c
# End Source File
# Begin Source File

SOURCE=.\Mopen.c
# End Source File
# Begin Source File

SOURCE=.\mopng.c
# End Source File
# Begin Source File

SOURCE=.\Moprgjpg.c
# End Source File
# Begin Source File

SOURCE=.\Moseqjpg.c
# End Source File
# Begin Source File

SOURCE=.\moutil81.c
# End Source File
# Begin Source File

SOURCE=.\mowavlet.c
# End Source File
# Begin Source File

SOURCE=.\Msave.c
# End Source File
# Begin Source File

SOURCE=.\msimstar.c
# End Source File
# Begin Source File

SOURCE=.\Msprgjpg.c
# End Source File
# Begin Source File

SOURCE=.\msseqjpg.c
# End Source File
# Begin Source File

SOURCE=.\msutil80.c
# End Source File
# Begin Source File

SOURCE=.\mswavlet.c
# End Source File
# Begin Source File

SOURCE=.\mtutil82.c
# End Source File
# Begin Source File

SOURCE=.\Mxp2s.c
# End Source File
# Begin Source File

SOURCE=.\mxrotate.c
# End Source File
# Begin Source File

SOURCE=.\mxs2p.c
# End Source File
# Begin Source File

SOURCE=.\mxzoom.c
# End Source File
# Begin Source File

SOURCE=.\pic2list.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ctl3d.h
# End Source File
# Begin Source File

SOURCE=.\mabout.h
# End Source File
# Begin Source File

SOURCE=.\Mchild.h
# End Source File
# Begin Source File

SOURCE=.\mctrl.h
# End Source File
# Begin Source File

SOURCE=.\mdlg.h
# End Source File
# Begin Source File

SOURCE=.\merror.h
# End Source File
# Begin Source File

SOURCE=.\Mframe.h
# End Source File
# Begin Source File

SOURCE=.\minerva.h
# End Source File
# Begin Source File

SOURCE=.\Mmenu.h
# End Source File
# Begin Source File

SOURCE=.\Mmisc.h
# End Source File
# Begin Source File

SOURCE=.\Mmru.h
# End Source File
# Begin Source File

SOURCE=.\Mopcodes.h
# End Source File
# Begin Source File

SOURCE=.\Mopen.h
# End Source File
# Begin Source File

SOURCE=.\Msave.h
# End Source File
# Begin Source File

SOURCE=.\pic2list.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\3DCHECK.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\H_MOVE.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\MCHILD.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\MFRAME.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\PEGASUS.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Bin\picn1820.SSM
# End Source File
# Begin Source File

SOURCE=.\Res\picn1820.ssm
# End Source File
# End Target
# End Project
