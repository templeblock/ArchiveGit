# Microsoft Visual C++ generated build script - Do not modify

PROJ = AVIHOT
DEBUG = 1
PROGTYPE = 2
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\HOTSPOT\VBX\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MAIN.C      
FIRSTCPP =             
RC = rc
CFLAGS_D_VEXE = /nologo /G2 /Gc /Zp1 /W3 /Zi /ALw /Od /D "_DEBUG" /FR /GD /Fd"AVIHOT.PDB"
CFLAGS_R_VEXE = /nologo /Gs /G2 /Gc /Zp1 /W3 /ALw /Ox /D "NDEBUG" /FR /GD 
LFLAGS_D_VEXE = /NOLOGO /NOD /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_VEXE = /NOLOGO /NOD /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_VEXE = oldnames libw ldllcew vbapi vfw commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_VEXE = oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = AVIHOT.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_VEXE)
LFLAGS = $(LFLAGS_D_VEXE)
LIBS = $(LIBS_D_VEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_VEXE)
LFLAGS = $(LFLAGS_R_VEXE)
LIBS = $(LIBS_R_VEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = MAIN.SBR \
		AVI.SBR \
		DEBUG.SBR \
		HOTSPOT.SBR \
		MOVIE.SBR \
		SBCLS.SBR \
		UTIL.SBR \
		DRAWHOT.SBR


MAIN_DEP = c:\hotspot\include\digitalv.h \
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h \
	c:\hotspot\vbx\avivbx.h


HOTSPOT_RCDEP = c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h \
	c:\hotspot\vbx\avivbx.h \
	c:\vb\cdk\vbx.rcv \
	c:\hotspot\vbx\avicu.bmp \
	c:\hotspot\vbx\avicd.bmp \
	c:\hotspot\vbx\avimu.bmp \
	c:\hotspot\vbx\avieu.bmp


AVI_DEP = c:\hotspot\include\digitalv.h \
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


DEBUG_DEP = c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


HOTSPOT_DEP = c:\hotspot\include\digitalv.h \
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


MOVIE_DEP = c:\hotspot\include\digitalv.h \
	c:\vfwdk\inc\mciavi.h \
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


SBCLS_DEP = c:\hotspot\include\digitalv.h \
	c:\vb\cdk\vbapi.h \
	c:\hotspot\vbx\hotspot.h


UTIL_DEP = c:\hotspot\include\digitalv.h \
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


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
	c:\hotspot\vbx\hotspot.h \
	c:\vb\cdk\vbapi.h


all:	$(PROJ).VBX $(PROJ).BSC

MAIN.OBJ:	MAIN.C $(MAIN_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c MAIN.C

HOTSPOT.RES:	HOTSPOT.RC $(HOTSPOT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r HOTSPOT.RC

AVI.OBJ:	AVI.C $(AVI_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c AVI.C

DEBUG.OBJ:	DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DEBUG.C

HOTSPOT.OBJ:	HOTSPOT.C $(HOTSPOT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c HOTSPOT.C

MOVIE.OBJ:	MOVIE.C $(MOVIE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOVIE.C

SBCLS.OBJ:	SBCLS.C $(SBCLS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SBCLS.C

UTIL.OBJ:	UTIL.C $(UTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c UTIL.C

DRAWHOT.OBJ:	DRAWHOT.C $(DRAWHOT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DRAWHOT.C


$(PROJ).VBX::	HOTSPOT.RES

$(PROJ).VBX::	MAIN.OBJ AVI.OBJ DEBUG.OBJ HOTSPOT.OBJ MOVIE.OBJ SBCLS.OBJ UTIL.OBJ \
	DRAWHOT.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MAIN.OBJ +
AVI.OBJ +
DEBUG.OBJ +
HOTSPOT.OBJ +
MOVIE.OBJ +
SBCLS.OBJ +
UTIL.OBJ +
DRAWHOT.OBJ +
$(OBJS_EXT)
$(PROJ).VBX
$(MAPFILE)
c:\msvc\lib\+
c:\vb\cdk\+
c:\vfwdk\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) HOTSPOT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).VBX::	HOTSPOT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) HOTSPOT.RES $@

run: $(PROJ).VBX
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
