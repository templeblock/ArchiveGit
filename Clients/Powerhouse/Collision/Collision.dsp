# Microsoft Developer Studio Project File - Name="cllsn32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cllsn32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Collision.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Collision.mak" CFG="cllsn32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cllsn32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cllsn32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Ox /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\Release\Collision.res" /i "..\gameguts" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe"
# SUBTRACT LINK32 /pdb:none /incremental:yes /nodefaultlib

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Zi /Od /Ob1 /Gy /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /Fd"DEBUG.PDB" /c
# ADD CPP /nologo /G5 /W3 /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr /Fd".\DEBUG32.PDB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\Debug\Collision.res" /i "..\gameguts" /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows /incremental:no /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "cllsn32 - Win32 Release"
# Name "cllsn32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AmbientSnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\amovie.cpp
# End Source File
# Begin Source File

SOURCE=.\author.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Bcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\CarShots.cpp
# End Source File
# Begin Source File

SOURCE=.\Cells.cpp
# End Source File
# Begin Source File

SOURCE=.\cllsn.CPP
# End Source File
# Begin Source File

SOURCE=.\Collect.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\credits.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\dib.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\dirsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\GameState.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\hcontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Hslider.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\httables.cpp
# End Source File
# Begin Source File

SOURCE=.\Levels.cpp
# End Source File
# Begin Source File

SOURCE=.\line.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\mci.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Mcontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\mmiojunk.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\offscren.cpp
# End Source File
# Begin Source File

SOURCE=.\Opponents.cpp
# End Source File
# Begin Source File

SOURCE=.\options.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\pal.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\parser.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\phapp.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\randseq.cpp
# End Source File
# Begin Source File

SOURCE=.\Rcontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\scenes.hxx
# End Source File
# Begin Source File

SOURCE=.\Score.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\script.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\sound.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\speedchk.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\sprite.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\transit.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\vcontrol.cpp
# End Source File
# Begin Source File

SOURCE=.\Vid3d.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\Video.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\videodrw.cpp
# End Source File
# Begin Source File

SOURCE=.\videofx.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\vtable.cpp
# End Source File
# Begin Source File

SOURCE=.\vworld.cpp
# End Source File
# Begin Source File

SOURCE=.\wdebug.cpp
# End Source File
# Begin Source File

SOURCE=.\weapon.cpp
# End Source File
# Begin Source File

SOURCE=..\Gameguts\wintools.cpp
# End Source File
# Begin Source File

SOURCE=.\Worlds.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AmbientSnd.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\amovie.h
# End Source File
# Begin Source File

SOURCE=.\Author.h
# End Source File
# Begin Source File

SOURCE=.\author.p
# End Source File
# Begin Source File

SOURCE=.\CarShots.h
# End Source File
# Begin Source File

SOURCE=.\cells.h
# End Source File
# Begin Source File

SOURCE=.\Cllsndef.h
# End Source File
# Begin Source File

SOURCE=.\cllsnid.h
# End Source File
# Begin Source File

SOURCE=.\Collect.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\credits.h
# End Source File
# Begin Source File

SOURCE=.\Data3d.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\dib.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\dirsnd.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\GameState.h
# End Source File
# Begin Source File

SOURCE=.\Levels.h
# End Source File
# Begin Source File

SOURCE=.\line.p
# End Source File
# Begin Source File

SOURCE=.\mmiojunk.p
# End Source File
# Begin Source File

SOURCE=.\offcells.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\OFFSCREN.H
# End Source File
# Begin Source File

SOURCE=.\Opponents.h
# End Source File
# Begin Source File

SOURCE=.\options.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\parser.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\phapp.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\randseq.h
# End Source File
# Begin Source File

SOURCE=.\rcontrol.p
# End Source File
# Begin Source File

SOURCE=..\Gameguts\resource.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\scene.h
# End Source File
# Begin Source File

SOURCE=.\Score.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\script.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\sound.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\SPRITE.H
# End Source File
# Begin Source File

SOURCE=..\Gameguts\transit.h
# End Source File
# Begin Source File

SOURCE=..\Gameguts\vcontrol.h
# End Source File
# Begin Source File

SOURCE=.\Vid3d.h
# End Source File
# Begin Source File

SOURCE=.\videofx.h
# End Source File
# Begin Source File

SOURCE=..\gameguts\vtable.h
# End Source File
# Begin Source File

SOURCE=.\vworld.h
# End Source File
# Begin Source File

SOURCE=.\wdebug.p
# End Source File
# Begin Source File

SOURCE=.\Weapon.h
# End Source File
# Begin Source File

SOURCE=.\Worlds.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Group "Resource Scripts"

# PROP Default_Filter "*.rc"
# Begin Source File

