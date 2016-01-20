# Microsoft Developer Studio Project File - Name="Pmwfra32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Pmwfra32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Pmwfra32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pmwfra32.mak" CFG="Pmwfra32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pmwfra32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Pmwfra32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Pmwfra32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Pmwfra32\Release"
# PROP BASE Intermediate_Dir ".\Pmwfra32\Release"
# PROP BASE Target_Dir ".\Pmwfra32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\winRel"
# PROP Intermediate_Dir ".\winRel"
# PROP Target_Dir ".\Pmwfra32"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x40c /i "..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "Pmwfra32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Pmwfra32\Debug"
# PROP BASE Intermediate_Dir ".\Pmwfra32\Debug"
# PROP BASE Target_Dir ".\Pmwfra32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\winDebug"
# PROP Intermediate_Dir ".\winDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\Pmwfra32"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x40c /i "..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "Pmwfra32 - Win32 Release"
# Name "Pmwfra32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Pmwfra.rc
# End Source File
# Begin Source File

SOURCE=.\Pmwfra32.DEF
# End Source File
# Begin Source File

SOURCE=..\..\Pmwres.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\BuyMore.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\acnclbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ACNCLBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Add.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ADD.BMP
# End Source File
# Begin Source File

SOURCE=.\res\addr_inf.bmp
# End Source File
# Begin Source File

SOURCE=.\res\afindbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\AFINDBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Agdn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Agdn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\aglogo1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Res\aglogo1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\AgMind.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Res\AgMind.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\agtool3.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Agup.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\Agup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\all.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ALL.BMP
# End Source File
# Begin Source File

SOURCE=.\res\aopenbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\AOPENBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\res\art_icon.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\artbevel.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\artbevel.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\arttile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\arttile.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\arttitle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\arttitle.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\AStoreL.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Res\AStoreL.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Back.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\BACK.BMP
# End Source File
# Begin Source File

SOURCE=.\res\backpage.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\BACKPAGE.BMP
# End Source File
# Begin Source File

SOURCE=.\res\backzoom.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\BACKZOOM.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\ban.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\BANNPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BANNW.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BANR1.ICO
# End Source File
# Begin Source File

SOURCE=.\res\bill_inf.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Biz.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\BIZCARD.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\BIZCARDT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BIZCARDW.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BIZPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BLANK16.WAV
# End Source File
# Begin Source File

SOURCE=..\..\RES\BLANK8.WAV
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRANDNEW.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Bro.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO2T.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO2W.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO3T.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO3W.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO4T.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BRO4W.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\BROCHURE.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\BROPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\browtile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\browtile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bullet.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\BULLET.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\Cal.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALGT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALGW.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALMT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALMW.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALN1.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\res\Caltwnp.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALWT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CALWW.WMF
# End Source File
# Begin Source File

SOURCE=.\res\canclbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\CANCLBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\canlink.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CANLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\res\canpan.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CANPAN.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\cantlink.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CANTLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\car.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARD1.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARDPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARDTS.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARDTT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARDWS.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CARDWT.WMF
# End Source File
# Begin Source File

SOURCE=.\res\catalog.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\CATALOG.ICO
# End Source File
# Begin Source File

SOURCE=.\res\category.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Cer.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\CERTIF.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\CERTIFL.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CERTIFP.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\CERTPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Close.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\CLOSE.BMP
# End Source File
# Begin Source File

SOURCE=.\res\closesml.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\CLOSESML.BMP
# End Source File
# Begin Source File

SOURCE=.\res\combobut.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\COMBOBUT.BMP
# End Source File
# Begin Source File

SOURCE=.\res\create.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CREATE.CUR
# End Source File
# Begin Source File

SOURCE=.\res\crop.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\CROP.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\CSDISCLM.TXT
# End Source File
# Begin Source File

SOURCE=.\res\dblbegin.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\DBLBEGIN.ICO
# End Source File
# Begin Source File

SOURCE=.\res\dblprint.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\DBLPRINT.BMP
# End Source File
# Begin Source File

SOURCE=.\res\dblprmpt.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\DBLPRMPT.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\Delete.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\DELETE.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\desktile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\desktile.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dialup.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\dialup.ico
# End Source File
# Begin Source File

SOURCE=.\RES\direct_c.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\direct_c.ico
# End Source File
# Begin Source File

