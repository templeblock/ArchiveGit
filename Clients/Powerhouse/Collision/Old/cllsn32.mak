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

ALL : ".\cllsn32.exe" "$(OUTDIR)\cllsn32.bsc"

CLEAN : 
	-@erase "$(INTDIR)\BCONTROL.OBJ"
	-@erase "$(INTDIR)\BCONTROL.SBR"
	-@erase "$(INTDIR)\cllsn.obj"
	-@erase "$(INTDIR)\cllsn.res"
	-@erase "$(INTDIR)\cllsn.sbr"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\credits.sbr"
	-@erase "$(INTDIR)\DIALOG.OBJ"
	-@erase "$(INTDIR)\DIALOG.SBR"
	-@erase "$(INTDIR)\DIB.OBJ"
	-@erase "$(INTDIR)\DIB.SBR"
	-@erase "$(INTDIR)\GCONTROL.OBJ"
	-@erase "$(INTDIR)\GCONTROL.SBR"
	-@erase "$(INTDIR)\HCONTROL.OBJ"
	-@erase "$(INTDIR)\HCONTROL.SBR"
	-@erase "$(INTDIR)\HSLIDER.OBJ"
	-@erase "$(INTDIR)\HSLIDER.SBR"
	-@erase "$(INTDIR)\HTTABLES.OBJ"
	-@erase "$(INTDIR)\HTTABLES.SBR"
	-@erase "$(INTDIR)\InfPanel.obj"
	-@erase "$(INTDIR)\InfPanel.sbr"
	-@erase "$(INTDIR)\Mci.obj"
	-@erase "$(INTDIR)\Mci.sbr"
	-@erase "$(INTDIR)\MCONTROL.OBJ"
	-@erase "$(INTDIR)\MCONTROL.SBR"
	-@erase "$(INTDIR)\MEMORY.OBJ"
	-@erase "$(INTDIR)\MEMORY.SBR"
	-@erase "$(INTDIR)\MIDINOTE.OBJ"
	-@erase "$(INTDIR)\MIDINOTE.SBR"
	-@erase "$(INTDIR)\OFFSCREN.OBJ"
	-@erase "$(INTDIR)\OFFSCREN.SBR"
	-@erase "$(INTDIR)\Opponent.obj"
	-@erase "$(INTDIR)\Opponent.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\PAL.OBJ"
	-@erase "$(INTDIR)\PAL.SBR"
	-@erase "$(INTDIR)\PARSER.OBJ"
	-@erase "$(INTDIR)\PARSER.SBR"
	-@erase "$(INTDIR)\PHAPP.OBJ"
	-@erase "$(INTDIR)\PHAPP.SBR"
	-@erase "$(INTDIR)\RANDSEQ.OBJ"
	-@erase "$(INTDIR)\RANDSEQ.SBR"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Rcontrol.sbr"
	-@erase "$(INTDIR)\SCENE.OBJ"
	-@erase "$(INTDIR)\SCENE.SBR"
	-@erase "$(INTDIR)\SCRIPT.OBJ"
	-@erase "$(INTDIR)\SCRIPT.SBR"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\SOUND.SBR"
	-@erase "$(INTDIR)\SPEEDCHK.OBJ"
	-@erase "$(INTDIR)\SPEEDCHK.SBR"
	-@erase "$(INTDIR)\SPRITE.OBJ"
	-@erase "$(INTDIR)\SPRITE.SBR"
	-@erase "$(INTDIR)\TRANSIT.OBJ"
	-@erase "$(INTDIR)\TRANSIT.SBR"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\UTILS.SBR"
	-@erase "$(INTDIR)\VCONTROL.OBJ"
	-@erase "$(INTDIR)\VCONTROL.SBR"
	-@erase "$(INTDIR)\VIDEO.OBJ"
	-@erase "$(INTDIR)\VIDEO.SBR"
	-@erase "$(INTDIR)\videodrw.obj"
	-@erase "$(INTDIR)\videodrw.sbr"
	-@erase "$(INTDIR)\videofx.obj"
	-@erase "$(INTDIR)\videofx.sbr"
	-@erase "$(INTDIR)\vsndmix.obj"
	-@erase "$(INTDIR)\vsndmix.sbr"
	-@erase "$(INTDIR)\VTABLE.OBJ"
	-@erase "$(INTDIR)\VTABLE.SBR"
	-@erase "$(INTDIR)\vworld.obj"
	-@erase "$(INTDIR)\vworld.sbr"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\weapon.sbr"
	-@erase "$(INTDIR)\WINTOOLS.OBJ"
	-@erase "$(INTDIR)\WINTOOLS.SBR"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase "$(INTDIR)\Worlds.sbr"
	-@erase "$(OUTDIR)\cllsn32.bsc"
	-@erase ".\cllsn32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Ox /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /G5 /ML /W3 /GX /Ox /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr"$(INTDIR)/" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\.\rel32/