SOURCE=.\Author.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\cllsn.rc

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WDebug.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Wave Files"

# PROP Default_Filter "*.wav"
# Begin Source File

SOURCE=.\111mchng.wav
# End Source File
# Begin Source File

SOURCE=.\112mchng.wav
# End Source File
# Begin Source File

SOURCE=.\12rcktln.wav
# End Source File
# Begin Source File

SOURCE=.\13lsrfsn.wav
# End Source File
# Begin Source File

SOURCE=.\2112pwkx.wav
# End Source File
# Begin Source File

SOURCE=.\341wndsh.wav
# End Source File
# Begin Source File

SOURCE=.\41prtlnr.wav
# End Source File
# Begin Source File

SOURCE=.\71crshth.wav
# End Source File
# Begin Source File

SOURCE=.\boss.wav
# End Source File
# Begin Source File

SOURCE=.\carsound.wav
# End Source File
# Begin Source File

SOURCE=.\Driver11.wav
# End Source File
# Begin Source File

SOURCE=.\energy1.wav
# End Source File
# Begin Source File

SOURCE=.\expsmall.wav
# End Source File
# Begin Source File

SOURCE=.\goodbye.wav
# End Source File
# Begin Source File

SOURCE=.\jack.wav
# End Source File
# Begin Source File

SOURCE=.\jackatt.wav
# End Source File
# Begin Source File

SOURCE=.\lexpbig.wav
# End Source File
# Begin Source File

SOURCE=.\lexpsmal.wav
# End Source File
# Begin Source File

SOURCE=.\lrfloat.wav
# End Source File
# Begin Source File

SOURCE=.\lrgrowl.wav
# End Source File
# Begin Source File

SOURCE=.\minebeep.wav
# End Source File
# Begin Source File

SOURCE=.\minelnch.wav
# End Source File
# Begin Source File

SOURCE=.\oppin.wav
# End Source File
# Begin Source File

SOURCE=.\portal.wav
# End Source File
# Begin Source File

SOURCE=.\scrap1.wav
# End Source File
# Begin Source File

SOURCE=.\shot.wav
# End Source File
# Begin Source File

SOURCE=.\spklnch.wav
# End Source File
# Begin Source File

SOURCE=.\strong.wav
# End Source File
# Begin Source File

SOURCE=.\switch.wav
# End Source File
# Begin Source File

SOURCE=.\swtchof8.wav
# End Source File
# Begin Source File

SOURCE=.\thmplnch.wav
# End Source File
# Begin Source File

SOURCE=.\thumpexp.wav
# End Source File
# Begin Source File

SOURCE=.\turn.wav
# End Source File
# Begin Source File

SOURCE=.\turn180.wav
# End Source File
# Begin Source File

SOURCE=.\wallcrsh.wav
# End Source File
# Begin Source File

SOURCE=.\weak.wav
# End Source File
# End Group
# Begin Group "Icon Files"

# PROP Default_Filter "*.ico"
# Begin Source File

SOURCE=.\cllsn.ico
# End Source File
# Begin Source File

SOURCE=.\hpoint.cur
# End Source File
# Begin Source File

SOURCE=.\no.cur
# End Source File
# End Group
# Begin Group "Bitmap Files"

# PROP Default_Filter "*.bmp"
# Begin Source File

SOURCE=.\BCRACK1.bmp
# End Source File
# Begin Source File

SOURCE=.\BCRACK2.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker1.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker1x.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker2.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker2x.bmp
# End Source File
# Begin Source File

SOURCE=.\BREAKER3.bmp
# End Source File
# Begin Source File

SOURCE=.\BREAKER3X.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker4.bmp
# End Source File
# Begin Source File

SOURCE=.\breaker4x.bmp
# End Source File
# Begin Source File

SOURCE=.\BREAKER5.bmp
# End Source File
# Begin Source File

SOURCE=.\BREAKER5X.bmp
# End Source File
# Begin Source File

SOURCE=.\buttin.bmp
# End Source File
# Begin Source File

SOURCE=.\buttout.bmp
# End Source File
# Begin Source File

SOURCE=.\cancelin.bmp
# End Source File
# Begin Source File

SOURCE=.\cancelot.bmp
# End Source File
# Begin Source File

SOURCE=.\col01_01.bmp
# End Source File
# Begin Source File

SOURCE=.\col02_01.bmp
# End Source File
# Begin Source File

SOURCE=.\col02_02.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_01.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_02.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_03.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_04.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_05.bmp
# End Source File
# Begin Source File

SOURCE=.\col04_06.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_01.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_02.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_03.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_04.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_05.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_06.bmp
# End Source File
# Begin Source File

SOURCE=.\col06_07.bmp
# End Source File
# Begin Source File

