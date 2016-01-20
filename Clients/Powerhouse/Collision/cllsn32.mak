# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=cllsn32 - Win32 Release
!MESSAGE No configuration specified.  Defaulting to cllsn32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "cllsn32 - Win32 Release" && "$(CFG)" !=\
 "cllsn32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cllsn32.mak" CFG="cllsn32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cllsn32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cllsn32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "cllsn32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\rel32"
# PROP Intermediate_Dir ".\rel32"
OUTDIR=.\rel32
INTDIR=.\rel32

ALL : ".\Collision.exe" "$(OUTDIR)\Collision.bsc"

CLEAN : 
	-@erase "$(INTDIR)\AmbientSnd.obj"
	-@erase "$(INTDIR)\AmbientSnd.sbr"
	-@erase "$(INTDIR)\amovie.obj"
	-@erase "$(INTDIR)\amovie.sbr"
	-@erase "$(INTDIR)\author.obj"
	-@erase "$(INTDIR)\author.sbr"
	-@erase "$(INTDIR)\Bcontrol.obj"
	-@erase "$(INTDIR)\Bcontrol.sbr"
	-@erase "$(INTDIR)\CarShots.obj"
	-@erase "$(INTDIR)\CarShots.sbr"
	-@erase "$(INTDIR)\Cells.obj"
	-@erase "$(INTDIR)\Cells.sbr"
	-@erase "$(INTDIR)\cllsn.obj"
	-@erase "$(INTDIR)\cllsn.sbr"
	-@erase "$(INTDIR)\Collect.obj"
	-@erase "$(INTDIR)\Collect.sbr"
	-@erase "$(INTDIR)\Collision.res"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\credits.sbr"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\Dialog.sbr"
	-@erase "$(INTDIR)\dib.obj"
	-@erase "$(INTDIR)\dib.sbr"
	-@erase "$(INTDIR)\dirsnd.obj"
	-@erase "$(INTDIR)\dirsnd.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\GameState.obj"
	-@erase "$(INTDIR)\GameState.sbr"
	-@erase "$(INTDIR)\hcontrol.obj"
	-@erase "$(INTDIR)\hcontrol.sbr"
	-@erase "$(INTDIR)\Hslider.obj"
	-@erase "$(INTDIR)\Hslider.sbr"
	-@erase "$(INTDIR)\httables.obj"
	-@erase "$(INTDIR)\httables.sbr"
	-@erase "$(INTDIR)\Levels.obj"
	-@erase "$(INTDIR)\Levels.sbr"
	-@erase "$(INTDIR)\line.obj"
	-@erase "$(INTDIR)\line.sbr"
	-@erase "$(INTDIR)\mci.obj"
	-@erase "$(INTDIR)\mci.sbr"
	-@erase "$(INTDIR)\Mcontrol.obj"
	-@erase "$(INTDIR)\Mcontrol.sbr"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\Memory.sbr"
	-@erase "$(INTDIR)\mmiojunk.obj"
	-@erase "$(INTDIR)\mmiojunk.sbr"
	-@erase "$(INTDIR)\offscren.obj"
	-@erase "$(INTDIR)\offscren.sbr"
	-@erase "$(INTDIR)\Opponents.obj"
	-@erase "$(INTDIR)\Opponents.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\pal.obj"
	-@erase "$(INTDIR)\pal.sbr"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\parser.sbr"
	-@erase "$(INTDIR)\phapp.obj"
	-@erase "$(INTDIR)\phapp.sbr"
	-@erase "$(INTDIR)\randseq.obj"
	-@erase "$(INTDIR)\randseq.sbr"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Rcontrol.sbr"
	-@erase "$(INTDIR)\scene.obj"
	-@erase "$(INTDIR)\scene.sbr"
	-@erase "$(INTDIR)\Score.obj"
	-@erase "$(INTDIR)\Score.sbr"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\script.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\speedchk.obj"
	-@erase "$(INTDIR)\speedchk.sbr"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\sprite.sbr"
	-@erase "$(INTDIR)\transit.obj"
	-@erase "$(INTDIR)\transit.sbr"
	-@erase "$(INTDIR)\vcontrol.obj"
	-@erase "$(INTDIR)\vcontrol.sbr"
	-@erase "$(INTDIR)\Vid3d.obj"
	-@erase "$(INTDIR)\Vid3d.sbr"
	-@erase "$(INTDIR)\Video.obj"
	-@erase "$(INTDIR)\Video.sbr"
	-@erase "$(INTDIR)\videodrw.obj"
	-@erase "$(INTDIR)\videodrw.sbr"
	-@erase "$(INTDIR)\videofx.obj"
	-@erase "$(INTDIR)\videofx.sbr"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(INTDIR)\vtable.sbr"
	-@erase "$(INTDIR)\vworld.obj"
	-@erase "$(INTDIR)\vworld.sbr"
	-@erase "$(INTDIR)\wdebug.obj"
	-@erase "$(INTDIR)\wdebug.sbr"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\weapon.sbr"
	-@erase "$(INTDIR)\wintools.obj"
	-@erase "$(INTDIR)\wintools.sbr"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase "$(INTDIR)\Worlds.sbr"
	-@erase "$(OUTDIR)\Collision.bsc"
	-@erase ".\Collision.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Ox /Ot /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /G5 /ML /W3 /GX /Ox /Ot /Ob2 /Gy /I "..\gameguts" /D "WIN32"\
 /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr"$(INTDIR)/"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\.\rel32/
