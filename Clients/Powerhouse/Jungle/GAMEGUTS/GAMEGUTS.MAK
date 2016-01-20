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
PROJPATH = D:\DEV\SRC\GAMEGUTS\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = BCONTROL.CPP
RC = rc
CFLAGS_D_LIB = /nologo /f- /G3 /Gy /W3 /Zi /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /FR /GA /Fd"DEBUG.PDB"
CFLAGS_R_LIB = /nologo /f- /Gs /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /FR /GA 
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
SBRS = BCONTROL.SBR \
		DIALOG.SBR \
		DIB.SBR \
		GCONTROL.SBR \
		HCONTROL.SBR \
		HSLIDER.SBR \
		HTTABLES.SBR \
		MCI.SBR \
		MCONTROL.SBR \
		MEMORY.SBR \
		OFFSCREN.SBR \
		PAL.SBR \
		PARSER.SBR \
		PHAPP.SBR \
		SCENE.SBR \
		SCRIPT.SBR \
		SOUND.SBR \
		SPEEDCHK.SBR \
		SPRITE.SBR \
		TRANSIT.SBR \
		UTILS.SBR \
		VCONTROL.SBR \
		VIDEO.SBR \
		VTABLE.SBR \
		WINTOOLS.SBR


BCONTROL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h \
	d:\dev\src\gameguts\wing.h


DIALOG_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


DIB_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\wing.h


GCONTROL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h


HCONTROL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h


HSLIDER_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h


HTTABLES_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\wing.h


MCI_DEP = d:\dev\src\gameguts\digitalv.h \
	d:\dev\src\gameguts\mciavi.h \
	d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


MCONTROL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h \
	d:\dev\src\gameguts\wing.h


MEMORY_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


OFFSCREN_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\wing.h


PAL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


PARSER_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\parser.h


PHAPP_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h \
	d:\dev\src\gameguts\wing.h \
	d:\dev\src\gameguts\commonid.h \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\vreco.p


SCENE_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


SCRIPT_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\script.h


SOUND_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\sound.h \
	d:\dev\src\gameguts\wavemix.h


SPEEDCHK_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


SPRITE_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\script.h


TRANSIT_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\transit.h \
	d:\dev\src\gameguts\wing.h


UTILS_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p


VCONTROL_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\commonid.h \
	d:\dev\src\jungle\jungleid.h


VIDEO_DEP = d:\dev\src\gameguts\digitalv.h \
	d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\mciavi.h \
	d:\dev\src\gameguts\vfw.h


VTABLE_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\control.h \
	d:\dev\src\gameguts\commonid.h \
	d:\dev\src\gameguts\parser.h


WINTOOLS_DEP = d:\dev\src\gameguts\proto.h \
	d:\dev\src\gameguts\phtypes.h \
	d:\dev\src\gameguts\dib.h \
	d:\dev\src\gameguts\offscren.h \
	d:\dev\src\gameguts\macros.h \
	d:\dev\src\gameguts\port.h \
	d:\dev\src\gameguts\vtable.h \
	d:\dev\src\gameguts\scene.h \
	d:\dev\src\gameguts\sprite.h \
	d:\dev\src\gameguts\toon.h \
	d:\dev\src\gameguts\wintoon.h \
	d:\dev\src\gameguts\phapp.h \
	d:\dev\src\gameguts\bcontrol.p \
	d:\dev\src\gameguts\dialog.p \
	d:\dev\src\gameguts\hcontrol.p \
	d:\dev\src\gameguts\httables.p \
	d:\dev\src\gameguts\mci.p \
	d:\dev\src\gameguts\memory.p \
	d:\dev\src\gameguts\pal.p \
	d:\dev\src\gameguts\speedchk.p \
	d:\dev\src\gameguts\transit.p \
	d:\dev\src\gameguts\utils.p \
	d:\dev\src\gameguts\vcontrol.p \
	d:\dev\src\gameguts\video.p \
	d:\dev\src\gameguts\vtable.p \
	d:\dev\src\gameguts\wintools.p \
	d:\dev\src\gameguts\hslider.p \
	d:\dev\src\gameguts\gcontrol.p \
	d:\dev\src\gameguts\mcontrol.p \
	d:\dev\src\gameguts\title.p \
	d:\dev\src\gameguts\commonid.h