CPP_SBRS=.\.\rel32/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "NDEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\gameguts" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cllsn.res" /i "..\gameguts" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cllsn32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BCONTROL.SBR" \
	"$(INTDIR)\cllsn.sbr" \
	"$(INTDIR)\credits.sbr" \
	"$(INTDIR)\DIALOG.SBR" \
	"$(INTDIR)\DIB.SBR" \
	"$(INTDIR)\GCONTROL.SBR" \
	"$(INTDIR)\HCONTROL.SBR" \
	"$(INTDIR)\HSLIDER.SBR" \
	"$(INTDIR)\HTTABLES.SBR" \
	"$(INTDIR)\InfPanel.sbr" \
	"$(INTDIR)\Mci.sbr" \
	"$(INTDIR)\MCONTROL.SBR" \
	"$(INTDIR)\MEMORY.SBR" \
	"$(INTDIR)\MIDINOTE.SBR" \
	"$(INTDIR)\OFFSCREN.SBR" \
	"$(INTDIR)\Opponent.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\PAL.SBR" \
	"$(INTDIR)\PARSER.SBR" \
	"$(INTDIR)\PHAPP.SBR" \
	"$(INTDIR)\RANDSEQ.SBR" \
	"$(INTDIR)\Rcontrol.sbr" \
	"$(INTDIR)\SCENE.SBR" \
	"$(INTDIR)\SCRIPT.SBR" \
	"$(INTDIR)\SOUND.SBR" \
	"$(INTDIR)\SPEEDCHK.SBR" \
	"$(INTDIR)\SPRITE.SBR" \
	"$(INTDIR)\TRANSIT.SBR" \
	"$(INTDIR)\UTILS.SBR" \
	"$(INTDIR)\VCONTROL.SBR" \
	"$(INTDIR)\VIDEO.SBR" \
	"$(INTDIR)\videodrw.sbr" \
	"$(INTDIR)\videofx.sbr" \
	"$(INTDIR)\vsndmix.sbr" \
	"$(INTDIR)\VTABLE.SBR" \
	"$(INTDIR)\vworld.sbr" \
	"$(INTDIR)\weapon.sbr" \
	"$(INTDIR)\WINTOOLS.SBR" \
	"$(INTDIR)\Worlds.sbr"

"$(OUTDIR)\cllsn32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86 /out:".\cllsn32.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/cllsn32.pdb" /machine:IX86 /out:".\cllsn32.exe"\
 