CPP_SBRS=.\.\rel32/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "NDEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo".\rel32/Collision.res" /i "..\gameguts" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Collision.res" /i "..\gameguts" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\rel32/Collision.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Collision.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AmbientSnd.sbr" \
	"$(INTDIR)\amovie.sbr" \
	"$(INTDIR)\author.sbr" \
	"$(INTDIR)\Bcontrol.sbr" \
	"$(INTDIR)\CarShots.sbr" \
	"$(INTDIR)\Cells.sbr" \
	"$(INTDIR)\cllsn.sbr" \
	"$(INTDIR)\Collect.sbr" \
	"$(INTDIR)\credits.sbr" \
	"$(INTDIR)\Dialog.sbr" \
	"$(INTDIR)\dib.sbr" \
	"$(INTDIR)\dirsnd.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\GameState.sbr" \
	"$(INTDIR)\hcontrol.sbr" \
	"$(INTDIR)\Hslider.sbr" \
	"$(INTDIR)\httables.sbr" \
	"$(INTDIR)\Levels.sbr" \
	"$(INTDIR)\line.sbr" \
	"$(INTDIR)\mci.sbr" \
	"$(INTDIR)\Mcontrol.sbr" \
	"$(INTDIR)\Memory.sbr" \
	"$(INTDIR)\mmiojunk.sbr" \
	"$(INTDIR)\offscren.sbr" \
	"$(INTDIR)\Opponents.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\pal.sbr" \
	"$(INTDIR)\parser.sbr" \
	"$(INTDIR)\phapp.sbr" \
	"$(INTDIR)\randseq.sbr" \
	"$(INTDIR)\Rcontrol.sbr" \
	"$(INTDIR)\scene.sbr" \
	"$(INTDIR)\Score.sbr" \
	"$(INTDIR)\script.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\speedchk.sbr" \
	"$(INTDIR)\sprite.sbr" \
	"$(INTDIR)\transit.sbr" \
	"$(INTDIR)\vcontrol.sbr" \
	"$(INTDIR)\Vid3d.sbr" \
	"$(INTDIR)\Video.sbr" \
	"$(INTDIR)\videodrw.sbr" \
	"$(INTDIR)\videofx.sbr" \
	"$(INTDIR)\vtable.sbr" \
	"$(INTDIR)\vworld.sbr" \
	"$(INTDIR)\wdebug.sbr" \
	"$(INTDIR)\weapon.sbr" \
	"$(INTDIR)\wintools.sbr" \
	"$(INTDIR)\Worlds.sbr"

"$(OUTDIR)\Collision.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe"
# SUBTRACT LINK32 /pdb:none /incremental:yes /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Collision.pdb" /machine:IX86\
 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AmbientSnd.obj" \
	"$(INTDIR)\amovie.obj" \
	"$(INTDIR)\author.obj" \
	"$(INTDIR)\Bcontrol.obj" \
	"$(INTDIR)\CarShots.obj" \
	"$(INTDIR)\Cells.obj" \
	"$(INTDIR)\cllsn.obj" \
	"$(INTDIR)\Collect.obj" \
	"$(INTDIR)\Collision.res" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\dib.obj" \
	"$(INTDIR)\dirsnd.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\GameState.obj" \
	"$(INTDIR)\hcontrol.obj" \
	"$(INTDIR)\Hslider.obj" \
	"$(INTDIR)\httables.obj" \
	"$(INTDIR)\Levels.obj" \
	"$(INTDIR)\line.obj" \
	"$(INTDIR)\mci.obj" \
	"$(INTDIR)\Mcontrol.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\mmiojunk.obj" \
	"$(INTDIR)\offscren.obj" \
	"$(INTDIR)\Opponents.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\pal.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\phapp.obj" \
	"$(INTDIR)\randseq.obj" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\scene.obj" \
	"$(INTDIR)\Score.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\speedchk.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\transit.obj" \
	"$(INTDIR)\vcontrol.obj" \
	"$(INTDIR)\Vid3d.obj" \
	"$(INTDIR)\Video.obj" \
	"$(INTDIR)\videodrw.obj" \
	"$(INTDIR)\videofx.obj" \
	"$(INTDIR)\vtable.obj" \
	"$(INTDIR)\vworld.obj" \
	"$(INTDIR)\wdebug.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\wintools.obj" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\dsound.lib" \
	"..\Gameguts\dxguid.lib" \
	"..\Gameguts\StrmBase.lib" \
	"..\Gameguts\vfw32.lib" \
	"..\Gameguts\wavmix32.lib" \
	"..\Gameguts\wing32.lib"

".\Collision.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\dbg32"
# PROP Intermediate_Dir ".\dbg32"
OUTDIR=.\dbg32
INTDIR=.\dbg32

