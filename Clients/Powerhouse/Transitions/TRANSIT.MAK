# Microsoft Visual C++ generated build script - Do not modify

PROJ = TRANSIT
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\DEV\SRC\TRANSIT\
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
CFLAGS_D_WEXE = /nologo /f- /G3 /Gy /W3 /Zi /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /I "..\..\gameguts" /FR /GA /Fd"DEBUG.PDB"
CFLAGS_R_WEXE = /nologo /f- /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /I "..\..\gameguts" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
LIBS_R_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo /k
RUNFLAGS = 
DEFFILE = ..\GAMEGUTS\POWER.DEF
OBJS_EXT = 
LIBS_EXT = ..\GAMEGUTS\WAVEMIX.LIB ..\GAMEGUTS\WING.LIB ..\GAMEGUTS\VFW.LIB 
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
SBRS = BCONTROL.SBR \
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
		VTABLE.SBR \
		GCONTROL.SBR \
		PARSER.SBR \
		MAIN.SBR \
		MCONTROL.SBR \
		BMULTI.SBR \
		CREDITS.SBR \
		DIBPRINT.SBR \
		MIDINOTE.SBR \
		RANDSEQ.SBR \
		VIDEODRW.SBR


WAVEMIX_DEP = 

WING_DEP = 

VFW_DEP = 

BCONTROL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\wing.h


DIALOG_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


DIB_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\wing.h \
	c:\dev\src\gameguts\dibfx.h


HCONTROL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h


HTTABLES_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\wing.h


MCI_DEP = c:\dev\src\gameguts\digitalv.h \
	c:\dev\src\gameguts\mciavi.h \
	c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


MEMORY_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


OFFSCREN_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\wing.h


PAL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


SCRIPT_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\script.h


SOUND_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


SPEEDCHK_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


SPRITE_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\script.h


UTILS_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


VCONTROL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\commonid.h


VIDEO_DEP = c:\dev\src\gameguts\digitalv.h \
	c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\mciavi.h \
	c:\dev\src\gameguts\vidobsrv.h \
	c:\dev\src\gameguts\vfw.h


WINTOOLS_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\commonid.h \
	c:\dev\src\gameguts\wing.h


TRANSIT_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\transit.h \
	c:\dev\src\gameguts\wing.h \
	c:\dev\src\gameguts\randseq.h


PHAPP_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\wing.h \
	c:\dev\src\gameguts\commonid.h \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\resource.h


SCENE_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h


HSLIDER_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h


VTABLE_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\commonid.h \
	c:\dev\src\gameguts\parser.h


GCONTROL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h


PARSER_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\parser.h


TRANSIT_RCDEP = c:\dev\src\transit\mainid.h


MAIN_DEP = c:\dev\src\transit\main.h \
	c:\dev\src\transit\mainid.h


MCONTROL_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\wing.h


BMULTI_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h


CREDITS_DEP = c:\dev\src\gameguts\credits.h \
	c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\parser.h


DIBPRINT_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p


MIDINOTE_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\midinote.p


RANDSEQ_DEP = c:\dev\src\gameguts\randseq.h


VIDEODRW_DEP = c:\dev\src\gameguts\proto.h \
	c:\dev\src\gameguts\phtypes.h \
	c:\dev\src\gameguts\dib.h \
	c:\dev\src\gameguts\dibid.h \
	c:\dev\src\gameguts\offscren.h \
	c:\dev\src\gameguts\macros.h \
	c:\dev\src\gameguts\port.h \
	c:\dev\src\gameguts\vtable.h \
	c:\dev\src\gameguts\scene.h \
	c:\dev\src\gameguts\sprite.h \
	c:\dev\src\gameguts\toon.h \
	c:\dev\src\gameguts\wintoon.h \
	c:\dev\src\gameguts\sound.h \
	c:\dev\src\gameguts\wavemix.h \
	c:\dev\src\gameguts\phapp.h \
	c:\dev\src\gameguts\bcontrol.p \
	c:\dev\src\gameguts\dialog.p \
	c:\dev\src\gameguts\hcontrol.p \
	c:\dev\src\gameguts\httables.p \
	c:\dev\src\gameguts\mci.p \
	c:\dev\src\gameguts\memory.p \
	c:\dev\src\gameguts\pal.p \
	c:\dev\src\gameguts\speedchk.p \
	c:\dev\src\gameguts\transit.p \
	c:\dev\src\gameguts\utils.p \
	c:\dev\src\gameguts\vcontrol.p \
	c:\dev\src\gameguts\video.p \
	c:\dev\src\gameguts\videodrw.p \
	c:\dev\src\gameguts\vtable.p \
	c:\dev\src\gameguts\wintools.p \
	c:\dev\src\gameguts\hslider.p \
	c:\dev\src\gameguts\gcontrol.p \
	c:\dev\src\gameguts\mcontrol.p \
	c:\dev\src\gameguts\bmulti.p \
	c:\dev\src\gameguts\control.h \
	c:\dev\src\gameguts\commonid.h