LINK32_OBJS= \
	"$(INTDIR)\BCONTROL.OBJ" \
	"$(INTDIR)\cllsn.obj" \
	"$(INTDIR)\cllsn.res" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\DIALOG.OBJ" \
	"$(INTDIR)\DIB.OBJ" \
	"$(INTDIR)\GCONTROL.OBJ" \
	"$(INTDIR)\HCONTROL.OBJ" \
	"$(INTDIR)\HSLIDER.OBJ" \
	"$(INTDIR)\HTTABLES.OBJ" \
	"$(INTDIR)\InfPanel.obj" \
	"$(INTDIR)\Mci.obj" \
	"$(INTDIR)\MCONTROL.OBJ" \
	"$(INTDIR)\MEMORY.OBJ" \
	"$(INTDIR)\MIDINOTE.OBJ" \
	"$(INTDIR)\OFFSCREN.OBJ" \
	"$(INTDIR)\Opponent.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\PAL.OBJ" \
	"$(INTDIR)\PARSER.OBJ" \
	"$(INTDIR)\PHAPP.OBJ" \
	"$(INTDIR)\RANDSEQ.OBJ" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\SCENE.OBJ" \
	"$(INTDIR)\SCRIPT.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\SPEEDCHK.OBJ" \
	"$(INTDIR)\SPRITE.OBJ" \
	"$(INTDIR)\TRANSIT.OBJ" \
	"$(INTDIR)\UTILS.OBJ" \
	"$(INTDIR)\VCONTROL.OBJ" \
	"$(INTDIR)\VIDEO.OBJ" \
	"$(INTDIR)\videodrw.obj" \
	"$(INTDIR)\videofx.obj" \
	"$(INTDIR)\vsndmix.obj" \
	"$(INTDIR)\VTABLE.OBJ" \
	"$(INTDIR)\vworld.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\WINTOOLS.OBJ" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\VFW32.LIB" \
	"..\Gameguts\Wavmix32.lib" \
	"..\Gameguts\WING32.LIB"

".\cllsn32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : ".\cllsn32.exe" "$(OUTDIR)\cllsn32.bsc"

CLEAN : 
	-@erase "$(INTDIR)\BCONTROL.OBJ"
	-@erase "$(INTDIR)\BCONTROL.SBR"
	-@erase "$(INTDIR)\cllsn.obj"
	-@erase "$(INTDIR)\cllsn.res"
	-@erase "$(INTDIR)\cllsn.sbr"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\credits.sbr"
	-@erase "$(INTDIR)\DIALOG.OBJ"
	-@erase "$(INTDIR)\DIALOG.SBR"
	-@erase "$(INTDIR)\DIB.OBJ"
	-@erase "$(INTDIR)\DIB.SBR"
	-@erase "$(INTDIR)\GCONTROL.OBJ"
	-@erase "$(INTDIR)\GCONTROL.SBR"
	-@erase "$(INTDIR)\HCONTROL.OBJ"
	-@erase "$(INTDIR)\HCONTROL.SBR"
	-@erase "$(INTDIR)\HSLIDER.OBJ"
	-@erase "$(INTDIR)\HSLIDER.SBR"
	-@erase "$(INTDIR)\HTTABLES.OBJ"
	-@erase "$(INTDIR)\HTTABLES.SBR"
	-@erase "$(INTDIR)\InfPanel.obj"
	-@erase "$(INTDIR)\InfPanel.sbr"
	-@erase "$(INTDIR)\Mci.obj"
	-@erase "$(INTDIR)\Mci.sbr"
	-@erase "$(INTDIR)\MCONTROL.OBJ"
	-@erase "$(INTDIR)\MCONTROL.SBR"
	-@erase "$(INTDIR)\MEMORY.OBJ"
	-@erase "$(INTDIR)\MEMORY.SBR"
	-@erase "$(INTDIR)\MIDINOTE.OBJ"
	-@erase "$(INTDIR)\MIDINOTE.SBR"
	-@erase "$(INTDIR)\OFFSCREN.OBJ"
	-@erase "$(INTDIR)\OFFSCREN.SBR"
	-@erase "$(INTDIR)\Opponent.obj"
	-@erase "$(INTDIR)\Opponent.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\PAL.OBJ"
	-@erase "$(INTDIR)\PAL.SBR"
	-@erase "$(INTDIR)\PARSER.OBJ"
	-@erase "$(INTDIR)\PARSER.SBR"
	-@erase "$(INTDIR)\PHAPP.OBJ"
	-@erase "$(INTDIR)\PHAPP.SBR"
	-@erase "$(INTDIR)\RANDSEQ.OBJ"
	-@erase "$(INTDIR)\RANDSEQ.SBR"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Rcontrol.sbr"
	-@erase "$(INTDIR)\SCENE.OBJ"
	-@erase "$(INTDIR)\SCENE.SBR"
	-@erase "$(INTDIR)\SCRIPT.OBJ"
	-@erase "$(INTDIR)\SCRIPT.SBR"
	-@erase "$(INTDIR)\SOUND.OBJ"
	-@erase "$(INTDIR)\SOUND.SBR"
	-@erase "$(INTDIR)\SPEEDCHK.OBJ"
	-@erase "$(INTDIR)\SPEEDCHK.SBR"
	-@erase "$(INTDIR)\SPRITE.OBJ"
	-@erase "$(INTDIR)\SPRITE.SBR"
	-@erase "$(INTDIR)\TRANSIT.OBJ"
	-@erase "$(INTDIR)\TRANSIT.SBR"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\UTILS.SBR"
	-@erase "$(INTDIR)\VCONTROL.OBJ"
	-@erase "$(INTDIR)\VCONTROL.SBR"
	-@erase "$(INTDIR)\VIDEO.OBJ"
	-@erase "$(INTDIR)\VIDEO.SBR"
	-@erase "$(INTDIR)\videodrw.obj"
	-@erase "$(INTDIR)\videodrw.sbr"
	-@erase "$(INTDIR)\videofx.obj"
	-@erase "$(INTDIR)\videofx.sbr"
	-@erase "$(INTDIR)\vsndmix.obj"
	-@erase "$(INTDIR)\vsndmix.sbr"
	-@erase "$(INTDIR)\VTABLE.OBJ"
	-@erase "$(INTDIR)\VTABLE.SBR"
	-@erase "$(INTDIR)\vworld.obj"
	-@erase "$(INTDIR)\vworld.sbr"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\weapon.sbr"
	-@erase "$(INTDIR)\WINTOOLS.OBJ"
	-@erase "$(INTDIR)\WINTOOLS.SBR"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase "$(INTDIR)\Worlds.sbr"
	-@erase "$(OUTDIR)\cllsn32.bsc"
	-@erase "$(OUTDIR)\cllsn32.pdb"
	-@erase ".\cllsn32.exe"
	-@erase ".\DEBUG32.PDB"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Zi /Od /Ob1 /Gy /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /Fd"DEBUG.PDB" /c
