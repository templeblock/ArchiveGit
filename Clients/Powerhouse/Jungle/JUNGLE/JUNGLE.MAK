# Microsoft Visual C++ generated build script - Do not modify

PROJ = JUNGLE
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = c:\windows\system\krnl386.exe
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = N:\DEV\ARCHIVE\JUNGLE\JUNGLE\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = JUNGLE.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /f- /G3 /Gy /W3 /Zi /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /D "VOICE_RECO" /I "..\gameguts" /FR /GA /Fd"DEBUG.PDB"
CFLAGS_R_WEXE = /nologo /f- /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /D "VOICE_RECO" /I "d:\dev\src\gameguts" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
LIBS_R_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
RCFLAGS = /x /v /i c:\msvc\include /i ..\gameguts 
RESFLAGS = /k
RUNFLAGS = 
DEFFILE = ..\GAMEGUTS\POWER.DEF
OBJS_EXT = 
LIBS_EXT = ..\GAMEGUTS\WAVEMIX.LIB ..\GAMEGUTS\WING.LIB ..\GAMEGUTS\VFW.LIB ICWAPI16.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = JUNGLE.SBR \
		BCONTROL.SBR \
		DIALOG.SBR \
		DIB.SBR \
		HCONTROL.SBR \
		HTTABLES.SBR \
		MCI.SBR \
		MEMORY.SBR \
		OFFSCREN.SBR \
		PAL.SBR \
		SCRIPT.SBR \
		SOUND.SBR \
		SPEEDCHK.SBR \
		SPRITE.SBR \
		UTILS.SBR \
		VCONTROL.SBR \
		VIDEO.SBR \
		WINTOOLS.SBR \
		TRANSIT.SBR \
		PHAPP.SBR \
		SCENE.SBR \
		HSLIDER.SBR \
		GCONTROL.SBR \
		VTABLE.SBR \
		VRECO.SBR \
		PARSER.SBR \
		MCONTROL.SBR \
		BMULTI.SBR \
		CREDITS.SBR \
		PUZZLE2.SBR \
		PUZZLE3.SBR \
		PUZZLE5.SBR \
		PUZZLE4.SBR \
		RANDSEQ.SBR \
		VIDEODRW.SBR \
		RCONTROL.SBR \
		WORLDS.SBR


WAVEMIX_DEP = 

WING_DEP = 

VFW_DEP = 

ICWAPI16_DEP = 

JUNGLE_DEP = n:\dev\archive\jungle\jungle\jungle.h \
	n:\dev\archive\jungle\jungle\jungleid.h \
	n:\dev\archive\jungle\jungle\vreco.h


JUNGLE_RCDEP = n:\dev\archive\jungle\jungle\jungleid.h


BCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\wing.h


DIALOG_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


DIB_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\wing.h


HCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h


HTTABLES_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\wing.h


MCI_DEP = n:\dev\archive\jungle\gameguts\digitalv.h \
	n:\dev\archive\jungle\gameguts\mciavi.h \
	n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


MEMORY_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


OFFSCREN_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\wing.h


PAL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


SCRIPT_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\script.h


SOUND_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


SPEEDCHK_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


SPRITE_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\script.h


UTILS_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


VCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\commonid.h


VIDEO_DEP = n:\dev\archive\jungle\gameguts\digitalv.h \
	n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\mciavi.h \
	n:\dev\archive\jungle\gameguts\vfw.h


WINTOOLS_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\commonid.h \
	n:\dev\archive\jungle\gameguts\wing.h


TRANSIT_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\transit.h \
	n:\dev\archive\jungle\gameguts\wing.h \
	n:\dev\archive\jungle\gameguts\randseq.h


PHAPP_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\wing.h \
	n:\dev\archive\jungle\gameguts\commonid.h \
	n:\dev\archive\jungle\gameguts\control.h


SCENE_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p


HSLIDER_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h


GCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h


VTABLE_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\commonid.h \
	n:\dev\archive\jungle\gameguts\parser.h


VRECO_DEP = n:\dev\archive\jungle\jungle\vreco.h \
	n:\dev\archive\jungle\jungle\vmapi.h \
	n:\dev\archive\jungle\jungle\icwapi32.h \
	n:\dev\archive\jungle\jungle\icwapi16.h


PARSER_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\parser.h


MCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\wing.h


BMULTI_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h


CREDITS_DEP = n:\dev\archive\jungle\gameguts\credits.h \
	n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\parser.h


PUZZLE2_DEP = n:\dev\archive\jungle\jungle\jungle.h \
	n:\dev\archive\jungle\jungle\jungleid.h


PUZZLE3_DEP = n:\dev\archive\jungle\jungle\jungle.h \
	n:\dev\archive\jungle\jungle\jungleid.h


PUZZLE5_DEP = n:\dev\archive\jungle\jungle\jungle.h \
	n:\dev\archive\jungle\jungle\jungleid.h


PUZZLE4_DEP = n:\dev\archive\jungle\jungle\jungle.h \
	n:\dev\archive\jungle\jungle\jungleid.h