SOURCE=.\col07_01.bmp
# End Source File
# Begin Source File

SOURCE=.\col07_02.bmp
# End Source File
# Begin Source File

SOURCE=.\col07_03.bmp
# End Source File
# Begin Source File

SOURCE=.\col07_04.bmp
# End Source File
# Begin Source File

SOURCE=.\col07_05.bmp
# End Source File
# Begin Source File

SOURCE=.\colblank.bmp
# End Source File
# Begin Source File

SOURCE=.\creditin.bmp
# End Source File
# Begin Source File

SOURCE=.\creditot.bmp
# End Source File
# Begin Source File

SOURCE=.\digit0.bmp
# End Source File
# Begin Source File

SOURCE=.\digit1.bmp
# End Source File
# Begin Source File

SOURCE=.\digit2.bmp
# End Source File
# Begin Source File

SOURCE=.\digit3.bmp
# End Source File
# Begin Source File

SOURCE=.\digit4.bmp
# End Source File
# Begin Source File

SOURCE=.\digit5.bmp
# End Source File
# Begin Source File

SOURCE=.\digit6.bmp
# End Source File
# Begin Source File

SOURCE=.\digit7.bmp
# End Source File
# Begin Source File

SOURCE=.\digit8.bmp
# End Source File
# Begin Source File

SOURCE=.\digit9.bmp
# End Source File
# Begin Source File

SOURCE=.\energy_a.bmp
# End Source File
# Begin Source File

SOURCE=.\energy_b.bmp
# End Source File
# Begin Source File

SOURCE=.\exitin.bmp
# End Source File
# Begin Source File

SOURCE=.\exitout.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP100.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP101.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP102.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP103.bmp
# End Source File
# Begin Source File

SOURCE=.\exp200.bmp
# End Source File
# Begin Source File

SOURCE=.\exp201.bmp
# End Source File
# Begin Source File

SOURCE=.\exp202.bmp
# End Source File
# Begin Source File

SOURCE=.\exp203.bmp
# End Source File
# Begin Source File

SOURCE=.\exp300.bmp
# End Source File
# Begin Source File

SOURCE=.\exp301.bmp
# End Source File
# Begin Source File

SOURCE=.\exp302.bmp
# End Source File
# Begin Source File

SOURCE=.\exp303.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP400.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP401.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP402.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP403.bmp
# End Source File
# Begin Source File

SOURCE=.\exp500.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP501.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP502.bmp
# End Source File
# Begin Source File

SOURCE=.\EXP503.bmp
# End Source File
# Begin Source File

SOURCE=.\exp601.bmp
# End Source File
# Begin Source File

SOURCE=.\exp602.bmp
# End Source File
# Begin Source File

SOURCE=.\exp603.bmp
# End Source File
# Begin Source File

SOURCE=.\gry_off.bmp
# End Source File
# Begin Source File

SOURCE=.\gry_std.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliBOMB.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliJACK.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliLSR.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliMGN.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliMINE.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliMISL.bmp
# End Source File
# Begin Source File

SOURCE=.\hiliSCRAP.bmp
# End Source File
# Begin Source File

SOURCE=.\hudammo.bmp
# End Source File
# Begin Source File

SOURCE=.\hudbar_s.bmp
# End Source File
# Begin Source File

SOURCE=.\hudbar_w.bmp
# End Source File
# Begin Source File

SOURCE=.\hudblank.bmp
# End Source File
# Begin Source File

SOURCE=.\hudenr_s.bmp
# End Source File
# Begin Source File

SOURCE=.\hudenr_w.bmp
# End Source File
# Begin Source File

SOURCE=.\hudentry.bmp
# End Source File
# Begin Source File

SOURCE=.\hudexit.bmp
# End Source File
# Begin Source File

SOURCE=.\hudmine.bmp
# End Source File
# Begin Source File

SOURCE=.\hudrock.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0000.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0001.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0002.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0003.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0004.bmp
# End Source File
# Begin Source File

SOURCE=.\jack0005.bmp
# End Source File
# Begin Source File

SOURCE=.\LBURST0.bmp
# End Source File
# Begin Source File

SOURCE=.\LBURST1.bmp
# End Source File
# Begin Source File

SOURCE=.\LBURST2.bmp
# End Source File
# Begin Source File

SOURCE=.\leroi0.bmp
# End Source File
# Begin Source File

SOURCE=.\leroi1.bmp
# End Source File
# Begin Source File

SOURCE=.\leroi2.bmp
# End Source File
# Begin Source File

SOURCE=.\leroi3.bmp
# End Source File
# Begin Source File

SOURCE=.\leroi4.bmp
# End Source File
# Begin Source File

SOURCE=.\nodigit.bmp
# End Source File
# Begin Source File

