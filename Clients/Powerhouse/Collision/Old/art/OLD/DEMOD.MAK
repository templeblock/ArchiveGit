# Microsoft Visual C++ generated build script - Do not modify

PROJ = DEMOD
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\DEV\POWER\DEMOD\
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
CFLAGS_D_WEXE = /nologo /f- /G3 /Gy /W3 /Zi /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /FR /GA /Fd"DEBUG.PDB"
CFLAGS_R_WEXE = /nologo /f- /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = libw mlibcew oldnames mmsystem.lib 
LIBS_R_WEXE = libw mlibcew oldnames mmsystem.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo /k
RUNFLAGS = 
DEFFILE = ..\GAMEGUTS\POWER.DEF
OBJS_EXT = 
LIBS_EXT = ..\GAMEGUTS\WAVEMIX.LIB ..\GAMEGUTS\WING.LIB 
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
		MAIN.SBR \
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
		DEMOD.SBR


WAVEMIX_DEP = 

WING_DEP = 

BCONTROL_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\control.h \
	d:\dev\power\gameguts\sound.h \
	d:\dev\power\gameguts\wavemix.h


DIALOG_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


DIB_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\wing.h


HCONTROL_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\control.h \
	d:\dev\power\gameguts\sound.h \
	d:\dev\power\gameguts\wavemix.h


HTTABLES_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\wing.h


MAIN_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\sound.h \
	d:\dev\power\gameguts\wavemix.h \
	d:\dev\power\gameguts\sprite.h \
	d:\dev\power\gameguts\wing.h


MCI_DEP = d:\dev\power\gameguts\digitalv.h \
	d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


MEMORY_DEP = d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p


OFFSCREN_DEP = d:\dev\power\gameguts\wing.h \
	d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


PAL_DEP = d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p


SCRIPT_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\sprite.h \
	d:\dev\power\gameguts\script.h


SOUND_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\sound.h \
	d:\dev\power\gameguts\wavemix.h


SPEEDCHK_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


SPRITE_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\sprite.h \
	d:\dev\power\gameguts\script.h


UTILS_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


VCONTROL_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\control.h


VIDEO_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


WINTOOLS_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h


DEMOD_DEP = d:\dev\power\gameguts\main.h \
	d:\dev\power\gameguts\dib.h \
	d:\dev\power\gameguts\proto.h \
	d:\dev\power\gameguts\bcontrol.p \
	d:\dev\power\gameguts\dialog.p \
	d:\dev\power\gameguts\hcontrol.p \
	d:\dev\power\gameguts\httables.p \
	d:\dev\power\gameguts\mci.p \
	d:\dev\power\gameguts\memory.p \
	d:\dev\power\gameguts\pal.p \
	d:\dev\power\gameguts\main.p \
	d:\dev\power\gameguts\speedchk.p \
	d:\dev\power\gameguts\utils.p \
	d:\dev\power\gameguts\vcontrol.p \
	d:\dev\power\gameguts\video.p \
	d:\dev\power\gameguts\wintools.p \
	d:\dev\power\gameguts\title.p \
	d:\dev\power\gameguts\commonid.h \
	d:\dev\power\gameguts\offscren.h \
	d:\dev\power\gameguts\sound.h \
	d:\dev\power\gameguts\wavemix.h \
	d:\dev\power\demod\demod.h \
	d:\dev\power\demod\demod.p


DEMOD_RCDEP = d:\dev\power\demod\resource.h \
	d:\dev\power\demod\commonid.h \
	d:\dev\power\demod\demod.h


all:	$(PROJ).EXE $(PROJ).BSC

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

MAIN.OBJ:	..\GAMEGUTS\MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\GAMEGUTS\MAIN.CPP

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

DEMOD.OBJ:	DEMOD.CPP $(DEMOD_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DEMOD.CPP

DEMOD.RES:	DEMOD.RC $(DEMOD_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r DEMOD.RC


$(PROJ).EXE::	DEMOD.RES

$(PROJ).EXE::	BCONTROL.OBJ DIALOG.OBJ DIB.OBJ HCONTROL.OBJ HTTABLES.OBJ MAIN.OBJ \
	MCI.OBJ MEMORY.OBJ OFFSCREN.OBJ PAL.OBJ SCRIPT.OBJ SOUND.OBJ SPEEDCHK.OBJ SPRITE.OBJ \
	UTILS.OBJ VCONTROL.OBJ VIDEO.OBJ WINTOOLS.OBJ DEMOD.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
BCONTROL.OBJ +
DIALOG.OBJ +
DIB.OBJ +
HCONTROL.OBJ +
HTTABLES.OBJ +
MAIN.OBJ +
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
DEMOD.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\ver15\lib\+
c:\msvc\ver15\mfc\lib\+
..\GAMEGUTS\WAVEMIX.LIB+
..\GAMEGUTS\WING.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) DEMOD.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	DEMOD.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) DEMOD.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
