# Microsoft Visual C++ generated build script - Do not modify

PROJ = MEDIT
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\C-SRC\WINDOWS\FD\MEDIT\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MEDIT.C     
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /D "_DOS" /FR /Fd"MEDIT.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G2 /W3 /AL /Ox /D "NDEBUG" /D "_DOS" 
LFLAGS_D_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /CO /STACK:5120
LFLAGS_R_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /STACK:5120
LIBS_D_DEXE = oldnames llibce
LIBS_R_DEXE = oldnames llibce
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = MEASM.OBJ 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = MEDIT.SBR \
		MEMOUSE.SBR \
		MIFF.SBR


MEASM_DEP = 

MEDIT_DEP = e:\c-src\windows\fd\medit\ack3d.h \
	e:\c-src\windows\fd\medit\ackeng.h \
	e:\c-src\windows\fd\medit\mapedit.h


MEMOUSE_DEP = 

MIFF_DEP = e:\c-src\windows\fd\medit\ack3d.h \
	e:\c-src\windows\fd\medit\ackeng.h \
	e:\c-src\windows\fd\medit\iff.h


all:	$(PROJ).EXE

MEDIT.OBJ:	MEDIT.C $(MEDIT_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c MEDIT.C

MEMOUSE.OBJ:	MEMOUSE.C $(MEMOUSE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MEMOUSE.C

MIFF.OBJ:	MIFF.C $(MIFF_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MIFF.C

$(PROJ).EXE::	MEDIT.OBJ MEMOUSE.OBJ MIFF.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MEDIT.OBJ +
MEMOUSE.OBJ +
MIFF.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
d:\msvc\lib\+
c:\msvc\lib\+
d:\msvc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