SOURCE=.\res\downmark.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\DRAW.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\edit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\EDIT.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\EDITTEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\res\elli_cre.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\ELLI_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\Env.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\ENV_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENV_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENVELOPE.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENVPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\ENVW.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_TRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WCD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WCDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WCU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WCUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WLD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WLDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WLU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WLUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WRD0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WRDN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WRU0.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\EVD_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\EVD_WRUN.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\Fax.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\FAXCOVER.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\FAXL.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\FAXP.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\FAXPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\res\findbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\FINDBTN.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\First.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\FIRST.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\FOURARRO.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\FOURARRO.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\GLOBAL.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryABdr.bmp
# End Source File
# Begin Source File

SOURCE=..\..\glrybk1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\glrybk1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryBk2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryPBdr.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryPBk1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryProj.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\GlryTtlA.bmp
# End Source File
# Begin Source File

SOURCE=.\res\grayhtch.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\HALFCARD.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\HCARDT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\HCARDW.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Hcr.ico
# End Source File
# Begin Source File

SOURCE=.\res\helpcld.bmp
# End Source File
# Begin Source File

SOURCE=.\res\helpclu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hintclsd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hintclsu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintCntD.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintCntU.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintHd1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\HintHd1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintHd2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\HintHd2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hinthlpD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\hinthlpD.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hinthlpU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\hinthlpU.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintTutD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\HintTutD.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HintTutU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\HintTutU.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ILLEGAL.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\ILLEGAL.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\import.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\IMPORT.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\ITOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\LABEL.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\LABPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Last.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LAST.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Lbl.ico
# End Source File
# Begin Source File

SOURCE=.\res\left_ar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\leftmark.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Let.ico
# End Source File
# Begin Source File

SOURCE=.\res\line_cre.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\LINE_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\res\linkfrm.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\LINKFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\master.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\MASTER.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\master2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\MODIFY.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\MOREDOWN.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\MoreInfD.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\MoreInfU.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\MOREUP.WMF
# End Source File
# Begin Source File

SOURCE=.\res\move_hgu.cur
# End Source File
# Begin Source File

SOURCE=..\..\Res\move_hgu.cur
# End Source File
# Begin Source File

SOURCE=.\res\move_vgu.cur
# End Source File
# Begin Source File

SOURCE=..\..\Res\move_vgu.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\NEWSLTR.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\Next.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\NEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\res\nextfrm.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\NEXTFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\res\nextpage.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\NEXTPAGE.BMP
# End Source File
# Begin Source File

SOURCE=.\res\nextzoom.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\NEXTZOOM.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\NFX32.BMP
# End Source File
# Begin Source File

SOURCE=.\res\none.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\NONE.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Not.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTECARD.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTEPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTETS.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTETT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTEWS.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NOTEWT.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Nws.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\NWSPRJ.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NWST.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\NWSW.WMF
# End Source File
# Begin Source File

SOURCE=.\res\okbtn.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\OKBTN.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\ORDERFRM.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\PAGENEXT.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\PAGEPREV.WMF
# End Source File
# Begin Source File

SOURCE=.\res\panning.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\PANNING.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\paprtile.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Pcr.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\PHILFRNK.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\PICT32.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\PICTURES.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\PLACE.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\PLACE_CU.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\PLACE_CU.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\PMGW1.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\PMGW1.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Pmw.rc2
# End Source File
# Begin Source File

SOURCE=.\res\posind.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\POSIND.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\POST1.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\POSTCARD.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\POSTERL.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\POSTERP.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\POSTPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\res\prevfrm.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PREVFRM.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT1.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print13.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT13.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print13r.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT13R.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print14.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT14.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print14r.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT14R.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT2.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print2n3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT2N3.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print2n4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT2N4.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print2r3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT2R3.BMP
# End Source File
# Begin Source File

SOURCE=.\res\print2r4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PRINT2R4.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\probevel.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\probevel.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\PROJ32.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\PROJECTS.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\protile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\protile.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\protitle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\protitle.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\PROX.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\PROX.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\PWORKS32.BMP
# End Source File
# Begin Source File

SOURCE=.\res\rect_cre.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\RECT_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\RESIZE.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\RESIZE.CUR
# End Source File
# Begin Source File

SOURCE=..\..\Res\resize_c.cur
# End Source File
# Begin Source File

SOURCE=..\..\Res\resize_r.cur
# End Source File
# Begin Source File

SOURCE=.\res\rghtmark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\right_ar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ROTATE.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\ROTATE.CUR
# End Source File
# Begin Source File

SOURCE=.\res\rotind.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\ROTIND.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\Save.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SAVE.BMP
# End Source File
# Begin Source File

SOURCE=..\..\Res\select_c.cur
# End Source File
# Begin Source File

SOURCE=..\..\Res\select_r.cur
# End Source File
# Begin Source File