SOURCE=.\Noin.bmp
# End Source File
# Begin Source File

SOURCE=.\Noout.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_1.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_2.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_3.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_4.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_5.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_6.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_7.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_8.bmp
# End Source File
# Begin Source File

SOURCE=.\opp1_9.bmp
# End Source File
# Begin Source File

SOURCE=.\oppblank.bmp
# End Source File
# Begin Source File

SOURCE=.\optionin.bmp
# End Source File
# Begin Source File

SOURCE=.\optionot.bmp
# End Source File
# Begin Source File

SOURCE=.\quitin.bmp
# End Source File
# Begin Source File

SOURCE=.\quitout.bmp
# End Source File
# Begin Source File

SOURCE=.\r0034.bmp
# End Source File
# Begin Source File

SOURCE=.\r0035.bmp
# End Source File
# Begin Source File

SOURCE=.\r0036.bmp
# End Source File
# Begin Source File

SOURCE=.\r0037.bmp
# End Source File
# Begin Source File

SOURCE=.\r0038.bmp
# End Source File
# Begin Source File

SOURCE=.\r0039.bmp
# End Source File
# Begin Source File

SOURCE=.\r_bombs.bmp
# End Source File
# Begin Source File

SOURCE=.\r_lasers.bmp
# End Source File
# Begin Source File

SOURCE=.\r_mines.bmp
# End Source File
# Begin Source File

SOURCE=.\r_missle.bmp
# End Source File
# Begin Source File

SOURCE=.\r_rounds.bmp
# End Source File
# Begin Source File

SOURCE=.\rdr_hex0.bmp
# End Source File
# Begin Source File

SOURCE=.\rdr_unj0.bmp
# End Source File
# Begin Source File

SOURCE=.\red_auto.bmp
# End Source File
# Begin Source File

SOURCE=.\red_on.bmp
# End Source File
# Begin Source File

SOURCE=.\resumein.bmp
# End Source File
# Begin Source File

SOURCE=.\resumeot.bmp
# End Source File
# Begin Source File

SOURCE=.\Rtrnin.bmp
# End Source File
# Begin Source File

SOURCE=.\Rtrnout.bmp
# End Source File
# Begin Source File

SOURCE=.\Savein.bmp
# End Source File
# Begin Source File

SOURCE=.\Saveout.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp01.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp02.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp03.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp04.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp05.bmp
# End Source File
# Begin Source File

SOURCE=.\shexp06.bmp
# End Source File
# Begin Source File

SOURCE=.\skull1.bmp
# End Source File
# Begin Source File

SOURCE=.\skull10.bmp
# End Source File
# Begin Source File

SOURCE=.\skull100.bmp
# End Source File
# Begin Source File

SOURCE=.\spine00.bmp
# End Source File
# Begin Source File

SOURCE=.\spine01.bmp
# End Source File
# Begin Source File

SOURCE=.\spine02.bmp
# End Source File
# Begin Source File

SOURCE=.\Sprk0001.bmp
# End Source File
# Begin Source File

SOURCE=.\Sprk0002.bmp
# End Source File
# Begin Source File

SOURCE=.\Sprk0003.bmp
# End Source File
# Begin Source File

SOURCE=.\Sprk0004.bmp
# End Source File
# Begin Source File

SOURCE=.\startin.bmp
# End Source File
# Begin Source File

SOURCE=.\startout.bmp
# End Source File
# Begin Source File

SOURCE=.\Thumb.bmp
# End Source File
# Begin Source File

SOURCE=.\volume0.bmp
# End Source File
# Begin Source File

SOURCE=.\volume1.bmp
# End Source File
# Begin Source File

SOURCE=.\volume2.bmp
# End Source File
# Begin Source File

SOURCE=.\volume3.bmp
# End Source File
# Begin Source File

SOURCE=.\volume4.bmp
# End Source File
# Begin Source File

SOURCE=.\Yesin.bmp
# End Source File
# Begin Source File

SOURCE=.\Yesout.bmp
# End Source File
# Begin Source File

SOURCE=.\Zoomin.bmp
# End Source File
# Begin Source File

SOURCE=.\Zoomout.bmp
# End Source File
# End Group
# End Group
# Begin Group "Library Files"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=..\Gameguts\dsound.lib
# End Source File
# Begin Source File

SOURCE=..\Gameguts\dxguid.lib
# End Source File
# Begin Source File

SOURCE=..\Gameguts\StrmBase.lib
# End Source File
# Begin Source File

SOURCE=..\Gameguts\vfw32.lib
# End Source File
# Begin Source File

SOURCE=..\Gameguts\wavmix32.lib
# End Source File
# Begin Source File

SOURCE=..\Gameguts\wing32.lib
# End Source File
# End Group
# End Target
# End Project
