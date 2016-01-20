# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSREGR16
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
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = MSREGR.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /YX /Od /D "_DEBUG" /GA /Fd"MSREGR16.PDB" /Fp"MSREGR16.PCH"
CFLAGS_R_WEXE = /nologo /G3 /Gy /W3 /Gf /AM /YX /O1 /D "NDEBUG" /GA /Fp"MSREGR16.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /NOE /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /NOE /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MSREGR16.DEF
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
SBRS = MSREGR.SBR


MSREGR_DEP = c:\msreg\msreg.h \
	c:\msreg\msregent.h \
	c:\msreg\msregini.h


MSREGR_RCDEP = c:\msreg\res\msregr.ico


all:	$(PROJ).EXE

MSREGR.OBJ:	MSREGR.CPP $(MSREGR_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MSREGR.CPP

MSREGR.RES:	MSREGR.RC $(MSREGR_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MSREGR.RC


$(PROJ).EXE::	MSREGR.RES

$(PROJ).EXE::	MSREGR.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MSREGR.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
v:\msvc153\lib\+
v:\msvc153\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MSREGR.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	MSREGR.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MSREGR.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
