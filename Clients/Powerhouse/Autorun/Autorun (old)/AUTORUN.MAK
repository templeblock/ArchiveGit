# Microsoft Visual C++ generated build script - Do not modify

PROJ = AUTORUN
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = jungle     crap.exe    extra
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = N:\DEV\SRC\TOOLS\AUTORU~1\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = AUTORUN.CPP 
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Z7 /AM /Od /D "_DEBUG" /D "STRICT" /FR /GA 
CFLAGS_R_WEXE = /nologo /G2 /W3 /AM /O1 /D "NDEBUG" /D "STRICT" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO 
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE 
LIBS_D_WEXE = oldnames libw mlibcew 
LIBS_R_WEXE = oldnames libw mlibcew 
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


AUTORUN_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

AUTORUN.OBJ:	AUTORUN.CPP $(AUTORUN_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c AUTORUN.CPP

AUTORUN.RES:	AUTORUN.RC $(AUTORUN_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r AUTORUN.RC


$(PROJ).EXE::	AUTORUN.RES

$(PROJ).EXE::	AUTORUN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
AUTORUN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
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
