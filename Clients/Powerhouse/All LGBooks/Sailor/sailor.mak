# Microsoft Visual C++ generated build script - Do not modify

PROJ = SAILOR
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\VIRTUA~2\LGBOOKS.SRC\SAILOR\
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
LIBS_EXT = ..\GAMEGUTS\WING.LIB ..\GAMEGUTS\WINTOON.LIB ..\GAMEGUTS\WAVMIX16.LIB \
	..\GAMEGUTS\SLIBDIB.LIB ..\GAMEGUTS\VFW.LIB ..\GAMEGUTS\GAMEGUTS.LIB 
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
SBRS = FROGGER.SBR \
		CONNECT.SBR \
		SAILOR.SBR \
		TOONCTRL.SBR \
		BELONGS.SBR \
		CREDITS.SBR \
		LGBOOK.SBR \
		SHELTER.SBR \
		OPTIONS.SBR \
		COLORBOK.SBR \
		MIXUP.SBR \
		SOUNDBOX.SBR


WING_DEP = 

WINTOON_DEP = 

WAVMIX16_DEP = 

SLIBDIB_DEP = 

VFW_DEP = 

GAMEGUTS_DEP = 

FROGGER_DEP = d:\virtua~2\lgbooks.src\sailor\frogger.h \
	d:\virtua~2\lgbooks.src\sailor\sailor.h \
	d:\virtua~2\lgbooks.src\sailor\sailorid.h \
	d:\virtua~2\lgbooks.src\sailor\frogdef.h


CONNECT_DEP = d:\virtua~2\lgbooks.src\sailor\connect.h \
	d:\virtua~2\lgbooks.src\sailor\sailor.h \
	d:\virtua~2\lgbooks.src\sailor\sailorid.h \
	d:\virtua~2\lgbooks.src\sailor\conndef.h


SAILOR_DEP = d:\virtua~2\lgbooks.src\sailor\sailor.h \
	d:\virtua~2\lgbooks.src\sailor\sailorid.h \
	d:\virtua~2\lgbooks.src\sailor\frogger.h \
	d:\virtua~2\lgbooks.src\sailor\shelter.h \
	d:\virtua~2\lgbooks.src\sailor\connect.h


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


SHELTER_DEP = d:\virtua~2\lgbooks.src\sailor\shelter.h \
	d:\virtua~2\lgbooks.src\sailor\sailor.h \
	d:\virtua~2\lgbooks.src\sailor\sailorid.h


SAILOR_RCDEP = d:\virtua~2\lgbooks.src\sailor\sailorid.h \
	d:\virtua~2\lgbooks.src\sailor\dogger.rc \
	d:\virtua~2\lgbooks.src\sailor\house.rc \
	d:\virtua~2\lgbooks.src\sailor\connect.rc \
	d:\virtua~2\lgbooks.src\sailor\dress.rc


OPTIONS_DEP = d:\virtua~2\lgbooks.src\lgbook\options.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


COLORBOK_DEP = d:\virtua~2\lgbooks.src\lgbook\colorbok.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h \
	d:\virtua~2\lgbooks.src\lgbook\colordef.h


MIXUP_DEP = d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h \
	d:\virtua~2\lgbooks.src\lgbook\mixup.h \
	d:\virtua~2\lgbooks.src\lgbook\mixupdef.h


SOUNDBOX_DEP = d:\virtua~2\lgbooks.src\lgbook\soundbox.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbook.h \
	d:\virtua~2\lgbooks.src\lgbook\lgbookid.h \
	d:\virtua~2\lgbooks.src\lgbook\toonctrl.h \
	d:\virtua~2\lgbooks.src\lgbook\toon.h


all:	$(PROJ).EXE $(PROJ).BSC

FROGGER.OBJ:	FROGGER.CPP $(FROGGER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FROGGER.CPP

CONNECT.OBJ:	CONNECT.CPP $(CONNECT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONNECT.CPP

SAILOR.OBJ:	SAILOR.CPP $(SAILOR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SAILOR.CPP

TOONCTRL.OBJ:	..\LGBOOK\TOONCTRL.CPP $(TOONCTRL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\TOONCTRL.CPP

BELONGS.OBJ:	..\LGBOOK\BELONGS.CPP $(BELONGS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\BELONGS.CPP

CREDITS.OBJ:	..\LGBOOK\CREDITS.CPP $(CREDITS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\CREDITS.CPP

LGBOOK.OBJ:	..\LGBOOK\LGBOOK.CPP $(LGBOOK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\LGBOOK.CPP

SHELTER.OBJ:	SHELTER.CPP $(SHELTER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SHELTER.CPP

SAILOR.RES:	SAILOR.RC $(SAILOR_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r SAILOR.RC

OPTIONS.OBJ:	..\LGBOOK\OPTIONS.CPP $(OPTIONS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\OPTIONS.CPP

COLORBOK.OBJ:	..\LGBOOK\COLORBOK.CPP $(COLORBOK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\COLORBOK.CPP

MIXUP.OBJ:	..\LGBOOK\MIXUP.CPP $(MIXUP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\MIXUP.CPP

SOUNDBOX.OBJ:	..\LGBOOK\SOUNDBOX.CPP $(SOUNDBOX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ..\LGBOOK\SOUNDBOX.CPP


$(PROJ).EXE::	SAILOR.RES

$(PROJ).EXE::	FROGGER.OBJ CONNECT.OBJ SAILOR.OBJ TOONCTRL.OBJ BELONGS.OBJ CREDITS.OBJ \
	LGBOOK.OBJ SHELTER.OBJ OPTIONS.OBJ COLORBOK.OBJ MIXUP.OBJ SOUNDBOX.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
FROGGER.OBJ +
CONNECT.OBJ +
SAILOR.OBJ +
TOONCTRL.OBJ +
BELONGS.OBJ +
CREDITS.OBJ +
LGBOOK.OBJ +
SHELTER.OBJ +
OPTIONS.OBJ +
COLORBOK.OBJ +
MIXUP.OBJ +
SOUNDBOX.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
..\GAMEGUTS\WING.LIB+
..\GAMEGUTS\WINTOON.LIB+
..\GAMEGUTS\WAVMIX16.LIB+
..\GAMEGUTS\SLIBDIB.LIB+
..\GAMEGUTS\VFW.LIB+
..\GAMEGUTS\GAMEGUTS.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) SAILOR.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	SAILOR.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) SAILOR.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