RANDSEQ_DEP = n:\dev\archive\jungle\gameguts\randseq.h


VIDEODRW_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\jungle\jungleid.h


RCONTROL_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\worlds.h


WORLDS_DEP = n:\dev\archive\jungle\gameguts\proto.h \
	n:\dev\archive\jungle\gameguts\phtypes.h \
	n:\dev\archive\jungle\gameguts\dib.h \
	n:\dev\archive\jungle\gameguts\offscren.h \
	n:\dev\archive\jungle\gameguts\macros.h \
	n:\dev\archive\jungle\gameguts\port.h \
	n:\dev\archive\jungle\gameguts\vtable.h \
	n:\dev\archive\jungle\gameguts\scene.h \
	n:\dev\archive\jungle\gameguts\sprite.h \
	n:\dev\archive\jungle\gameguts\toon.h \
	n:\dev\archive\jungle\gameguts\wintoon.h \
	n:\dev\archive\jungle\gameguts\sound.h \
	n:\dev\archive\jungle\gameguts\wavemix.h \
	n:\dev\archive\jungle\gameguts\phapp.h \
	n:\dev\archive\jungle\gameguts\bcontrol.p \
	n:\dev\archive\jungle\gameguts\dialog.p \
	n:\dev\archive\jungle\gameguts\hcontrol.p \
	n:\dev\archive\jungle\gameguts\httables.p \
	n:\dev\archive\jungle\gameguts\mci.p \
	n:\dev\archive\jungle\gameguts\memory.p \
	n:\dev\archive\jungle\gameguts\pal.p \
	n:\dev\archive\jungle\gameguts\speedchk.p \
	n:\dev\archive\jungle\gameguts\transit.p \
	n:\dev\archive\jungle\gameguts\utils.p \
	n:\dev\archive\jungle\gameguts\vcontrol.p \
	n:\dev\archive\jungle\gameguts\video.p \
	n:\dev\archive\jungle\gameguts\videodrw.p \
	n:\dev\archive\jungle\gameguts\vtable.p \
	n:\dev\archive\jungle\gameguts\wintools.p \
	n:\dev\archive\jungle\gameguts\hslider.p \
	n:\dev\archive\jungle\gameguts\gcontrol.p \
	n:\dev\archive\jungle\gameguts\rcontrol.p \
	n:\dev\archive\jungle\gameguts\mcontrol.p \
	n:\dev\archive\jungle\gameguts\bmulti.p \
	n:\dev\archive\jungle\gameguts\control.h \
	n:\dev\archive\jungle\gameguts\worlds.h


all:	$(PROJ).EXE $(PROJ).BSC