all:	$(PROJ).LIB $(PROJ).BSC

BCONTROL.OBJ:	BCONTROL.CPP $(BCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c BCONTROL.CPP

DIALOG.OBJ:	DIALOG.CPP $(DIALOG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIALOG.CPP

DIB.OBJ:	DIB.CPP $(DIB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIB.CPP

GCONTROL.OBJ:	GCONTROL.CPP $(GCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GCONTROL.CPP

HCONTROL.OBJ:	HCONTROL.CPP $(HCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HCONTROL.CPP

HSLIDER.OBJ:	HSLIDER.CPP $(HSLIDER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HSLIDER.CPP

HTTABLES.OBJ:	HTTABLES.CPP $(HTTABLES_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HTTABLES.CPP

MCI.OBJ:	MCI.CPP $(MCI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MCI.CPP

MCONTROL.OBJ:	MCONTROL.CPP $(MCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MCONTROL.CPP

MEMORY.OBJ:	MEMORY.CPP $(MEMORY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MEMORY.CPP

OFFSCREN.OBJ:	OFFSCREN.CPP $(OFFSCREN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c OFFSCREN.CPP

PAL.OBJ:	PAL.CPP $(PAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PAL.CPP

PARSER.OBJ:	PARSER.CPP $(PARSER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PARSER.CPP

PHAPP.OBJ:	PHAPP.CPP $(PHAPP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PHAPP.CPP

SCENE.OBJ:	SCENE.CPP $(SCENE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCENE.CPP

SCRIPT.OBJ:	SCRIPT.CPP $(SCRIPT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCRIPT.CPP

SOUND.OBJ:	SOUND.CPP $(SOUND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SOUND.CPP

SPEEDCHK.OBJ:	SPEEDCHK.CPP $(SPEEDCHK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPEEDCHK.CPP

SPRITE.OBJ:	SPRITE.CPP $(SPRITE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPRITE.CPP

TRANSIT.OBJ:	TRANSIT.CPP $(TRANSIT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TRANSIT.CPP

UTILS.OBJ:	UTILS.CPP $(UTILS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTILS.CPP

VCONTROL.OBJ:	VCONTROL.CPP $(VCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VCONTROL.CPP

VIDEO.OBJ:	VIDEO.CPP $(VIDEO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VIDEO.CPP

VTABLE.OBJ:	VTABLE.CPP $(VTABLE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c VTABLE.CPP

WINTOOLS.OBJ:	WINTOOLS.CPP $(WINTOOLS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WINTOOLS.CPP

$(PROJ).LIB::	BCONTROL.OBJ DIALOG.OBJ DIB.OBJ GCONTROL.OBJ HCONTROL.OBJ HSLIDER.OBJ \
	HTTABLES.OBJ MCI.OBJ MCONTROL.OBJ MEMORY.OBJ OFFSCREN.OBJ PAL.OBJ PARSER.OBJ PHAPP.OBJ \
	SCENE.OBJ SCRIPT.OBJ SOUND.OBJ SPEEDCHK.OBJ SPRITE.OBJ TRANSIT.OBJ UTILS.OBJ VCONTROL.OBJ \
	VIDEO.OBJ VTABLE.OBJ WINTOOLS.OBJ $(OBJS_EXT)
	echo >NUL @<<$(PROJ).CRF
$@ /PAGESIZE:64
y
+BCONTROL.OBJ &
+DIALOG.OBJ &
+DIB.OBJ &
+GCONTROL.OBJ &
+HCONTROL.OBJ &
+HSLIDER.OBJ &
+HTTABLES.OBJ &
+MCI.OBJ &
+MCONTROL.OBJ &
+MEMORY.OBJ &
+OFFSCREN.OBJ &
+PAL.OBJ &
+PARSER.OBJ &
+PHAPP.OBJ &
+SCENE.OBJ &
+SCRIPT.OBJ &
+SOUND.OBJ &
+SPEEDCHK.OBJ &
+SPRITE.OBJ &
+TRANSIT.OBJ &
+UTILS.OBJ &
+VCONTROL.OBJ &
+VIDEO.OBJ &
+VTABLE.OBJ &
+WINTOOLS.OBJ &
;
<<
	if exist $@ del $@
	lib @$(PROJ).CRF

$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
