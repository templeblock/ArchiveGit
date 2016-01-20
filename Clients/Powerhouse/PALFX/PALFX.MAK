# Microsoft Visual C++ generated build script - Do not modify

PROJ = PALFX
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\DEV\PALFX\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /GA /Fd"PALFX.PDB"
CFLAGS_R_WEXE = /nologo /W3 /FR /O1 /DNDEBUG /AM /GA
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = PALFX.DEF
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
SBRS = DIB.SBR \
		PALFX.SBR


DIB_DEP = d:\dev\palfx\gmem.h \
	d:\dev\palfx\dib.h


PALFX_DEP = d:\dev\palfx\palfx.h \
	d:\dev\palfx\dib.h \
	d:\dev\palfx\gmem.h


PALFX_RCDEP = d:\dev\palfx\palfx.h \
	d:\dev\palfx\palfx.rcv \
	d:\dev\palfx\palfx.ico


all:	$(PROJ).EXE $(PROJ).BSC

DIB.OBJ:	DIB.C $(DIB_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DIB.C

PALFX.OBJ:	PALFX.C $(PALFX_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PALFX.C

PALFX.RES:	PALFX.RC $(PALFX_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r PALFX.RC


$(PROJ).EXE::	PALFX.RES

$(PROJ).EXE::	DIB.OBJ PALFX.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DIB.OBJ +
PALFX.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) PALFX.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	PALFX.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) PALFX.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