# ADD CPP /nologo /G5 /W3 /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr /Fd"DEBUG32.PDB" /c
CPP_PROJ=/nologo /G5 /MLd /W3 /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /Fr"$(INTDIR)/"\
 /Fo"$(INTDIR)/" /Fd"DEBUG32.PDB" /c 
CPP_OBJS=.\.\dbg32/
CPP_SBRS=.\.\dbg32/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "_DEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\gameguts" /d "_DEBUG"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/cllsn.res" /i "..\gameguts" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cllsn32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BCONTROL.SBR" \
	"$(INTDIR)\cllsn.sbr" \
	"$(INTDIR)\credits.sbr" \
	"$(INTDIR)\DIALOG.SBR" \
	"$(INTDIR)\DIB.SBR" \
	"$(INTDIR)\GCONTROL.SBR" \
	"$(INTDIR)\HCONTROL.SBR" \
	"$(INTDIR)\HSLIDER.SBR" \
	"$(INTDIR)\HTTABLES.SBR" \
	"$(INTDIR)\InfPanel.sbr" \
	"$(INTDIR)\Mci.sbr" \
	"$(INTDIR)\MCONTROL.SBR" \
	"$(INTDIR)\MEMORY.SBR" \
	"$(INTDIR)\MIDINOTE.SBR" \
	"$(INTDIR)\OFFSCREN.SBR" \
	"$(INTDIR)\Opponent.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\PAL.SBR" \
	"$(INTDIR)\PARSER.SBR" \
	"$(INTDIR)\PHAPP.SBR" \
	"$(INTDIR)\RANDSEQ.SBR" \
	"$(INTDIR)\Rcontrol.sbr" \
	"$(INTDIR)\SCENE.SBR" \
	"$(INTDIR)\SCRIPT.SBR" \
	"$(INTDIR)\SOUND.SBR" \
	"$(INTDIR)\SPEEDCHK.SBR" \
	"$(INTDIR)\SPRITE.SBR" \
	"$(INTDIR)\TRANSIT.SBR" \
	"$(INTDIR)\UTILS.SBR" \
	"$(INTDIR)\VCONTROL.SBR" \
	"$(INTDIR)\VIDEO.SBR" \
	"$(INTDIR)\videodrw.sbr" \
	"$(INTDIR)\videofx.sbr" \
	"$(INTDIR)\vsndmix.sbr" \
	"$(INTDIR)\VTABLE.SBR" \
	"$(INTDIR)\vworld.sbr" \
	"$(INTDIR)\weapon.sbr" \
	"$(INTDIR)\WINTOOLS.SBR" \
	"$(INTDIR)\Worlds.sbr"