all:	$(PROJ).EXE $(PROJ).BSC

BCONTROL.OBJ:	..\GAMEGUTS\BCONTROL.CPP $(BCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c ..\GAMEGUTS\BCONTROL.CPP

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

VTABLE.OBJ:	..\GAMEGUTS\VTABLE.CPP $(VTABLE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VTABLE.CPP

GCONTROL.OBJ:	..\GAMEGUTS\GCONTROL.CPP $(GCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\GCONTROL.CPP

PARSER.OBJ:	..\GAMEGUTS\PARSER.CPP $(PARSER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\PARSER.CPP

TRANSIT.RES:	TRANSIT.RC $(TRANSIT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r TRANSIT.RC

MAIN.OBJ:	MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAIN.CPP

MCONTROL.OBJ:	..\GAMEGUTS\MCONTROL.CPP $(MCONTROL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MCONTROL.CPP

BMULTI.OBJ:	..\GAMEGUTS\BMULTI.CPP $(BMULTI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\BMULTI.CPP

CREDITS.OBJ:	..\GAMEGUTS\CREDITS.CPP $(CREDITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\CREDITS.CPP

DIBPRINT.OBJ:	..\GAMEGUTS\DIBPRINT.CPP $(DIBPRINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\DIBPRINT.CPP

MIDINOTE.OBJ:	..\GAMEGUTS\MIDINOTE.CPP $(MIDINOTE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MIDINOTE.CPP

RANDSEQ.OBJ:	..\GAMEGUTS\RANDSEQ.CPP $(RANDSEQ_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\RANDSEQ.CPP

VIDEODRW.OBJ:	..\GAMEGUTS\VIDEODRW.CPP $(VIDEODRW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\VIDEODRW.CPP


$(PROJ).EXE::	TRANSIT.RES

$(PROJ).EXE::	BCONTROL.OBJ DIALOG.OBJ DIB.OBJ HCONTROL.OBJ HTTABLES.OBJ MCI.OBJ MEMORY.OBJ \
	OFFSCREN.OBJ PAL.OBJ SCRIPT.OBJ SOUND.OBJ SPEEDCHK.OBJ SPRITE.OBJ UTILS.OBJ VCONTROL.OBJ \
	VIDEO.OBJ WINTOOLS.OBJ TRANSIT.OBJ PHAPP.OBJ SCENE.OBJ HSLIDER.OBJ VTABLE.OBJ GCONTROL.OBJ \
	PARSER.OBJ MAIN.OBJ MCONTROL.OBJ BMULTI.OBJ CREDITS.OBJ DIBPRINT.OBJ MIDINOTE.OBJ \
	RANDSEQ.OBJ VIDEODRW.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
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
VTABLE.OBJ +
GCONTROL.OBJ +
PARSER.OBJ +
MAIN.OBJ +
MCONTROL.OBJ +
BMULTI.OBJ +
CREDITS.OBJ +
DIBPRINT.OBJ +
MIDINOTE.OBJ +
RANDSEQ.OBJ +
VIDEODRW.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\mfc\lib\+
c:\msvc\lib\+
..\GAMEGUTS\WAVEMIX.LIB+
..\GAMEGUTS\WING.LIB+
..\GAMEGUTS\VFW.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) TRANSIT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	TRANSIT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) TRANSIT.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
