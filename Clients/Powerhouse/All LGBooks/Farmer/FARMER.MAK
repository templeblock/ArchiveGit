# Microsoft Visual C++ generated build script - Do not modify

PROJ = FARMER
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\VIRTUA~2\LGBOOKS.SRC\FARMER\
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
CFLAGS_D_WEXE = /nologo /f- /G3 /Gy /W3 /Zi /AM /Od /Ob1 /D "_DEBUG" /D "STRICT" /D "USE_WINTOON" /D "NO_MIDI" /D "NO_CONTROLS" /D "USE_DIBFX" /FR /GA /Fd"DEBUG.PDB"
CFLAGS_R_WEXE = /nologo /f- /G3 /Gy /W3 /AM /Ox /Ob1 /D "NDEBUG" /D "STRICT" /D "USE_WINTOON" /D "NO_MIDI" /D "NO_CONTROLS" /D "USE_DIBFX" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO /nopackcode
LFLAGS_R_WEXE = /NOLOGO /NOD /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /nopackcode
LIBS_D_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
LIBS_R_WEXE = libw mlibcew oldnames commdlg.lib mmsystem.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo /k
RUNFLAGS = 
DEFFILE = ..\GAMEGUTS\POWER.DEF
OBJS_EXT = 
LIBS_EXT = ..\GAMEGUTS\WING.LIB ..\GAMEGUTS\WINTOON.LIB ..\GAMEGUTS\SLIBDIB.LIB ..\GAMEGUTS\VFW.LIB \
	..\GAMEGUTS\WAVMIX16.LIB ..\GAMEGUTS\GAMEGUTS.LIB 
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
SBRS = TOONCTRL.SBR \
		BELONGS.SBR \
		CREDITS.SBR \
		LGBOOK.SBR \
		FARMER.SBR \
		HIDESEEK.SBR \
		CLEANUP.SBR \
		SOUNDOFF.SBR \
		COLORBOK.SBR \
		OPTIONS.SBR \
		SOUNDBOX.SBR \
		MIXUP.SBR


WING_DEP = 

WINTOON_DEP = 

SLIBDIB_DEP = 

VFW_DEP = 

WAVMIX16_DEP = 

GAMEGUTS_DEP = 

TOONCTRL_DEP = d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


BELONGS_DEP = d:\virtua~2\lgbooks.src\lgbook\belongs.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h \
	d:\virtua~2\lgbooks.src\lgbook\belondef.h


CREDITS_DEP = d:\virtua~2\lgbooks.src\lgbook\credits.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


LGBOOK_DEP = d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h \
	d:\virtua~2\lgbooks.src\lgbook\belongs.h \
	d:\virtua~2\lgbooks.src\lgbook\credits.h \
	d:\virtua~2\lgbooks.src\lgbook\options.h \
	d:\virtua~2\lgbooks.src\lgbook\colorbok.h \
	d:\virtua~2\lgbooks.src\lgbook\soundbox.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbres.h


FARMER_DEP = d:\virtua~2\lgbooks.src\farmer\farmer.h \
	d:\virtua~2\lgbooks.src\farmer\farmerid.h \
	d:\virtua~2\lgbooks.src\farmer\hideseek.h \
	d:\virtua~2\lgbooks.src\farmer\soundoff.h \
	d:\virtua~2\lgbooks.src\farmer\cleanup.h


HIDESEEK_DEP = d:\virtua~2\lgbooks.src\farmer\farmer.h \
	d:\virtua~2\lgbooks.src\farmer\farmerid.h \
	d:\virtua~2\lgbooks.src\farmer\hideseek.h \
	d:\virtua~2\lgbooks.src\farmer\hidedef.h


FARMER_RCDEP = d:\virtua~2\lgbooks.src\farmer\farmerid.h \
	d:\virtua~2\lgbooks.src\farmer\mixup.rc \
	d:\virtua~2\lgbooks.src\farmer\cleanup.rc \
	d:\virtua~2\lgbooks.src\farmer\hideseek.rc \
	d:\virtua~2\lgbooks.src\farmer\soundoff.rc