"$(OUTDIR)\cllsn32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows /incremental:no /debug /machine:IX86 /out:".\cllsn32.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/cllsn32.pdb" /debug /machine:IX86\
 /out:".\cllsn32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BCONTROL.OBJ" \
	"$(INTDIR)\cllsn.obj" \
	"$(INTDIR)\cllsn.res" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\DIALOG.OBJ" \
	"$(INTDIR)\DIB.OBJ" \
	"$(INTDIR)\GCONTROL.OBJ" \
	"$(INTDIR)\HCONTROL.OBJ" \
	"$(INTDIR)\HSLIDER.OBJ" \
	"$(INTDIR)\HTTABLES.OBJ" \
	"$(INTDIR)\InfPanel.obj" \
	"$(INTDIR)\Mci.obj" \
	"$(INTDIR)\MCONTROL.OBJ" \
	"$(INTDIR)\MEMORY.OBJ" \
	"$(INTDIR)\MIDINOTE.OBJ" \
	"$(INTDIR)\OFFSCREN.OBJ" \
	"$(INTDIR)\Opponent.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\PAL.OBJ" \
	"$(INTDIR)\PARSER.OBJ" \
	"$(INTDIR)\PHAPP.OBJ" \
	"$(INTDIR)\RANDSEQ.OBJ" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\SCENE.OBJ" \
	"$(INTDIR)\SCRIPT.OBJ" \
	"$(INTDIR)\SOUND.OBJ" \
	"$(INTDIR)\SPEEDCHK.OBJ" \
	"$(INTDIR)\SPRITE.OBJ" \
	"$(INTDIR)\TRANSIT.OBJ" \
	"$(INTDIR)\UTILS.OBJ" \
	"$(INTDIR)\VCONTROL.OBJ" \
	"$(INTDIR)\VIDEO.OBJ" \
	"$(INTDIR)\videodrw.obj" \
	"$(INTDIR)\videofx.obj" \
	"$(INTDIR)\vsndmix.obj" \
	"$(INTDIR)\VTABLE.OBJ" \
	"$(INTDIR)\vworld.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\WINTOOLS.OBJ" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\VFW32.LIB" \
	"..\Gameguts\Wavmix32.lib" \
	"..\Gameguts\WING32.LIB"

