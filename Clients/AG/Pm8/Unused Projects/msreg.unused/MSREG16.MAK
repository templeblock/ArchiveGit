# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSREG16
DEBUG = 0
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\MSREG\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = MSREGDLL.CPP
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /Gy /W3 /Gf /Zi /ALw /YX /Od /D "_DEBUG" /D "_USRDLL" /GD /Fd"MSREG16.PDB"    /Fp"MSREG16.PCH"
CFLAGS_R_WDLL = /nologo /G3 /Gy /W3 /Gf /ALw /YX /O1 /D "NDEBUG" /D "_USRDLL" /GD /Fp"MSREG16.PCH"
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP /MAP:FULL
LIBS_D_WDLL = gcpp300\msvc1x\_300d.lib oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = gcpp300\msvc1x\_300.lib oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo 
RUNFLAGS = 
DEFFILE = MSREG16.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = MSREGDLL.SBR \
		COMMAND.SBR \
		DRIVEMGR.SBR


MSREGDLL_DEP = d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\msregent.h \
	d:\msreg\command.h \
	d:\msreg\drivemgr.h


COMMAND_DEP = d:\msreg\command.h


DRIVEMGR_DEP = d:\msreg\drivemgr.h


all:	$(PROJ).DLL

MSREGDLL.OBJ:	MSREGDLL.CPP $(MSREGDLL_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MSREGDLL.CPP

COMMAND.OBJ:	COMMAND.CPP $(COMMAND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COMMAND.CPP

DRIVEMGR.OBJ:	DRIVEMGR.CPP $(DRIVEMGR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DRIVEMGR.CPP


$(PROJ).DLL::	MSREGDLL.OBJ COMMAND.OBJ DRIVEMGR.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MSREGDLL.OBJ +
COMMAND.OBJ +
DRIVEMGR.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@
	implib /nowep $(PROJ).LIB $(PROJ).DLL


run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