ALL : ".\Collision.exe" "$(OUTDIR)\Collision.bsc"

CLEAN : 
	-@erase "$(INTDIR)\AmbientSnd.obj"
	-@erase "$(INTDIR)\AmbientSnd.sbr"
	-@erase "$(INTDIR)\amovie.obj"
	-@erase "$(INTDIR)\amovie.sbr"
	-@erase "$(INTDIR)\author.obj"
	-@erase "$(INTDIR)\author.sbr"
	-@erase "$(INTDIR)\Bcontrol.obj"
	-@erase "$(INTDIR)\Bcontrol.sbr"
	-@erase "$(INTDIR)\CarShots.obj"
	-@erase "$(INTDIR)\CarShots.sbr"
	-@erase "$(INTDIR)\Cells.obj"
	-@erase "$(INTDIR)\Cells.sbr"
	-@erase "$(INTDIR)\cllsn.obj"
	-@erase "$(INTDIR)\cllsn.sbr"
	-@erase "$(INTDIR)\Collect.obj"
	-@erase "$(INTDIR)\Collect.sbr"
	-@erase "$(INTDIR)\Collision.res"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\credits.sbr"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\Dialog.sbr"
	-@erase "$(INTDIR)\dib.obj"
	-@erase "$(INTDIR)\dib.sbr"
	-@erase "$(INTDIR)\dirsnd.obj"
	-@erase "$(INTDIR)\dirsnd.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\GameState.obj"
	-@erase "$(INTDIR)\GameState.sbr"
	-@erase "$(INTDIR)\hcontrol.obj"
	-@erase "$(INTDIR)\hcontrol.sbr"
	-@erase "$(INTDIR)\Hslider.obj"
	-@erase "$(INTDIR)\Hslider.sbr"
	-@erase "$(INTDIR)\httables.obj"
	-@erase "$(INTDIR)\httables.sbr"
	-@erase "$(INTDIR)\Levels.obj"
	-@erase "$(INTDIR)\Levels.sbr"
	-@erase "$(INTDIR)\line.obj"
	-@erase "$(INTDIR)\line.sbr"
	-@erase "$(INTDIR)\mci.obj"
	-@erase "$(INTDIR)\mci.sbr"
	-@erase "$(INTDIR)\Mcontrol.obj"
	-@erase "$(INTDIR)\Mcontrol.sbr"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\Memory.sbr"
	-@erase "$(INTDIR)\mmiojunk.obj"
	-@erase "$(INTDIR)\mmiojunk.sbr"
	-@erase "$(INTDIR)\offscren.obj"
	-@erase "$(INTDIR)\offscren.sbr"
	-@erase "$(INTDIR)\Opponents.obj"
	-@erase "$(INTDIR)\Opponents.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\pal.obj"
	-@erase "$(INTDIR)\pal.sbr"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\parser.sbr"
	-@erase "$(INTDIR)\phapp.obj"
	-@erase "$(INTDIR)\phapp.sbr"
	-@erase "$(INTDIR)\randseq.obj"
	-@erase "$(INTDIR)\randseq.sbr"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Rcontrol.sbr"
	-@erase "$(INTDIR)\scene.obj"
	-@erase "$(INTDIR)\scene.sbr"
	-@erase "$(INTDIR)\Score.obj"
	-@erase "$(INTDIR)\Score.sbr"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\script.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\speedchk.obj"
	-@erase "$(INTDIR)\speedchk.sbr"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\sprite.sbr"
	-@erase "$(INTDIR)\transit.obj"
	-@erase "$(INTDIR)\transit.sbr"
	-@erase "$(INTDIR)\vcontrol.obj"
	-@erase "$(INTDIR)\vcontrol.sbr"
	-@erase "$(INTDIR)\Vid3d.obj"
	-@erase "$(INTDIR)\Vid3d.sbr"
	-@erase "$(INTDIR)\Video.obj"
	-@erase "$(INTDIR)\Video.sbr"
	-@erase "$(INTDIR)\videodrw.obj"
	-@erase "$(INTDIR)\videodrw.sbr"
	-@erase "$(INTDIR)\videofx.obj"
	-@erase "$(INTDIR)\videofx.sbr"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(INTDIR)\vtable.sbr"
	-@erase "$(INTDIR)\vworld.obj"
	-@erase "$(INTDIR)\vworld.sbr"
	-@erase "$(INTDIR)\wdebug.obj"
	-@erase "$(INTDIR)\wdebug.sbr"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\weapon.sbr"
	-@erase "$(INTDIR)\wintools.obj"
	-@erase "$(INTDIR)\wintools.sbr"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase "$(INTDIR)\Worlds.sbr"
	-@erase "$(OUTDIR)\Collision.bsc"
	-@erase "$(OUTDIR)\Collision.pdb"
	-@erase ".\Collision.exe"
	-@erase ".\DEBUG32.PDB"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Zi /Od /Ob1 /Gy /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /Fd"DEBUG.PDB" /c