".\cllsn32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=\DEV\SRC\Gameguts\SPEEDCHK.CPP
DEP_CPP_SPEED=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SPEEDCHK.OBJ" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SPEEDCHK.SBR" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\MCONTROL.CPP
DEP_CPP_MCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MCONTROL.OBJ" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MCONTROL.SBR" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\SCRIPT.CPP
DEP_CPP_SCRIP=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SCRIPT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SCRIPT.OBJ" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SCRIPT.SBR" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\VIDEO.CPP
DEP_CPP_VIDEO=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\vidobsrv.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\VIDEO.OBJ" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\VIDEO.SBR" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\DIB.CPP
DEP_CPP_DIB_C=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibfx.h"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\DIB.OBJ" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DIB.SBR" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\SPRITE.CPP
DEP_CPP_SPRIT=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SCRIPT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SPRITE.OBJ" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SPRITE.SBR" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\HSLIDER.CPP
DEP_CPP_HSLID=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HSLIDER.OBJ" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HSLIDER.SBR" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\HTTABLES.CPP
DEP_CPP_HTTAB=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HTTABLES.OBJ" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HTTABLES.SBR" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\HCONTROL.CPP
DEP_CPP_HCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\HCONTROL.OBJ" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\HCONTROL.SBR" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\PAL.CPP
DEP_CPP_PAL_C=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\PAL.OBJ" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\PAL.SBR" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\Mci.cpp
DEP_CPP_MCI_C=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Mci.obj" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Mci.sbr" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\VTABLE.CPP
DEP_CPP_VTABL=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PARSER.H"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\VTABLE.OBJ" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\VTABLE.SBR" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\TRANSIT.CPP
DEP_CPP_TRANS=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\RANDSEQ.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\gameguts\transit.h"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\TRANSIT.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\TRANSIT.SBR" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\WINTOOLS.CPP
DEP_CPP_WINTO=\
	"..\gameguts\commonid.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\WINTOOLS.OBJ" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\WINTOOLS.SBR" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\PHAPP.CPP
