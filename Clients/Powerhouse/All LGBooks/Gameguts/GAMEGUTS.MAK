# Microsoft Visual C++ generated build script - Do not modify

PROJ = GAMEGUTS
DEBUG = 0
PROGTYPE = 4
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = N:\DEV\ARCHIVE\ALLLGB~1\GAMEGUTS\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = DIALOG.CPP  
RC = rc
CFLAGS_D_LIB = /nologo /f- /G3 /Gy /W3 /Z7 /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /D "USE_WINTOON" /D "NO_MIDI" /D "NO_CONTROLS" /D "USE_DIBFX" /FR /GA 
CFLAGS_R_LIB = /nologo /f- /Gs /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /D "USE_WINTOON" /D "NO_MIDI" /D "NO_CONTROLS" /D "USE_DIBFX" /FR /GA 
RCFLAGS = /nologo 
RESFLAGS = /nologo /k
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_LIB)
LFLAGS = 
LIBS = 
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_LIB)
LFLAGS = 
LIBS = 
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = DIALOG.SBR \
		DIB.SBR \
		HTTABLES.SBR \
		MCI.SBR \
		MEMORY.SBR \
		OFFSCREN.SBR \
		PAL.SBR \
		SCRIPT.SBR \
		SOUND.SBR \
		SPEEDCHK.SBR \
		SPRITE.SBR \
		WINTOOLS.SBR \
		SCENE.SBR \
		PHAPP.SBR \
		TRANSIT.SBR \
		TOON.SBR \
		PARSER.SBR \
		RANDSEQ.SBR \
		DIBPRINT.SBR \
		BCONTROL.SBR \
		CREDITS.SBR \
		GCONTROL.SBR \
		HCONTROL.SBR \
		HSLIDER.SBR \
		MCONTROL.SBR \
		MIDINOTE.SBR \
		TGARDR.SBR \
		VCONTROL.SBR \
		VIDEO.SBR \
		VIDEODRW.SBR \
		VTABLE.SBR


DIALOG_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


DIB_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\wing.h \
	n:\dev\archive\alllgb~1\gameguts\dibfx.h


HTTABLES_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\wing.h


MCI_DEP = n:\dev\archive\alllgb~1\gameguts\digitalv.h \
	n:\dev\archive\alllgb~1\gameguts\mciavi.h \
	n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


MEMORY_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


OFFSCREN_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\wing.h


PAL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


SCRIPT_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\script.h


SOUND_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


SPEEDCHK_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


SPRITE_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\script.h


WINTOOLS_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\commonid.h \
	n:\dev\archive\alllgb~1\gameguts\wing.h


SCENE_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h


PHAPP_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\wing.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\resource.h


TRANSIT_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\transit.h \
	n:\dev\archive\alllgb~1\gameguts\wing.h \
	n:\dev\archive\alllgb~1\gameguts\randseq.h


TOON_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\transit.h \
	n:\dev\archive\alllgb~1\gameguts\parser.h \
	n:\dev\archive\alllgb~1\gameguts\wing.h \
	n:\dev\archive\alllgb~1\gameguts\dibfx.h


PARSER_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\parser.h


RANDSEQ_DEP = n:\dev\archive\alllgb~1\gameguts\randseq.h


DIBPRINT_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p


BCONTROL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\wing.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h


CREDITS_DEP = n:\dev\archive\alllgb~1\gameguts\credits.h \
	n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\parser.h


GCONTROL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h


HCONTROL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h


HSLIDER_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h


MCONTROL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\wing.h


MIDINOTE_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\midinote.p


TGARDR_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\tgardr.h


VCONTROL_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h


VIDEO_DEP = n:\dev\archive\alllgb~1\gameguts\digitalv.h \
	n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mciavi.h \
	n:\dev\archive\alllgb~1\gameguts\vidobsrv.h \
	n:\dev\archive\alllgb~1\gameguts\vfw.h


VIDEODRW_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h


VTABLE_DEP = n:\dev\archive\alllgb~1\gameguts\proto.h \
	n:\dev\archive\alllgb~1\gameguts\phtypes.h \
	n:\dev\archive\alllgb~1\gameguts\dib.h \
	n:\dev\archive\alllgb~1\gameguts\dibid.h \
	n:\dev\archive\alllgb~1\gameguts\offscren.h \
	n:\dev\archive\alllgb~1\gameguts\macros.h \
	n:\dev\archive\alllgb~1\gameguts\port.h \
	n:\dev\archive\alllgb~1\gameguts\vtable.h \
	n:\dev\archive\alllgb~1\gameguts\scene.h \
	n:\dev\archive\alllgb~1\gameguts\sprite.h \
	n:\dev\archive\alllgb~1\gameguts\toon.h \
	n:\dev\archive\alllgb~1\gameguts\wintoon.h \
	n:\dev\archive\alllgb~1\gameguts\sound.h \
	n:\dev\archive\alllgb~1\gameguts\wavemix.h \
	n:\dev\archive\alllgb~1\gameguts\phapp.h \
	n:\dev\archive\alllgb~1\gameguts\bcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\dialog.p \
	n:\dev\archive\alllgb~1\gameguts\hcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\httables.p \
	n:\dev\archive\alllgb~1\gameguts\mci.p \
	n:\dev\archive\alllgb~1\gameguts\memory.p \
	n:\dev\archive\alllgb~1\gameguts\pal.p \
	n:\dev\archive\alllgb~1\gameguts\speedchk.p \
	n:\dev\archive\alllgb~1\gameguts\transit.p \
	n:\dev\archive\alllgb~1\gameguts\vcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\video.p \
	n:\dev\archive\alllgb~1\gameguts\videodrw.p \
	n:\dev\archive\alllgb~1\gameguts\vtable.p \
	n:\dev\archive\alllgb~1\gameguts\wintools.p \
	n:\dev\archive\alllgb~1\gameguts\hslider.p \
	n:\dev\archive\alllgb~1\gameguts\gcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\mcontrol.p \
	n:\dev\archive\alllgb~1\gameguts\control.h \
	n:\dev\archive\alllgb~1\gameguts\commonid.h \
	n:\dev\archive\alllgb~1\gameguts\parser.h


