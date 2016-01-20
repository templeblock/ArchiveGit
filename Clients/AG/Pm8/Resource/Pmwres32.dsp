# Microsoft Developer Studio Project File - Name="pmwres32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pmwres32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Pmwres32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pmwres32.mak" CFG="pmwres32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pmwres32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pmwres32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pmwres32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\pmwres32\Release"
# PROP BASE Intermediate_Dir ".\pmwres32\Release"
# PROP BASE Target_Dir ".\pmwres32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\winRel"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\pmwres32"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\App" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "pmwres32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\pmwres32\Debug"
# PROP BASE Intermediate_Dir ".\pmwres32\Debug"
# PROP BASE Target_Dir ".\pmwres32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\winDebug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\pmwres32"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "pmwres32 - Win32 Release"
# Name "pmwres32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Pmw.rc
# End Source File
# Begin Source File

SOURCE=.\Pmwres.c
# End Source File
# Begin Source File

SOURCE=.\Pmwres32.def
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\1NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\1Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\2NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\2Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\3NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\3Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\4NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\4Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\5NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\5Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\6NumbD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\6Numbu.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ACNCLBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ADD.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\addr_inf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\AFINDBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\AGBrowTile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Agdn.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\aglogo1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\AgMind.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\agsenside.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\agsentile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\agsentitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\agtool3.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Agup.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ALL.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\AOPENBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\app_icon.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\art_icon.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\artbevel.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\arttile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\arttitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\AStoreL.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BACK.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BACKPAGE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BACKZOOM.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ban.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BANNPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BANNW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BANR1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bill_inf.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Biz.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BIZCARD.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BIZCARDT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BIZCARDW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BIZPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\blank.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BLANK16.WAV
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BLANK8.WAV
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\BoldFont.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\BoldFont_F.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\BoldFont_X.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\border_t.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRANDNEW.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Bro.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO2T.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO2W.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO3T.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO3W.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO4T.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BRO4W.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BROCHURE.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BROPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\browtile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\BULLET.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cal.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALGT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALGW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALMT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALMW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALN1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Caltwnp.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALWT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALWW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CANCLBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CANLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CANPAN.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CANTLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\car.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARD1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARDPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARDTS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARDTT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARDWS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CARDWT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CATALOG.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\category.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cer.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CERTIF.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CERTIFL.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CERTIFP.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CERTPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\cft.ico
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\CirbulD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\Cirbulu.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\CirOtbulD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\CirOtbulu.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CLOSE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CLOSESML.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\COMBOBUT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CREATE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CROP.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CSDISCLM.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\DBLBEGIN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\DBLPRINT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\DBLPRMPT.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\DELETE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\desktile.bmp
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\DiabulD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\Diabulu.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\dialup.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\direct_c.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\downmark.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\DRAW.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EDIT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EDITTEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ELLI_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Env.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENV_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENVELOPE.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENVPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ENVW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\EVD_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Fax.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FAXCOVER.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FAXL.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FAXP.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FAXPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FINDBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FIRST.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\FOURARRO.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GLOBAL.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryABdr.bmp
# End Source File
# Begin Source File

