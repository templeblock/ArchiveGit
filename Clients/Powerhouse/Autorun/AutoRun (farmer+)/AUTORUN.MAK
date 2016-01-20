# Microsoft Visual C++ generated build script - Do not modify

PROJ = AUTORUN
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = N:\DEV\ARCHIVE\AUTORUN\AU7CF7~5\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = AUTORUN.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /GA /Fd"AUTORUN.PDB"
CFLAGS_R_WEXE = /nologo /W3 /FR /O1 /DNDEBUG /AM /GA
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = AUTORUN.DEF
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
SBRS = AUTORUN.SBR


AUTORUN_DEP = n:\dev\archive\autorun\au7cf7~5\autorun.h


AUTORUN_RCDEP = n:\dev\archive\autorun\au7cf7~5\autorun.h \
	n:\dev\archive\autorun\au7cf7~5\autorun.ico


all:	$(PROJ).EXE $(PROJ).BSC

AUTORUN.OBJ:	AUTORUN.C $(AUTORUN_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c AUTORUN.C

AUTORUN.RES:	AUTORUN.RC $(AUTORUN_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r AUTORUN.RC


$(PROJ).EXE::	AUTORUN.RES

$(PROJ).EXE::	AUTORUN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
AUTORUN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\mfc\lib\+
c:\msvc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) AUTORUN.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	AUTORUN.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) AUTORUN.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
