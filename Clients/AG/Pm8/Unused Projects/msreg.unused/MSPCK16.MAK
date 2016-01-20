# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSPCK16
DEBUG = 0
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\MSREG\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBUG" /D "_USRDLL" /GD /Fd"MSPCK16.PDB" 
CFLAGS_R_WDLL = /nologo /G3 /W3 /ALw /O1 /D "NDEBUG" /D "_USRDLL" /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP /MAP:FULL
LIBS_D_WDLL = lafxdwd oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = lafxdw oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MSPCK16.DEF
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
SBRS = STDAFX.SBR \
		MSPCKDLL.SBR \
		MSPACKET.SBR


STDAFX_DEP = d:\msreg\stdafx.h


MSPCKDLL_DEP = d:\msreg\stdafx.h \
	d:\msreg\mspacket.h \
	d:\msreg\mspckdll.h \
	d:\msreg\inifile.h


MSPACKET_DEP = d:\msreg\stdafx.h \
	d:\msreg\mspacket.h


all:	$(PROJ).DLL

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

MSPCKDLL.OBJ:	MSPCKDLL.CPP $(MSPCKDLL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSPCKDLL.CPP

MSPACKET.OBJ:	MSPACKET.CPP $(MSPACKET_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSPACKET.CPP


$(PROJ).DLL::	STDAFX.OBJ MSPCKDLL.OBJ MSPACKET.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
MSPCKDLL.OBJ +
MSPACKET.OBJ +
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