JUNGLE.OBJ:	JUNGLE.CPP $(JUNGLE_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c JUNGLE.CPP

JUNGLE.RES:	JUNGLE.RC $(JUNGLE_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r JUNGLE.RC

BCONTROL.OBJ:	..\GAMEGUTS\BCONTROL.CPP $(BCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\BCONTROL.CPP

DIALOG.OBJ:	..\GAMEGUTS\DIALOG.CPP $(DIALOG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\DIALOG.CPP

DIB.OBJ:	..\GAMEGUTS\DIB.CPP $(DIB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\DIB.CPP

HCONTROL.OBJ:	..\GAMEGUTS\HCONTROL.CPP $(HCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\HCONTROL.CPP

HTTABLES.OBJ:	..\GAMEGUTS\HTTABLES.CPP $(HTTABLES_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\HTTABLES.CPP

MCI.OBJ:	..\GAMEGUTS\MCI.CPP $(MCI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MCI.CPP

MEMORY.OBJ:	..\GAMEGUTS\MEMORY.CPP $(MEMORY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MEMORY.CPP

OFFSCREN.OBJ:	..\GAMEGUTS\OFFSCREN.CPP $(OFFSCREN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\OFFSCREN.CPP

PAL.OBJ:	..\GAMEGUTS\PAL.CPP $(PAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\PAL.CPP

SCRIPT.OBJ:	..\GAMEGUTS\SCRIPT.CPP $(SCRIPT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\SCRIPT.CPP

SOUND.OBJ:	..\GAMEGUTS\SOUND.CPP $(SOUND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\SOUND.CPP

SPEEDCHK.OBJ:	..\GAMEGUTS\SPEEDCHK.CPP $(SPEEDCHK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\SPEEDCHK.CPP

SPRITE.OBJ:	..\GAMEGUTS\SPRITE.CPP $(SPRITE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\SPRITE.CPP

UTILS.OBJ:	..\GAMEGUTS\UTILS.CPP $(UTILS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\UTILS.CPP

VCONTROL.OBJ:	..\GAMEGUTS\VCONTROL.CPP $(VCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VCONTROL.CPP

VIDEO.OBJ:	..\GAMEGUTS\VIDEO.CPP $(VIDEO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VIDEO.CPP

WINTOOLS.OBJ:	..\GAMEGUTS\WINTOOLS.CPP $(WINTOOLS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\WINTOOLS.CPP

TRANSIT.OBJ:	..\GAMEGUTS\TRANSIT.CPP $(TRANSIT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\TRANSIT.CPP

PHAPP.OBJ:	..\GAMEGUTS\PHAPP.CPP $(PHAPP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\PHAPP.CPP

SCENE.OBJ:	..\GAMEGUTS\SCENE.CPP $(SCENE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\SCENE.CPP

HSLIDER.OBJ:	..\GAMEGUTS\HSLIDER.CPP $(HSLIDER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\HSLIDER.CPP

GCONTROL.OBJ:	..\GAMEGUTS\GCONTROL.CPP $(GCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\GCONTROL.CPP

VTABLE.OBJ:	..\GAMEGUTS\VTABLE.CPP $(VTABLE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VTABLE.CPP

VRECO.OBJ:	VRECO.CPP $(VRECO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VRECO.CPP

PARSER.OBJ:	..\GAMEGUTS\PARSER.CPP $(PARSER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\PARSER.CPP

MCONTROL.OBJ:	..\GAMEGUTS\MCONTROL.CPP $(MCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MCONTROL.CPP

BMULTI.OBJ:	..\GAMEGUTS\BMULTI.CPP $(BMULTI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\BMULTI.CPP

CREDITS.OBJ:	..\GAMEGUTS\CREDITS.CPP $(CREDITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\CREDITS.CPP

PUZZLE2.OBJ:	PUZZLE2.CPP $(PUZZLE2_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PUZZLE2.CPP

PUZZLE3.OBJ:	PUZZLE3.CPP $(PUZZLE3_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PUZZLE3.CPP

PUZZLE5.OBJ:	PUZZLE5.CPP $(PUZZLE5_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PUZZLE5.CPP

PUZZLE4.OBJ:	PUZZLE4.CPP $(PUZZLE4_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PUZZLE4.CPP

RANDSEQ.OBJ:	..\GAMEGUTS\RANDSEQ.CPP $(RANDSEQ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\RANDSEQ.CPP

VIDEODRW.OBJ:	..\GAMEGUTS\VIDEODRW.CPP $(VIDEODRW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VIDEODRW.CPP

RCONTROL.OBJ:	..\GAMEGUTS\RCONTROL.CPP $(RCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\RCONTROL.CPP

WORLDS.OBJ:	..\GAMEGUTS\WORLDS.CPP $(WORLDS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\WORLDS.CPP


$(PROJ).EXE::	JUNGLE.RES

$(PROJ).EXE::	JUNGLE.OBJ BCONTROL.OBJ DIALOG.OBJ DIB.OBJ HCONTROL.OBJ HTTABLES.OBJ \
	MCI.OBJ MEMORY.OBJ OFFSCREN.OBJ PAL.OBJ SCRIPT.OBJ SOUND.OBJ SPEEDCHK.OBJ SPRITE.OBJ \
	UTILS.OBJ VCONTROL.OBJ VIDEO.OBJ WINTOOLS.OBJ TRANSIT.OBJ PHAPP.OBJ SCENE.OBJ HSLIDER.OBJ \
	GCONTROL.OBJ VTABLE.OBJ VRECO.OBJ PARSER.OBJ MCONTROL.OBJ BMULTI.OBJ CREDITS.OBJ \
	PUZZLE2.OBJ PUZZLE3.OBJ PUZZLE5.OBJ PUZZLE4.OBJ RANDSEQ.OBJ VIDEODRW.OBJ RCONTROL.OBJ \
	WORLDS.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
JUNGLE.OBJ +
BCONTROL.OBJ +
DIALOG.OBJ +
DIB.OBJ +
HCONTROL.OBJ +
HTTABLES.OBJ +
MCI.OBJ +
MEMORY.OBJ +
OFFSCREN.OBJ +
PAL.OBJ +
SCRIPT.OBJ +
SOUND.OBJ +
SPEEDCHK.OBJ +
SPRITE.OBJ +
UTILS.OBJ +
VCONTROL.OBJ +
VIDEO.OBJ +
WINTOOLS.OBJ +
TRANSIT.OBJ +
PHAPP.OBJ +
SCENE.OBJ +
HSLIDER.OBJ +
GCONTROL.OBJ +
VTABLE.OBJ +
VRECO.OBJ +
PARSER.OBJ +
MCONTROL.OBJ +
BMULTI.OBJ +
CREDITS.OBJ +
PUZZLE2.OBJ +
PUZZLE3.OBJ +
PUZZLE5.OBJ +
PUZZLE4.OBJ +
RANDSEQ.OBJ +
VIDEODRW.OBJ +
RCONTROL.OBJ +
WORLDS.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
..\GAMEGUTS\WAVEMIX.LIB+
..\GAMEGUTS\WING.LIB+
..\GAMEGUTS\VFW.LIB+
ICWAPI16.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) JUNGLE.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	JUNGLE.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) JUNGLE.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