DEP_CPP_PHAPP=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\resource.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\PHAPP.OBJ" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\PHAPP.SBR" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\DIALOG.CPP
DEP_CPP_DIALO=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\DIALOG.OBJ" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DIALOG.SBR" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\SOUND.CPP
DEP_CPP_SOUND=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SOUND.SBR" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\SCENE.CPP
DEP_CPP_SCENE=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SCENE.OBJ" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SCENE.SBR" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\MEMORY.CPP
DEP_CPP_MEMOR=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMORY.OBJ" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMORY.SBR" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\GCONTROL.CPP
DEP_CPP_GCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\GCONTROL.OBJ" : $(SOURCE) $(DEP_CPP_GCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GCONTROL.SBR" : $(SOURCE) $(DEP_CPP_GCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\UTILS.CPP
DEP_CPP_UTILS=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\UTILS.OBJ" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UTILS.SBR" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\VCONTROL.CPP
DEP_CPP_VCONT=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\VCONTROL.OBJ" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\VCONTROL.SBR" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\OFFSCREN.CPP
DEP_CPP_OFFSC=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\OFFSCREN.OBJ" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OFFSCREN.SBR" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\PARSER.CPP
DEP_CPP_PARSE=\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PARSER.H"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\PARSER.OBJ" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\PARSER.SBR" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\BCONTROL.CPP
DEP_CPP_BCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WING.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\BCONTROL.OBJ" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\BCONTROL.SBR" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\videodrw.cpp
DEP_CPP_VIDEOD=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
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

SOURCE=\DEV\SRC\Gameguts\credits.cpp
DEP_CPP_CREDI=\
	"..\gameguts\credits.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PARSER.H"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
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

SOURCE=\DEV\SRC\Gameguts\Wavmix32.lib

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\WING32.LIB

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\VFW32.LIB

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\RANDSEQ.CPP
DEP_CPP_RANDS=\
	".\..\Gameguts\RANDSEQ.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\RANDSEQ.OBJ" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\RANDSEQ.SBR" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\options.cpp
DEP_CPP_OPTIO=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\cllsn\cllsnid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\cllsn.H"\
	".\options.h"\
	

"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\weapon.cpp
DEP_CPP_WEAPO=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\vidobsrv.h"\
	".\..\cllsn\cllsnid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\cllsn.P"\
	".\sndmix\sndmix.p"\
	".\vsndmix.h"\
	".\vworld.h"\
	".\wdebug.cpp"\
	".\wdebug.p"\
	

"$(INTDIR)\weapon.obj" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"

"$(INTDIR)\weapon.sbr" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\DEV\SRC\Gameguts\MIDINOTE.CPP
DEP_CPP_MIDIN=\
	"..\gameguts\dib.h"\
	"..\gameguts\midinote.p"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MIDINOTE.OBJ" : $(SOURCE) $(DEP_CPP_MIDIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MIDINOTE.SBR" : $(SOURCE) $(DEP_CPP_MIDIN) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsn.CPP
DEP_CPP_CLLSN=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\credits.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\vidobsrv.h"\
	".\..\cllsn\cllsnid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\cllsn.H"\
	".\InfPanel.h"\
	".\options.h"\
	".\rcontrol.p"\
	".\sndmix\sndmix.p"\
	".\videofx.h"\
	".\vsndmix.h"\
	".\vworld.h"\
	".\Worlds.h"\
	

"$(INTDIR)\cllsn.obj" : $(SOURCE) $(DEP_CPP_CLLSN) "$(INTDIR)"

"$(INTDIR)\cllsn.sbr" : $(SOURCE) $(DEP_CPP_CLLSN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cllsn.RC
DEP_RSC_CLLSN_=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\resource.h"\
	".\..\cllsn\cllsnid.h"\
	

"$(INTDIR)\cllsn.res" : $(SOURCE) $(DEP_RSC_CLLSN_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Rcontrol.cpp
DEP_CPP_RCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\rcontrol.p"\
	".\Worlds.h"\
	

"$(INTDIR)\Rcontrol.obj" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"

"$(INTDIR)\Rcontrol.sbr" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Worlds.cpp
DEP_CPP_WORLD=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\cllsn\cllsnid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\Worlds.h"\
	

"$(INTDIR)\Worlds.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"

"$(INTDIR)\Worlds.sbr" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Opponent.cpp
DEP_CPP_OPPON=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\Worlds.h"\
	

"$(INTDIR)\Opponent.obj" : $(SOURCE) $(DEP_CPP_OPPON) "$(INTDIR)"

"$(INTDIR)\Opponent.sbr" : $(SOURCE) $(DEP_CPP_OPPON) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Worlds.h

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

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

SOURCE=.\options.h

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Weapon.h

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

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

SOURCE=.\InfPanel.cpp
DEP_CPP_INFPA=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\InfPanel.h"\
	".\Worlds.h"\
	

"$(INTDIR)\InfPanel.obj" : $(SOURCE) $(DEP_CPP_INFPA) "$(INTDIR)"

"$(INTDIR)\InfPanel.sbr" : $(SOURCE) $(DEP_CPP_INFPA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\vworld.cpp
DEP_CPP_VWORL=\
	"..\cllsn\Weapon.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\vidobsrv.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\sndmix\sndmix.p"\
	".\vsndmix.h"\
	".\vworld.h"\
	

"$(INTDIR)\vworld.obj" : $(SOURCE) $(DEP_CPP_VWORL) "$(INTDIR)"

"$(INTDIR)\vworld.sbr" : $(SOURCE) $(DEP_CPP_VWORL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\videofx.cpp
DEP_CPP_VIDEOF=\
	"..\gameguts\control.h"\
	"..\gameguts\dib.h"\
	"..\gameguts\phapp.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\gameguts\vidobsrv.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\dibid.h"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHTYPES.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\sndmix\sndmix.p"\
	".\videofx.h"\
	".\vsndmix.h"\
	

"$(INTDIR)\videofx.obj" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"

"$(INTDIR)\videofx.sbr" : $(SOURCE) $(DEP_CPP_VIDEOF) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\vsndmix.cpp
DEP_CPP_VSNDM=\
	".\sndmix\sndmix.p"\
	".\vsndmix.h"\
	

"$(INTDIR)\vsndmix.obj" : $(SOURCE) $(DEP_CPP_VSNDM) "$(INTDIR)"

"$(INTDIR)\vsndmix.sbr" : $(SOURCE) $(DEP_CPP_VSNDM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SndMix.dll

!IF  "$(CFG)" == "cllsn32 - Win32 Release"

!ELSEIF  "$(CFG)" == "cllsn32 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
