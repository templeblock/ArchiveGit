# Microsoft Developer Studio Project File - Name="Install Program" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Install Program - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "INST32.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "INST32.MAK" CFG="Install Program - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Install Program - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Install Program - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Install Program - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\WinRel"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /Ot /Oi /Oy /Ob1 /Gf /Gy /I "d:\msdev\mfc\src" /I ".\Resource" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INSTALL" /Yu"STDAFX.H" /FD /c
# SUBTRACT CPP /Ox /Oa /Ow /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib ver.lib /nologo /stack:0x20480 /subsystem:windows /machine:IX86
# ADD LINK32 oldnames.lib winmm.lib version.lib app\al201\v4x\alfw.lib /nologo /subsystem:windows /map /machine:IX86 /out:"..\WinRel\install.exe"

!ELSEIF  "$(CFG)" == "Install Program - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\WinDebug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /I "d:\msdev\mfc\src" /I ".\Resource" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INSTALL" /Yu"STDAFX.H" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib ver.lib /nologo /stack:0x20480 /subsystem:windows /debug /machine:IX86
# ADD LINK32 oldnames.lib winmm.lib version.lib app\al201\v4x\alfw.lib /nologo /subsystem:windows /debug /machine:IX86 /out:"..\WinDebug\install.exe"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "Install Program - Win32 Release"
# Name "Install Program - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\App\CDEFLATE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CHDIRDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CPFL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CPROGBAR.CPP
# End Source File
# Begin Source File

SOURCE=.\App\DDB.CPP
# End Source File
# Begin Source File

SOURCE=.\App\DDE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\globatom.cpp
# End Source File
# Begin Source File

SOURCE=.\App\GRADIENT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\IABOUT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\ICONGRAT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\IDEFAULT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\IDSKSPC.CPP
# End Source File
# Begin Source File

SOURCE=.\App\iexplore.cpp
# End Source File
# Begin Source File

SOURCE=.\App\iispDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Inifile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\INSTALL.CPP
# End Source File
# Begin Source File

SOURCE=.\Resource\INSTALL.RC
# End Source File
# Begin Source File

SOURCE=.\App\INSTDAT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\IOPTIONS.CPP
# End Source File
# Begin Source File

SOURCE=.\App\IPROGRES.CPP
# End Source File
# Begin Source File

SOURCE=.\App\MASKEDBM.CPP
# End Source File
# Begin Source File

SOURCE=.\App\OD.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Pmwcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Rfor.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SORTARRY.CPP
# End Source File
# Begin Source File

SOURCE=.\App\SPARKLE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\SPINBUTT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\STDAFX.CPP
# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"
# End Source File
# Begin Source File

SOURCE=.\App\STRCLASS.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\App\array.h
# End Source File
# Begin Source File

SOURCE=.\App\CHDIRDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\CPFL.H
# End Source File
# Begin Source File

SOURCE=.\App\CPROGBAR.H
# End Source File
# Begin Source File

SOURCE=.\App\database.h
# End Source File
# Begin Source File

SOURCE=.\App\DB386.H
# End Source File
# Begin Source File

SOURCE=.\App\dbstore.h
# End Source File
# Begin Source File

SOURCE=.\App\DDB.H
# End Source File
# Begin Source File

SOURCE=.\App\error.h
# End Source File
# Begin Source File

SOURCE=.\App\GLOBATOM.H
# End Source File
# Begin Source File

SOURCE=.\App\GRADIENT.H
# End Source File
# Begin Source File

SOURCE=.\App\heap.h
# End Source File
# Begin Source File

SOURCE=.\App\IABOUT.H
# End Source File
# Begin Source File

SOURCE=.\App\IAGUIDE.H
# End Source File
# Begin Source File

SOURCE=.\App\ICONGRAT.H
# End Source File
# Begin Source File

SOURCE=.\App\IDEFAULT.H
# End Source File
# Begin Source File

SOURCE=.\App\IDISKSPC.H
# End Source File
# Begin Source File

SOURCE=.\App\iexplore.h
# End Source File
# Begin Source File

SOURCE=.\App\iispDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\INSTALL.H
# End Source File
# Begin Source File

SOURCE=.\App\INSTDAT.H
# End Source File
# Begin Source File

SOURCE=.\App\IOPTIONS.H
# End Source File
# Begin Source File

SOURCE=.\App\IPROGRES.H
# End Source File
# Begin Source File

SOURCE=.\App\list.h
# End Source File
# Begin Source File

SOURCE=.\App\MASKEDBM.H
# End Source File
# Begin Source File

SOURCE=.\App\mciwnd.h
# End Source File
# Begin Source File

SOURCE=.\App\misc.h
# End Source File
# Begin Source File

SOURCE=.\App\pagedefs.h
# End Source File
# Begin Source File

SOURCE=.\App\Pmw.h
# End Source File
# Begin Source File

SOURCE=.\App\PMWCFG.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\profile.h
# End Source File
# Begin Source File

SOURCE=.\App\projtext.h
# End Source File
# Begin Source File

SOURCE=.\App\record.h
# End Source File
# Begin Source File

SOURCE=.\App\redisp.h
# End Source File
# Begin Source File

SOURCE=.\App\Rfor.h
# End Source File
# Begin Source File

SOURCE=.\App\rwsem.h
# End Source File
# Begin Source File

SOURCE=.\App\sortarry.h
# End Source File
# Begin Source File

SOURCE=.\App\SPARKLE.H
# End Source File
# Begin Source File

SOURCE=.\App\SPINBUTT.H
# End Source File
# Begin Source File

SOURCE=.\App\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\App\storeman.h
# End Source File
# Begin Source File

SOURCE=.\App\strclass.h
# End Source File
# Begin Source File

SOURCE=.\App\system.h
# End Source File
# Begin Source File

SOURCE=.\App\tableman.h
# End Source File
# Begin Source File

SOURCE=.\App\Toolbarx.h
# End Source File
# Begin Source File

SOURCE=.\App\util.h
# End Source File
# Begin Source File

SOURCE=.\App\utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Resource\Shared\About1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\About4.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\About8.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\attsetup.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Blank16.wav
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Blank8.wav
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bptitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Bptitle8.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\cltitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cltitle8.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\dmtitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Dmtitle8.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Illegal.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Install.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PSTITLE.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Pstitle8.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Soundhlp.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Sparkle4.bmp
# End Source File
# End Group
# End Target
# End Project