SOURCE=.\glrybk1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\glrybk1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryBk2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryPBdr.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryPBk1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryProj.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\GlryTtlA.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\grayhtch.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HALFCARD.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HCARDT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HCARDW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Hcr.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\helpcld.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\helpclu.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hintclsd.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hintclsu.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hintcntd.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hintcntu.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HintHd1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HintHd2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hinthlpD.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\hinthlpU.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HintTutD.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\HintTutU.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ILLEGAL.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\IMPORT.BMP
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\ItalicFont.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\ItalicFont_F.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\GridAttrib\ItalicFont_X.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ITOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\LABEL.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\LABPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\LAST.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Lbl.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\left_ar.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\leftmark.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Let.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\LINE_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\LINKFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\madn.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MASTER.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\master2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\maup.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MODIFY.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MOREDOWN.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MoreInfD.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MoreInfU.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\MOREUP.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\move_hgu.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\move_vgu.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NEWSLTR.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NEXTFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NEXTPAGE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NEXTZOOM.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NFX32.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NONE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Not.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTECARD.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTEPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTETS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTETT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTEWS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NOTEWT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Nws.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NWSPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NWST.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\NWSW.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ogdnn.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ogupn.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\OKBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ORDERFRM.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PAGENEXT.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PAGEPREV.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PANNING.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\paprtile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Pcr.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PHILFRNK.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\photoprojects.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PICT32.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PICTURES.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PLACE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PLACE_CU.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PMGW1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PMW.RC2
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POSIND.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POST1.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POSTCARD.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POSTERL.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POSTERP.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\POSTPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PREVFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT1.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT13.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT13R.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT14.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT14R.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT2.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT2N3.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT2N4.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT2R3.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PRINT2R4.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\probevel.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PROJ32.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PROJECTS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\protile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\protitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PROX.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\PWORKS32.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\RECT_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\RESIZE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\resize_c.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\resize_r.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\rghtmark.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\right_ar.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ROTATE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ROTIND.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SAVE.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\select_c.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\select_r.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc0.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc135.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc180.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc225.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc270.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc315.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc45.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\selrc90.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\senbevel.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SENT32.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\sentile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\sentitle.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SHADOWS.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\sig.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SIZEIND.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SOUND_EX.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SOUND_PL.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SOUND_SP.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SOUND_ST.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SOUNDHLP.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SPIRAL.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SPLIT.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SPLIT135.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\SPLIT45.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Splith.cur
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Splitv.cur
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\SqrbulD.bmp"
# End Source File
# Begin Source File

SOURCE=".\Resource\Shared\Button Bmps\bullets\Sqrbulu.bmp"
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\STATNRY.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\STATNRYL.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\STATNRYP.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\STATPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Sti.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\sticker.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TCBUTTON.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TCNARROW.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TCSARROW.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TEXT_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tool1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tool2.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tool3.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tool4.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TOOLBARS.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tooltile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\TOONS.WMF
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Tsh.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\tshirt.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\UNDO.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\UNLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\upmark.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WEB.ICO
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\whtile.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WITHHELP.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMADDR.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMAGAL.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMBNEW.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMINET.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMNFX.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMPHOT.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMRMAD.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMSENT.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMWHLP.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMWORK.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\WLCMYPRJ.TXT
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ZOOM.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ZOOMIN.CUR
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ZOOMOUT.CUR
# End Source File
# End Group
# Begin Source File

SOURCE=.\Resource\Shared\Bizcardt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Bizcardw._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\blankl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\blankp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro2t._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro2w._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro3t._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro3w._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro4t._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\bro4w._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmll.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmlp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmnl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmnp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmtl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calmtp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltm.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmll._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmlp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmnl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmnp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmsl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmsp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmtl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltmtp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltw.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwll._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwlp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwnl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Caltwnp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwsl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwsp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwtl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltwtp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calty.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltyll._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltylp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltynl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltynp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltysl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltysp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\caltytl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\CALTYTP._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwll.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwlp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwnl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwnp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwtl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calwtp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calyll.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calylp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calynl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calynp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calytl.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\calytp.wmf
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cardts._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cardtt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cardws._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Cardwt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Certifl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Certifp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Faxl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Faxp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Hcardt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Hcardw._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Notets._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Notett._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Notews._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Notewt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Nwsdt._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\nwsdw._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Nwsst._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Nwssw._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ogdn.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\ogup.BMP
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Posterl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Posterp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Statnryl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Statnryp._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Under._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whcal._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whcard._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whlabl._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whnews._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whpost._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whstat._bm
# End Source File
# Begin Source File

SOURCE=.\Resource\Shared\Whweb._bm
# End Source File
# End Target
# End Project