SOURCE=.\RES\selrc0.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc0.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc135.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc135.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc180.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc180.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc225.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc225.cur
# End Source File
# Begin Source File

SOURCE=.\RES\selrc270.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc270.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc315.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc315.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc45.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc45.cur
# End Source File
# Begin Source File

SOURCE=.\res\selrc90.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\selrc90.cur
# End Source File
# Begin Source File

SOURCE=.\RES\senbevel.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\senbevel.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SENT32.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\sentile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sentile.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\sentitle.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\sentitle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shadows.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SHADOWS.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\sig.ico
# End Source File
# Begin Source File

SOURCE=.\res\sizeind.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\SIZEIND.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\SOUND_EX.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\SOUND_PL.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\SOUND_SP.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\SOUND_ST.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\SOUNDHLP.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\SOUNDHLP.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\SPIRAL.WMF
# End Source File
# Begin Source File

SOURCE=.\res\split.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\SPLIT.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\SPLIT135.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\SPLIT135.cur
# End Source File
# Begin Source File

SOURCE=.\RES\SPLIT45.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\SPLIT45.cur
# End Source File
# Begin Source File

SOURCE=.\RES\Splith.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Splith.cur
# End Source File
# Begin Source File

SOURCE=.\RES\Splitv.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\Splitv.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\STATNRY.ICO
# End Source File
# Begin Source File

SOURCE=..\..\RES\STATNRYL.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\STATNRYP.WMF
# End Source File
# Begin Source File

SOURCE=..\..\RES\STATPRJ.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Sti.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\sticker.ico
# End Source File
# Begin Source File

SOURCE=.\RES\tcbutton.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\TCBUTTON.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\tcnarrow.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\TCNARROW.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\tcsarrow.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\TCSARROW.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\TEXT.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\TEXT_CRE.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\TEXT_CRE.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\tool1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tool1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\tool2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tool2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\tool3.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tool3.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\tool4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tool4.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\TOOLBARS.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\tooltile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\tooltile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\TOONS.WMF
# End Source File
# Begin Source File

SOURCE=.\RES\Tsh.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\tshirt.ico
# End Source File
# Begin Source File

SOURCE=.\RES\Undo.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\UNDO.BMP
# End Source File
# Begin Source File

SOURCE=.\res\unlink.cur
# End Source File
# Begin Source File

SOURCE=..\..\RES\UNLINK.CUR
# End Source File
# Begin Source File

SOURCE=.\res\upmark.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Web.ico
# End Source File
# Begin Source File

SOURCE=..\..\RES\WEB.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\whtile.bmp
# End Source File
# Begin Source File

SOURCE=..\..\RES\WITHHELP.BMP
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMADDR.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMAGAL.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMBNEW.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMINET.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMNFX.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMPHOT.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMRMAD.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMSENT.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMWHLP.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMWORK.TXT
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMYPRJ.TXT
# End Source File
# Begin Source File

SOURCE=.\RES\ZOOM.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\ZOOM.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\ZOOMIN.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\ZOOMIN.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\ZOOMOUT.CUR
# End Source File
# Begin Source File

SOURCE=..\..\RES\ZOOMOUT.CUR
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\RES\Bizcardt._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Bizcardw._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro2t._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro2w._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro3t._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro3w._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro4t._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\bro4w._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmll.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmlp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmnl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmnp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmtl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calmtp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\caltm.wmf
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmll._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmlp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmnl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmnp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmsl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmsp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmtl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltmtp._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\caltw.wmf
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwll._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwlp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwnl._bm
# End Source File
# Begin Source File

SOURCE=..\..\res\Caltwnp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwsl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwsp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwtl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltwtp._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\calty.wmf
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltyll._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltylp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltynl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltynp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltysl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltysp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\caltytl._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\CALTYTP._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwll.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwlp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwnl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwnp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwtl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calwtp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calyll.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calylp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calynl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calynp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calytl.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\calytp.wmf
# End Source File
# Begin Source File

SOURCE=..\..\Res\Cardts._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Cardtt._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Cardws._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Cardwt._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Certifl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Certifp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Faxl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Faxp._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Hcardt._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Hcardw._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Notets._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Notett._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Notews._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Notewt._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Nwsdt._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\nwsdw._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Nwsst._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Nwssw._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Posterl._bm
# End Source File
# Begin Source File

SOURCE=..\..\Res\Posterp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Statnryl._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Statnryp._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\Under._bm
# End Source File
# Begin Source File

SOURCE=..\..\RES\WLCMHUB.TXT
# End Source File
# End Target
# End Project