# ADD CPP /nologo /G5 /W3 /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FAcs /Fr /Fd"DEBUG32.PDB" /c
CPP_PROJ=/nologo /G5 /MLd /W3 /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FAcs /Fa"$(INTDIR)/"\
 /Fr"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"DEBUG32.PDB" /c 
CPP_OBJS=.\.\dbg32/
CPP_SBRS=.\.\dbg32/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "_DEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo".\dbg32/Collision.res" /i "..\gameguts" /d "_DEBUG"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Collision.res" /i "..\gameguts" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\dbg32/Collision.bsc"
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Collision.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AmbientSnd.sbr" \
	"$(INTDIR)\amovie.sbr" \
	"$(INTDIR)\author.sbr" \
	"$(INTDIR)\Bcontrol.sbr" \
	"$(INTDIR)\CarShots.sbr" \
	"$(INTDIR)\Cells.sbr" \
	"$(INTDIR)\cllsn.sbr" \
	"$(INTDIR)\Collect.sbr" \
	"$(INTDIR)\credits.sbr" \
	"$(INTDIR)\Dialog.sbr" \
	"$(INTDIR)\dib.sbr" \
	"$(INTDIR)\dirsnd.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\GameState.sbr" \
	"$(INTDIR)\hcontrol.sbr" \
	"$(INTDIR)\Hslider.sbr" \
	"$(INTDIR)\httables.sbr" \
	"$(INTDIR)\Levels.sbr" \
	"$(INTDIR)\line.sbr" \
	"$(INTDIR)\mci.sbr" \
	"$(INTDIR)\Mcontrol.sbr" \
	"$(INTDIR)\Memory.sbr" \
	"$(INTDIR)\mmiojunk.sbr" \
	"$(INTDIR)\offscren.sbr" \
	"$(INTDIR)\Opponents.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\pal.sbr" \
	"$(INTDIR)\parser.sbr" \
	"$(INTDIR)\phapp.sbr" \
	"$(INTDIR)\randseq.sbr" \
	"$(INTDIR)\Rcontrol.sbr" \
	"$(INTDIR)\scene.sbr" \
	"$(INTDIR)\Score.sbr" \
	"$(INTDIR)\script.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\speedchk.sbr" \
	"$(INTDIR)\sprite.sbr" \
	"$(INTDIR)\transit.sbr" \
	"$(INTDIR)\vcontrol.sbr" \
	"$(INTDIR)\Vid3d.sbr" \
	"$(INTDIR)\Video.sbr" \
	"$(INTDIR)\videodrw.sbr" \
	"$(INTDIR)\videofx.sbr" \
	"$(INTDIR)\vtable.sbr" \
	"$(INTDIR)\vworld.sbr" \
	"$(INTDIR)\wdebug.sbr" \
	"$(INTDIR)\weapon.sbr" \
	"$(INTDIR)\wintools.sbr" \
	"$(INTDIR)\Worlds.sbr"

"$(OUTDIR)\Collision.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows /incremental:no /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe"
# SUBTRACT LINK32 /profile /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Collision.pdb" /debug /machine:IX86\
 /nodefaultlib:"msvcrt.lib" /out:".\Collision.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AmbientSnd.obj" \
	"$(INTDIR)\amovie.obj" \
	"$(INTDIR)\author.obj" \
	"$(INTDIR)\Bcontrol.obj" \
	"$(INTDIR)\CarShots.obj" \
	"$(INTDIR)\Cells.obj" \
	"$(INTDIR)\cllsn.obj" \
	"$(INTDIR)\Collect.obj" \
	"$(INTDIR)\Collision.res" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\dib.obj" \
	"$(INTDIR)\dirsnd.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\GameState.obj" \
	"$(INTDIR)\hcontrol.obj" \
	"$(INTDIR)\Hslider.obj" \
	"$(INTDIR)\httables.obj" \
	"$(INTDIR)\Levels.obj" \
	"$(INTDIR)\line.obj" \
	"$(INTDIR)\mci.obj" \
	"$(INTDIR)\Mcontrol.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\mmiojunk.obj" \
	"$(INTDIR)\offscren.obj" \
	"$(INTDIR)\Opponents.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\pal.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\phapp.obj" \
	"$(INTDIR)\randseq.obj" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\scene.obj" \
	"$(INTDIR)\Score.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\speedchk.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\transit.obj" \
	"$(INTDIR)\vcontrol.obj" \
	"$(INTDIR)\Vid3d.obj" \
	"$(INTDIR)\Video.obj" \
	"$(INTDIR)\videodrw.obj" \
	"$(INTDIR)\videofx.obj" \
	"$(INTDIR)\vtable.obj" \
	"$(INTDIR)\vworld.obj" \
	"$(INTDIR)\wdebug.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\wintools.obj" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\dsound.lib" \
	"..\Gameguts\dxguid.lib" \
	"..\Gameguts\StrmBase.lib" \
	"..\Gameguts\vfw32.lib" \
	"..\Gameguts\wavmix32.lib" \
	"..\Gameguts\wing32.lib"