all:	$(PROJ).LIB $(PROJ).BSC

DIALOG.OBJ:	DIALOG.CPP $(DIALOG_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c DIALOG.CPP

DIB.OBJ:	DIB.CPP $(DIB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIB.CPP

HTTABLES.OBJ:	HTTABLES.CPP $(HTTABLES_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HTTABLES.CPP

MCI.OBJ:	MCI.CPP $(MCI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MCI.CPP

MEMORY.OBJ:	MEMORY.CPP $(MEMORY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MEMORY.CPP

OFFSCREN.OBJ:	OFFSCREN.CPP $(OFFSCREN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c OFFSCREN.CPP

PAL.OBJ:	PAL.CPP $(PAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PAL.CPP

SCRIPT.OBJ:	SCRIPT.CPP $(SCRIPT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCRIPT.CPP

SOUND.OBJ:	SOUND.CPP $(SOUND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SOUND.CPP

SPEEDCHK.OBJ:	SPEEDCHK.CPP $(SPEEDCHK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPEEDCHK.CPP

SPRITE.OBJ:	SPRITE.CPP $(SPRITE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPRITE.CPP

WINTOOLS.OBJ:	WINTOOLS.CPP $(WINTOOLS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINTOOLS.CPP

SCENE.OBJ:	SCENE.CPP $(SCENE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCENE.CPP

PHAPP.OBJ:	PHAPP.CPP $(PHAPP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PHAPP.CPP

TRANSIT.OBJ:	TRANSIT.CPP $(TRANSIT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TRANSIT.CPP

TOON.OBJ:	TOON.CPP $(TOON_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TOON.CPP

PARSER.OBJ:	PARSER.CPP $(PARSER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PARSER.CPP

RANDSEQ.OBJ:	RANDSEQ.CPP $(RANDSEQ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c RANDSEQ.CPP

DIBPRINT.OBJ:	DIBPRINT.CPP $(DIBPRINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIBPRINT.CPP

BCONTROL.OBJ:	BCONTROL.CPP $(BCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BCONTROL.CPP

CREDITS.OBJ:	CREDITS.CPP $(CREDITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CREDITS.CPP

GCONTROL.OBJ:	GCONTROL.CPP $(GCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GCONTROL.CPP

HCONTROL.OBJ:	HCONTROL.CPP $(HCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HCONTROL.CPP

HSLIDER.OBJ:	HSLIDER.CPP $(HSLIDER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HSLIDER.CPP

MCONTROL.OBJ:	MCONTROL.CPP $(MCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MCONTROL.CPP

MIDINOTE.OBJ:	MIDINOTE.CPP $(MIDINOTE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MIDINOTE.CPP

TGARDR.OBJ:	TGARDR.CPP $(TGARDR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TGARDR.CPP

VCONTROL.OBJ:	VCONTROL.CPP $(VCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VCONTROL.CPP

VIDEO.OBJ:	VIDEO.CPP $(VIDEO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIDEO.CPP

VIDEODRW.OBJ:	VIDEODRW.CPP $(VIDEODRW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIDEODRW.CPP

VTABLE.OBJ:	VTABLE.CPP $(VTABLE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VTABLE.CPP

$(PROJ).LIB::	DIALOG.OBJ DIB.OBJ HTTABLES.OBJ MCI.OBJ MEMORY.OBJ OFFSCREN.OBJ PAL.OBJ \
	SCRIPT.OBJ SOUND.OBJ SPEEDCHK.OBJ SPRITE.OBJ WINTOOLS.OBJ SCENE.OBJ PHAPP.OBJ TRANSIT.OBJ \
	TOON.OBJ PARSER.OBJ RANDSEQ.OBJ DIBPRINT.OBJ BCONTROL.OBJ CREDITS.OBJ GCONTROL.OBJ \
	HCONTROL.OBJ HSLIDER.OBJ MCONTROL.OBJ MIDINOTE.OBJ TGARDR.OBJ VCONTROL.OBJ VIDEO.OBJ \
	VIDEODRW.OBJ VTABLE.OBJ $(OBJS_EXT)
	echo >NUL @<<$(PROJ).CRF
$@ /PAGESIZE:64
y
+DIALOG.OBJ &
+DIB.OBJ &
+HTTABLES.OBJ &
+MCI.OBJ &
+MEMORY.OBJ &
+OFFSCREN.OBJ &
+PAL.OBJ &
+SCRIPT.OBJ &
+SOUND.OBJ &
+SPEEDCHK.OBJ &
+SPRITE.OBJ &
+WINTOOLS.OBJ &
+SCENE.OBJ &
+PHAPP.OBJ &
+TRANSIT.OBJ &
+TOON.OBJ &
+PARSER.OBJ &
+RANDSEQ.OBJ &
+DIBPRINT.OBJ &
+BCONTROL.OBJ &
+CREDITS.OBJ &
+GCONTROL.OBJ &
+HCONTROL.OBJ &
+HSLIDER.OBJ &
+MCONTROL.OBJ &
+MIDINOTE.OBJ &
+TGARDR.OBJ &
+VCONTROL.OBJ &
+VIDEO.OBJ &
+VIDEODRW.OBJ &
+VTABLE.OBJ &
;
<<
	if exist $@ del $@
	lib @$(PROJ).CRF

$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
