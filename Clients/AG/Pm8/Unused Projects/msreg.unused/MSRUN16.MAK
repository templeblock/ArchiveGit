# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSRUN16
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\MSREG\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = DRIVEMGR.CPP
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /GA /Fd"MSRUN16.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G3 /Gy /W3 /Gf /AL /O1 /D "NDEBUG" /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /NOE /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO /MAP  
LFLAGS_R_WEXE = /NOLOGO /NOD /NOE /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /MAP  
LIBS_D_WEXE = lafxcwd oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = lafxcw oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MSRUN16.DEF
OBJS_EXT = 
LIBS_EXT = 
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
SBRS = DRIVEMGR.SBR \
		MSRUN.SBR \
		GLOBATOM.SBR


DRIVEMGR_DEP = c:\msreg\drivemgr.h


MSRUN_DEP = c:\msreg\msregini.h \
	c:\msreg\drivemgr.h \
	c:\msreg\globatom.h


MSRUN_RCDEP = c:\msreg\res\treasure.ico \
	c:\msreg\res\newprod.ico


GLOBATOM_DEP = c:\msreg\globatom.h


all:	$(PROJ).EXE

DRIVEMGR.OBJ:	DRIVEMGR.CPP $(DRIVEMGR_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c DRIVEMGR.CPP

MSRUN.OBJ:	MSRUN.CPP $(MSRUN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSRUN.CPP

MSRUN.RES:	MSRUN.RC $(MSRUN_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MSRUN.RC

GLOBATOM.OBJ:	GLOBATOM.CPP $(GLOBATOM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GLOBATOM.CPP


$(PROJ).EXE::	MSRUN.RES

$(PROJ).EXE::	DRIVEMGR.OBJ MSRUN.OBJ GLOBATOM.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DRIVEMGR.OBJ +
MSRUN.OBJ +
GLOBATOM.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
v:\msvc153\lib\+
v:\msvc153\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MSRUN.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	MSRUN.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MSRUN.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
