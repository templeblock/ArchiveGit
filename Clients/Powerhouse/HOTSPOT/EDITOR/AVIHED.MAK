# Microsoft Visual C++ generated build script - Do not modify

PROJ = AVIHED
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\HOTSPOT\EDITOR\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MOVIE.C     
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /FR /GA /Fd"AVIHED.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AMu /O1 /D "DEBUG" /FR 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw mlibcew vfw commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = AVIHED.DEF
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
SBRS = MAIN.SBR \
		SBCLS.SBR \
		FILE.SBR \
		DLG.SBR \
		HOTSPOT.SBR \
		MOVIE.SBR \
		DEBUG.SBR \
		AVI.SBR \
		UTIL.SBR \
		DRAWHOT.SBR


MAIN_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h \
	c:\hotspot\editor\avihed.h


AVIHED_RCDEP = c:\hotspot\editor\icon1.ico \
	c:\hotspot\editor\pattern.bmp


SBCLS_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h \
	c:\hotspot\editor\avihed.h \
	c:\hotspot\editor\avihede.h


FILE_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h \
	c:\hotspot\editor\avihed.h \
	c:\hotspot\editor\avihede.h \
	c:\vfwdk\inc\mciavi.h


DLG_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h \
	c:\hotspot\editor\avihed.h \
	c:\hotspot\editor\avihede.h


HOTSPOT_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h


MOVIE_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h


DEBUG_DEP = c:\hotspot\editor\hotspot.h


AVI_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h


UTIL_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\editor\hotspot.h


DRAWHOT_DEP = c:\vfwdk\inc\vfw.h \
	c:\vfwdk\inc\compman.h \
	c:\vfwdk\inc\compddk.h \
	c:\vfwdk\inc\drawdib.h \
	c:\vfwdk\inc\avifmt.h \
	c:\vfwdk\inc\mmreg.h \
	c:\vfwdk\inc\avifile.h \
	c:\vfwdk\inc\aviiface.h \
	c:\vfwdk\inc\compobj.h \
	c:\vfwdk\inc\scode.h \
	c:\vfwdk\inc\initguid.h \
	c:\vfwdk\inc\coguid.h \
	c:\vfwdk\inc\mciwnd.h \
	c:\vfwdk\inc\msvideo.h \
	c:\vfwdk\inc\avicap.h \
	c:\vfwdk\inc\msacm.h \
	c:\hotspot\editor\hotspot.h


all:	$(PROJ).EXE $(PROJ).BSC

MAIN.OBJ:	MAIN.C $(MAIN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MAIN.C

AVIHED.RES:	AVIHED.RC $(AVIHED_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r AVIHED.RC

SBCLS.OBJ:	SBCLS.C $(SBCLS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SBCLS.C

FILE.OBJ:	FILE.C $(FILE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FILE.C

DLG.OBJ:	DLG.C $(DLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DLG.C

HOTSPOT.OBJ:	HOTSPOT.C $(HOTSPOT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c HOTSPOT.C

MOVIE.OBJ:	MOVIE.C $(MOVIE_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c MOVIE.C

DEBUG.OBJ:	DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DEBUG.C

AVI.OBJ:	AVI.C $(AVI_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c AVI.C

UTIL.OBJ:	UTIL.C $(UTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c UTIL.C

DRAWHOT.OBJ:	DRAWHOT.C $(DRAWHOT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DRAWHOT.C


$(PROJ).EXE::	AVIHED.RES

$(PROJ).EXE::	MAIN.OBJ SBCLS.OBJ FILE.OBJ DLG.OBJ HOTSPOT.OBJ MOVIE.OBJ DEBUG.OBJ \
	AVI.OBJ UTIL.OBJ DRAWHOT.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MAIN.OBJ +
SBCLS.OBJ +
FILE.OBJ +
DLG.OBJ +
HOTSPOT.OBJ +
MOVIE.OBJ +
DEBUG.OBJ +
AVI.OBJ +
UTIL.OBJ +
DRAWHOT.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
c:\vb\cdk\+
c:\mvpubkit\mvtools\+
c:\vfwdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) AVIHED.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	AVIHED.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) AVIHED.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