CLEANUP_DEP = d:\virtua~2\lgbooks.src\farmer\farmer.h \
	d:\virtua~2\lgbooks.src\farmer\farmerid.h \
	d:\virtua~2\lgbooks.src\farmer\cleanup.h \
	d:\virtua~2\lgbooks.src\farmer\cleandef.h


SOUNDOFF_DEP = d:\virtua~2\lgbooks.src\farmer\farmer.h \
	d:\virtua~2\lgbooks.src\farmer\farmerid.h \
	d:\virtua~2\lgbooks.src\farmer\soundoff.h \
	d:\virtua~2\lgbooks.src\farmer\sounddef.h


COLORBOK_DEP = d:\virtua~2\lgbooks.src\lgbook\colorbok.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h \
	d:\virtua~2\lgbooks.src\lgbook\colordef.h


OPTIONS_DEP = d:\virtua~2\lgbooks.src\lgbook\options.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


SOUNDBOX_DEP = d:\virtua~2\lgbooks.src\lgbook\soundbox.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


MIXUP_DEP = d:\virtua~2\lgbooks.src\farmer\farmerid.h


all:	$(PROJ).EXE $(PROJ).BSC

TOONCTRL.OBJ:	..\LGBOOK\TOONCTRL.CPP $(TOONCTRL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\TOONCTRL.CPP

BELONGS.OBJ:	..\LGBOOK\BELONGS.CPP $(BELONGS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\BELONGS.CPP

CREDITS.OBJ:	..\LGBOOK\CREDITS.CPP $(CREDITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\CREDITS.CPP

LGBOOK.OBJ:	..\LGBOOK\LGBOOK.CPP $(LGBOOK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\LGBOOK.CPP

FARMER.OBJ:	FARMER.CPP $(FARMER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FARMER.CPP

HIDESEEK.OBJ:	HIDESEEK.CPP $(HIDESEEK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c HIDESEEK.CPP

FARMER.RES:	FARMER.RC $(FARMER_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r FARMER.RC

CLEANUP.OBJ:	CLEANUP.CPP $(CLEANUP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CLEANUP.CPP

SOUNDOFF.OBJ:	SOUNDOFF.CPP $(SOUNDOFF_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SOUNDOFF.CPP

COLORBOK.OBJ:	..\LGBOOK\COLORBOK.CPP $(COLORBOK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\COLORBOK.CPP

OPTIONS.OBJ:	..\LGBOOK\OPTIONS.CPP $(OPTIONS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\OPTIONS.CPP

SOUNDBOX.OBJ:	..\LGBOOK\SOUNDBOX.CPP $(SOUNDBOX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\SOUNDBOX.CPP

MIXUP.OBJ:	MIXUP.CPP $(MIXUP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MIXUP.CPP


$(PROJ).EXE::	FARMER.RES

$(PROJ).EXE::	TOONCTRL.OBJ BELONGS.OBJ CREDITS.OBJ LGBOOK.OBJ FARMER.OBJ HIDESEEK.OBJ \
	CLEANUP.OBJ SOUNDOFF.OBJ COLORBOK.OBJ OPTIONS.OBJ SOUNDBOX.OBJ MIXUP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
TOONCTRL.OBJ +
BELONGS.OBJ +
CREDITS.OBJ +
LGBOOK.OBJ +
FARMER.OBJ +
HIDESEEK.OBJ +
CLEANUP.OBJ +
SOUNDOFF.OBJ +
COLORBOK.OBJ +
OPTIONS.OBJ +
SOUNDBOX.OBJ +
MIXUP.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
..\GAMEGUTS\WING.LIB+
..\GAMEGUTS\WINTOON.LIB+
..\GAMEGUTS\SLIBDIB.LIB+
..\GAMEGUTS\VFW.LIB+
..\GAMEGUTS\WAVMIX16.LIB+
..\GAMEGUTS\GAMEGUTS.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) FARMER.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	FARMER.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) FARMER.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
