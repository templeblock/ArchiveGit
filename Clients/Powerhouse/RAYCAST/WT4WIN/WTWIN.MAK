# Microsoft Visual C++ generated build script - Do not modify

PROJ = WTWIN
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\DEV\RAYCAST\WT4WIN\
USEMFC = 1
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
CFLAGS_D_WEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /GA /Fd"WTWIN.PDB"
CFLAGS_R_WEXE = /nologo /W3 /FR /O1 /DNDEBUG /AM /GA
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = mafxcwd oldnames libw commdlg shell olecli olesvr mlibcew
LIBS_R_WEXE = mafxcw oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = WTWIN.DEF
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
SBRS = FRAMEBUF.SBR \
		LIST.SBR \
		TEXTURE.SBR \
		VIEW.SBR \
		WORLD.SBR \
		WT.SBR \
		WTWIN.SBR \
		SLICE-GA.SBR


FRAMEBUF_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\wtmem.h \
	d:\dev\raycast\wt4win\framebuf.h \
	d:\dev\raycast\wt4win\graphics.h


LIST_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\wtmem.h \
	d:\dev\raycast\wt4win\list.h


TEXTURE_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\wtmem.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\texture.h


VIEW_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\fixed.h \
	d:\dev\raycast\wt4win\wtmem.h \
	d:\dev\raycast\wt4win\view.h


WORLD_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\fixed.h \
	d:\dev\raycast\wt4win\wtmem.h \
	d:\dev\raycast\wt4win\texture.h \
	d:\dev\raycast\wt4win\table.h \
	d:\dev\raycast\wt4win\world.h


WT_DEP = d:\dev\raycast\wt4win\wt.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\fixed.h \
	d:\dev\raycast\wt4win\view.h \
	d:\dev\raycast\wt4win\texture.h \
	d:\dev\raycast\wt4win\table.h \
	d:\dev\raycast\wt4win\world.h \
	d:\dev\raycast\wt4win\worldfil.h \
	d:\dev\raycast\wt4win\framebuf.h \
	d:\dev\raycast\wt4win\render.h \
	d:\dev\raycast\wt4win\graphics.h \
	d:\dev\raycast\wt4win\input.h


WTWIN_DEP = d:\dev\raycast\wt4win\wing.h \
	d:\dev\raycast\wt4win\wingdll.h \
	d:\dev\raycast\wt4win\wtwin.h \
	d:\dev\raycast\wt4win\error.h \
	d:\dev\raycast\wt4win\dos-keyb.h


SLICE-GA_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

FRAMEBUF.OBJ:	FRAMEBUF.C $(FRAMEBUF_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FRAMEBUF.C

LIST.OBJ:	LIST.C $(LIST_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIST.C

TEXTURE.OBJ:	TEXTURE.C $(TEXTURE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TEXTURE.C

VIEW.OBJ:	VIEW.C $(VIEW_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c VIEW.C

WORLD.OBJ:	WORLD.C $(WORLD_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WORLD.C

WT.OBJ:	WT.C $(WT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WT.C

WTWIN.OBJ:	WTWIN.CPP $(WTWIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WTWIN.CPP

SLICE-GA.OBJ:	SLICE-GA.CPP $(SLICE-GA_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SLICE-GA.CPP


$(PROJ).EXE::	FRAMEBUF.OBJ LIST.OBJ TEXTURE.OBJ VIEW.OBJ WORLD.OBJ WT.OBJ WTWIN.OBJ \
	SLICE-GA.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
FRAMEBUF.OBJ +
LIST.OBJ +
TEXTURE.OBJ +
VIEW.OBJ +
WORLD.OBJ +
WT.OBJ +
WTWIN.OBJ +
SLICE-GA.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@


run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