".\Collision.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "cllsn32 - Win32 Release"
# Name "cllsn32 - Win32 Debug"

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\options.cpp
DEP_CPP_OPTIO=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\options.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\weapon.cpp
DEP_CPP_WEAPO=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\line.p"\
	".\mmiojunk.p"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\wdebug.p"\
	".\Worlds.h"\
	

"$(INTDIR)\weapon.obj" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"

"$(INTDIR)\weapon.sbr" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsn.CPP
DEP_CPP_CLLSN=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\credits.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\author.p"\
	".\cells.h"\
	".\cllsn.H"\
	".\Cllsndef.h"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\mmiojunk.p"\
	".\Opponents.h"\
	".\options.h"\
	".\rcontrol.p"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\wdebug.p"\
	".\Worlds.h"\
	

"$(INTDIR)\cllsn.obj" : $(SOURCE) $(DEP_CPP_CLLSN) "$(INTDIR)"

"$(INTDIR)\cllsn.sbr" : $(SOURCE) $(DEP_CPP_CLLSN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Rcontrol.cpp
DEP_CPP_RCONT=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\rcontrol.p"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Rcontrol.obj" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"

"$(INTDIR)\Rcontrol.sbr" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Worlds.cpp

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

DEP_CPP_WORLD=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\strmif.h"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PORT.H"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Worlds.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"

"$(INTDIR)\Worlds.sbr" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

DEP_CPP_WORLD=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Worlds.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"

"$(INTDIR)\Worlds.sbr" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsn.H

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\vworld.cpp
DEP_CPP_VWORL=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\cells.h"\
	".\Data3d.h"\
	".\Opponents.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\vworld.obj" : $(SOURCE) $(DEP_CPP_VWORL) "$(INTDIR)"

"$(INTDIR)\vworld.sbr" : $(SOURCE) $(DEP_CPP_VWORL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GameState.cpp
DEP_CPP_GAMES=\
	".\GameState.h"\
	

"$(INTDIR)\GameState.obj" : $(SOURCE) $(DEP_CPP_GAMES) "$(INTDIR)"

"$(INTDIR)\GameState.sbr" : $(SOURCE) $(DEP_CPP_GAMES) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Levels.cpp
DEP_CPP_LEVEL=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Cllsndef.h"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Levels.obj" : $(SOURCE) $(DEP_CPP_LEVEL) "$(INTDIR)"

"$(INTDIR)\Levels.sbr" : $(SOURCE) $(DEP_CPP_LEVEL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AmbientSnd.cpp
DEP_CPP_AMBIE=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	

"$(INTDIR)\AmbientSnd.obj" : $(SOURCE) $(DEP_CPP_AMBIE) "$(INTDIR)"

"$(INTDIR)\AmbientSnd.sbr" : $(SOURCE) $(DEP_CPP_AMBIE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Score.cpp
DEP_CPP_SCORE=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\Score.h"\
	

"$(INTDIR)\Score.obj" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"

"$(INTDIR)\Score.sbr" : $(SOURCE) $(DEP_CPP_SCORE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\author.cpp
DEP_CPP_AUTHO=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\Author.h"\
	".\author.p"\
	".\cells.h"\
	".\cllsn.H"\
	".\Cllsndef.h"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\author.obj" : $(SOURCE) $(DEP_CPP_AUTHO) "$(INTDIR)"

"$(INTDIR)\author.sbr" : $(SOURCE) $(DEP_CPP_AUTHO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Vid3d.cpp
DEP_CPP_VID3D=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\Vid3d.h"\
	

"$(INTDIR)\Vid3d.obj" : $(SOURCE) $(DEP_CPP_VID3D) "$(INTDIR)"

"$(INTDIR)\Vid3d.sbr" : $(SOURCE) $(DEP_CPP_VID3D) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\wdebug.cpp
DEP_CPP_WDEBU=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\author.p"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\line.p"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\wdebug.p"\
	".\Worlds.h"\
	

"$(INTDIR)\wdebug.obj" : $(SOURCE) $(DEP_CPP_WDEBU) "$(INTDIR)"

"$(INTDIR)\wdebug.sbr" : $(SOURCE) $(DEP_CPP_WDEBU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\line.cpp
DEP_CPP_LINE_=\
	".\line.p"\
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"

"$(INTDIR)\line.sbr" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mmiojunk.cpp
DEP_CPP_MMIOJ=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\Data3d.h"\
	".\mmiojunk.p"\
	

"$(INTDIR)\mmiojunk.obj" : $(SOURCE) $(DEP_CPP_MMIOJ) "$(INTDIR)"

"$(INTDIR)\mmiojunk.sbr" : $(SOURCE) $(DEP_CPP_MMIOJ) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Bcontrol.cpp
DEP_CPP_BCONT=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Bcontrol.obj" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Bcontrol.sbr" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\credits.cpp
DEP_CPP_CREDI=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\credits.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\Gameguts\parser.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\credits.obj" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\credits.sbr" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Dialog.cpp
DEP_CPP_DIALO=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Dialog.sbr" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\dib.cpp
DEP_CPP_DIB_C=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\dibfx.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dib.sbr" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\hcontrol.cpp
DEP_CPP_HCONT=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\hcontrol.obj" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hcontrol.sbr" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Hslider.cpp
DEP_CPP_HSLID=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Hslider.obj" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Hslider.sbr" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\httables.cpp
DEP_CPP_HTTAB=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\httables.obj" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httables.sbr" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\mci.cpp
DEP_CPP_MCI_C=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\mci.obj" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mci.sbr" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Mcontrol.cpp
DEP_CPP_MCONT=\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Mcontrol.obj" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Mcontrol.sbr" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Memory.cpp
DEP_CPP_MEMOR=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\offscren.cpp
DEP_CPP_OFFSC=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\offscren.obj" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\offscren.sbr" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\pal.cpp
DEP_CPP_PAL_C=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.sbr" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\parser.cpp
DEP_CPP_PARSE=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\Gameguts\parser.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\parser.sbr" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\phapp.cpp

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

DEP_CPP_PHAPP=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\resource.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\phapp.obj" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\phapp.sbr" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

DEP_CPP_PHAPP=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\resource.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\wing.h"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\phapp.obj" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\phapp.sbr" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\randseq.cpp
DEP_CPP_RANDS=\
	"..\Gameguts\randseq.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\randseq.obj" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\randseq.sbr" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\scene.cpp
DEP_CPP_SCENE=\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\scene.obj" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\scene.sbr" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\script.cpp
DEP_CPP_SCRIP=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\Gameguts\script.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\script.sbr" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\sound.cpp
DEP_CPP_SOUND=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\sound.sbr" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\speedchk.cpp
DEP_CPP_SPEED=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\speedchk.obj" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\speedchk.sbr" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\sprite.cpp
DEP_CPP_SPRIT=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\Gameguts\script.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\sprite.obj" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\sprite.sbr" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\transit.cpp
DEP_CPP_TRANS=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\Gameguts\randseq.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\transit.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\transit.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\transit.sbr" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\Video.cpp
DEP_CPP_VIDEO=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Video.obj" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Video.sbr" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\videodrw.cpp
DEP_CPP_VIDEOD=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\videodrw.obj" : $(SOURCE) $(DEP_CPP_VIDEOD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\videodrw.sbr" : $(SOURCE) $(DEP_CPP_VIDEOD) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\vtable.cpp
DEP_CPP_VTABL=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\Gameguts\parser.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\vtable.obj" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vtable.sbr" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\wintools.cpp
DEP_CPP_WINTO=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\Gameguts\wing.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\wintools.obj" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\wintools.sbr" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\videofx.cpp

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

DEP_CPP_VIDEOF=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\videofx.obj" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"

"$(INTDIR)\videofx.sbr" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

DEP_CPP_VIDEOF=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\videofx.obj" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"

"$(INTDIR)\videofx.sbr" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\wavmix32.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\wing32.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\vfw32.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsn.rc
DEP_RSC_CLLSN_=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\resource.h"\
	".\111mchng.wav"\
	".\112mchng.wav"\
	".\12rcktln.wav"\
	".\13lsrfsn.wav"\
	".\2112pwkx.wav"\
	".\341wndsh.wav"\
	".\41prtlnr.wav"\
	".\71crshth.wav"\
	".\Author.rc"\
	".\BCRACK1.bmp"\
	".\BCRACK2.bmp"\
	".\boss.wav"\
	".\BREAKER1.bmp"\
	".\BREAKER1X.bmp"\
	".\BREAKER2.bmp"\
	".\BREAKER2X.bmp"\
	".\BREAKER3.bmp"\
	".\BREAKER3X.bmp"\
	".\BREAKER4.bmp"\
	".\breaker4x.bmp"\
	".\BREAKER5.bmp"\
	".\BREAKER5X.bmp"\
	".\buttin.bmp"\
	".\buttout.bmp"\
	".\cancelin.bmp"\
	".\cancelot.bmp"\
	".\CARSOUND.wav"\
	".\cllsn.ico"\
	".\Cllsndef.h"\
	".\col01_01.bmp"\
	".\col02_01.bmp"\
	".\col02_02.bmp"\
	".\col04_01.bmp"\
	".\col04_02.bmp"\
	".\col04_03.bmp"\
	".\col04_04.bmp"\
	".\col04_05.bmp"\
	".\col04_06.bmp"\
	".\col06_01.bmp"\
	".\col06_02.bmp"\
	".\col06_03.bmp"\
	".\col06_04.bmp"\
	".\col06_05.bmp"\
	".\col06_06.bmp"\
	".\col06_07.bmp"\
	".\col07_01.bmp"\
	".\col07_02.bmp"\
	".\col07_03.bmp"\
	".\col07_04.bmp"\
	".\col07_05.bmp"\
	".\colblank.bmp"\
	".\creditin.bmp"\
	".\creditot.bmp"\
	".\DIGIT0.bmp"\
	".\digit1.bmp"\
	".\digit2.bmp"\
	".\digit3.bmp"\
	".\digit4.bmp"\
	".\digit5.bmp"\
	".\digit6.bmp"\
	".\DIGIT7.bmp"\
	".\digit8.bmp"\
	".\digit9.bmp"\
	".\Driver11.wav"\
	".\energy1.wav"\
	".\energy_a.bmp"\
	".\energy_b.bmp"\
	".\exitin.bmp"\
	".\exitout.bmp"\
	".\EXP100.bmp"\
	".\EXP101.bmp"\
	".\EXP102.bmp"\
	".\EXP103.bmp"\
	".\EXP200.bmp"\
	".\EXP201.bmp"\
	".\EXP202.bmp"\
	".\EXP203.bmp"\
	".\EXP300.bmp"\
	".\EXP301.bmp"\
	".\EXP302.bmp"\
	".\EXP303.bmp"\
	".\EXP400.bmp"\
	".\EXP401.bmp"\
	".\EXP402.bmp"\
	".\EXP403.bmp"\
	".\EXP500.bmp"\
	".\EXP501.bmp"\
	".\EXP502.bmp"\
	".\EXP503.bmp"\
	".\EXP601.bmp"\
	".\EXP602.bmp"\
	".\EXP603.bmp"\
	".\expsmall.wav"\
	".\goodbye.wav"\
	".\gry_off.bmp"\
	".\gry_std.bmp"\
	".\hiliBOMB.bmp"\
	".\hiliJACK.bmp"\
	".\hililr.bmp"\
	".\hiliLSR.bmp"\
	".\hiliMGN.bmp"\
	".\hiliMINE.bmp"\
	".\hiliMISL.bmp"\
	".\hiliSCRAP.bmp"\
	".\hilithmp.bmp"\
	".\hpoint.cur"\
	".\hudammo.bmp"\
	".\hudbar_s.bmp"\
	".\hudbar_w.bmp"\
	".\hudblank.bmp"\
	".\hudenr_s.bmp"\
	".\hudenr_w.bmp"\
	".\hudentry.bmp"\
	".\hudexit.bmp"\
	".\hudmine.bmp"\
	".\hudrock.bmp"\
	".\jack.wav"\
	".\JACK0000.bmp"\
	".\JACK0001.bmp"\
	".\JACK0002.bmp"\
	".\JACK0003.bmp"\
	".\JACK0004.bmp"\
	".\JACK0005.bmp"\
	".\jackatt.wav"\
	".\LBURST0.bmp"\
	".\LBURST1.bmp"\
	".\LBURST2.bmp"\
	".\LEROI0.bmp"\
	".\LEROI1.bmp"\
	".\LEROI2.bmp"\
	".\LEROI3.bmp"\
	".\LEROI4.bmp"\
	".\lexpbig.wav"\
	".\lexpsmal.wav"\
	".\lrfloat.wav"\
	".\lrgrowl.wav"\
	".\minebeep.wav"\
	".\minelnch.wav"\
	".\no.cur"\
	".\nodigit.bmp"\
	".\noin.bmp"\
	".\noout.bmp"\
	".\opp1_1.bmp"\
	".\opp1_2.bmp"\
	".\opp1_3.bmp"\
	".\opp1_4.bmp"\
	".\opp1_5.bmp"\
	".\opp1_6.bmp"\
	".\opp1_7.bmp"\
	".\opp1_8.bmp"\
	".\opp1_9.bmp"\
	".\oppblank.bmp"\
	".\oppin.wav"\
	".\optionin.bmp"\
	".\optionot.bmp"\
	".\portal.wav"\
	".\quitin.bmp"\
	".\quitout.bmp"\
	".\r_bombs.bmp"\
	".\r_jacks.bmp"\
	".\r_lasers.bmp"\
	".\r_leroi.bmp"\
	".\R_MINES.bmp"\
	".\r_missle.bmp"\
	".\R_ROUNDS.bmp"\
	".\r_scrap.bmp"\
	".\R_THUMP.bmp"\
	".\rdr_10to4.bmp"\
	".\rdr_2to8.bmp"\
	".\rdr_ammo.bmp"\
	".\rdr_barr.bmp"\
	".\rdr_blackhole.bmp"\
	".\rdr_car01.bmp"\
	".\rdr_car02.bmp"\
	".\rdr_car03.bmp"\
	".\rdr_car04.bmp"\
	".\rdr_car05.bmp"\
	".\rdr_car06.bmp"\
	".\rdr_car07.bmp"\
	".\rdr_car08.bmp"\
	".\rdr_car09.bmp"\
	".\rdr_car10.bmp"\
	".\rdr_car11.bmp"\
	".\rdr_car12.bmp"\
	".\rdr_collect.bmp"\
	".\rdr_cul.bmp"\
	".\rdr_empty.bmp"\
	".\rdr_energy.bmp"\
	".\rdr_entry.bmp"\
	".\rdr_exit.bmp"\
	".\rdr_hex0.bmp"\
	".\rdr_oppgen.bmp"\
	".\rdr_radiation.bmp"\
	".\rdr_safe.bmp"\
	".\rdr_switch.bmp"\
	".\rdr_unj0.bmp"\
	".\rdr_vert.bmp"\
	".\red_auto.bmp"\
	".\red_on.bmp"\
	".\resumein.bmp"\
	".\resumeot.bmp"\
	".\rtrnin.bmp"\
	".\rtrnout.bmp"\
	".\savein.bmp"\
	".\saveout.bmp"\
	".\scenes.hxx"\
	".\scrap1.wav"\
	".\scrnrkt.bmp"\
	".\shexp01.bmp"\
	".\shexp02.bmp"\
	".\shexp03.bmp"\
	".\shexp04.bmp"\
	".\skull1.bmp"\
	".\skull10.bmp"\
	".\skull100.bmp"\
	".\SPINE00.bmp"\
	".\SPINE01.bmp"\
	".\SPINE02.bmp"\
	".\spklnch.wav"\
	".\SPRK0001.bmp"\
	".\SPRK0002.bmp"\
	".\SPRK0003.bmp"\
	".\SPRK0004.bmp"\
	".\startin.bmp"\
	".\startout.bmp"\
	".\strong.wav"\
	".\switch.wav"\
	".\swtchof8.wav"\
	".\thmplnch.wav"\
	".\thumb.bmp"\
	".\thumper.bmp"\
	".\thumpexp.wav"\
	".\turn.wav"\
	".\turn180.wav"\
	".\volume0.bmp"\
	".\volume1.bmp"\
	".\volume2.bmp"\
	".\volume3.bmp"\
	".\volume4.bmp"\
	".\wallcrsh.wav"\
	".\weak.wav"\
	".\yesin.bmp"\
	".\yesout.bmp"\
	".\zoomin.bmp"\
	".\zoomout.bmp"\
	

"$(INTDIR)\Collision.res" : $(SOURCE) $(DEP_RSC_CLLSN_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\WDebug.rc
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=.\Author.rc
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=.\scenes.hxx

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cells.cpp
DEP_CPP_CELLS=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\cells.h"\
	".\Opponents.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Cells.obj" : $(SOURCE) $(DEP_CPP_CELLS) "$(INTDIR)"

"$(INTDIR)\Cells.sbr" : $(SOURCE) $(DEP_CPP_CELLS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CarShots.cpp
DEP_CPP_CARSH=\
	".\CarShots.h"\
	".\Opponents.h"\
	

"$(INTDIR)\CarShots.obj" : $(SOURCE) $(DEP_CPP_CARSH) "$(INTDIR)"

"$(INTDIR)\CarShots.sbr" : $(SOURCE) $(DEP_CPP_CARSH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Opponents.cpp
DEP_CPP_OPPON=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\CarShots.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Opponents.obj" : $(SOURCE) $(DEP_CPP_OPPON) "$(INTDIR)"

"$(INTDIR)\Opponents.sbr" : $(SOURCE) $(DEP_CPP_OPPON) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsnid.h

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Collect.cpp
DEP_CPP_COLLE=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Cllsndef.h"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\Collect.obj" : $(SOURCE) $(DEP_CPP_COLLE) "$(INTDIR)"

"$(INTDIR)\Collect.sbr" : $(SOURCE) $(DEP_CPP_COLLE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\StrmBase.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\amovie.cpp
DEP_CPP_AMOVI=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\amovie.obj" : $(SOURCE) $(DEP_CPP_AMOVI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\amovie.sbr" : $(SOURCE) $(DEP_CPP_AMOVI) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\vcontrol.cpp
DEP_CPP_VCONT=\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\vcontrol.obj" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vcontrol.sbr" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\dirsnd.cpp
DEP_CPP_DIRSN=\
	"..\gameguts\amovie.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dirsnd.obj" : $(SOURCE) $(DEP_CPP_DIRSN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dirsnd.sbr" : $(SOURCE) $(DEP_CPP_DIRSN) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\dxguid.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Src\Gameguts\dsound.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\game.cpp
DEP_CPP_GAME_=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\amovie.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\Gameguts\controla.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\DirSnd.h"\
	"..\Gameguts\evcode.h"\
	"..\gameguts\gcontrol.p"\
	"..\gameguts\hslider.p"\
	"..\gameguts\mci.p"\
	"..\gameguts\mcontrol.p"\
	"..\gameguts\memory.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\strmif.h"\
	"..\Gameguts\uuids.h"\
	"..\Gameguts\vcontrol.h"\
	"..\gameguts\vcontrol.p"\
	"..\gameguts\vidobsrv.h"\
	"..\gameguts\vtable.h"\
	"..\gameguts\wintools.p"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOON.H"\
	".\AmbientSnd.h"\
	".\cells.h"\
	".\cllsn.H"\
	".\Cllsndef.h"\
	".\Collect.h"\
	".\Data3d.h"\
	".\game.h"\
	".\GameState.h"\
	".\Levels.h"\
	".\Opponents.h"\
	".\rcontrol.p"\
	".\Score.h"\
	".\videofx.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\game.obj" : $(SOURCE) $(DEP_CPP_GAME_) "$(INTDIR)"

"$(INTDIR)\game.sbr" : $(SOURCE) $(DEP_CPP_GAME_) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
