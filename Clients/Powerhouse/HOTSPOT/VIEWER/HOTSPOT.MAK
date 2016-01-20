# Microsoft Visual C++ generated build script - Do not modify

PROJ = HOTSPOT
DEBUG = 1
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\HOTSPOT\WINSER\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = HOTSPOT.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /AMu /Od /D "_DEBUG" /Fd"HOTSPOT.PDB"
CFLAGS_R_WDLL = /nologo /W3 /AMw /O1 /D "NDEBUG" /FR /Gw 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw mdllcew mviewer2 vfw commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = oldnames libw mdllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = HOTSPOT.DEF
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
SBRS = HOTSPOT.SBR \
		AVI.SBR \
		HSPPLAY.SBR \
		MOVIE.SBR \
		MAIN.SBR \
		SBCLS.SBR \
		UTIL.SBR \
		DEBUG.SBR \
		DRAWHOT.SBR


HOTSPOT_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


AVI_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


HSPPLAY_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


MOVIE_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


MAIN_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


SBCLS_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


UTIL_DEP = c:\hotspot\include\digitalv.h \
	c:\mvpubkit\mvtools\viewer.h \
	c:\hotspot\winser\hotspot.h


DEBUG_DEP = c:\hotspot\winser\hotspot.h


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
	c:\hotspot\winser\hotspot.h


all:	$(PROJ).DLL

HOTSPOT.OBJ:	HOTSPOT.C $(HOTSPOT_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c HOTSPOT.C

AVI.OBJ:	AVI.C $(AVI_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c AVI.C

HSPPLAY.OBJ:	HSPPLAY.C $(HSPPLAY_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c HSPPLAY.C

MOVIE.OBJ:	MOVIE.C $(MOVIE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOVIE.C

MAIN.OBJ:	MAIN.C $(MAIN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MAIN.C

SBCLS.OBJ:	SBCLS.C $(SBCLS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SBCLS.C

UTIL.OBJ:	UTIL.C $(UTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c UTIL.C

DEBUG.OBJ:	DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DEBUG.C

DRAWHOT.OBJ:	DRAWHOT.C $(DRAWHOT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DRAWHOT.C


$(PROJ).DLL::	HOTSPOT.OBJ AVI.OBJ HSPPLAY.OBJ MOVIE.OBJ MAIN.OBJ SBCLS.OBJ UTIL.OBJ \
	DEBUG.OBJ DRAWHOT.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
HOTSPOT.OBJ +
AVI.OBJ +
HSPPLAY.OBJ +
MOVIE.OBJ +
MAIN.OBJ +
SBCLS.OBJ +
UTIL.OBJ +
DEBUG.OBJ +
DRAWHOT.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
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
	$(RC) $(RESFLAGS) $@
	implib /nowep $(PROJ).LIB $(PROJ).DLL


run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
