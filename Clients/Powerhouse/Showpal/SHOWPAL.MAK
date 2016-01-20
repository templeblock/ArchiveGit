# Microsoft Visual C++ generated build script - Do not modify

PROJ = SHOWPAL
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = C:\ANIM32\SHOWPAL\ 
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
CFLAGS_D_WEXE32 = /nologo /W3 /Zi /D "_X86_" /D "_DEBUG" /D "_WINDOWS" /FR /ML /Fd"SHOWPAL.PDB" /Fp"SHOWPAL.PCH"
CFLAGS_R_WEXE32 = /nologo /W3 /Ox /D "_X86_" /D "NDEBUG" /D "_WINDOWS" /ML /Fp"SHOWPAL.PCH"
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows nafxcwd.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows nafxcw.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG
R_RCDEFINES32 = -dNDEBUG
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = STDAFX.SBR \
		SHOWPAL.SBR \
		MAINFRM.SBR \
		SPDOC.SBR \
		SPVIEW.SBR \
		DIB.SBR \
		DIBPAL.SBR


SHOWPAL_RCDEP =  \
	c:\anim32\showpal\res\showpal.ico \
	c:\anim32\showpal\res\showpal.rc2


STDAFX_DEP =  \
	c:\anim32\showpal\stdafx.h


SHOWPAL_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\showpal.h \
	c:\anim32\showpal\mainfrm.h \
	c:\anim32\showpal\dib.h \
	c:\anim32\showpal\dibpal.h \
	c:\anim32\showpal\spdoc.h \
	c:\anim32\showpal\spview.h


MAINFRM_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\showpal.h \
	c:\anim32\showpal\mainfrm.h


SPDOC_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\showpal.h \
	c:\anim32\showpal\dib.h \
	c:\anim32\showpal\dibpal.h \
	c:\anim32\showpal\spdoc.h


SPVIEW_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\showpal.h \
	c:\anim32\showpal\dib.h \
	c:\anim32\showpal\dibpal.h \
	c:\anim32\showpal\spdoc.h \
	c:\anim32\showpal\spview.h


DIB_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\dib.h


DIBPAL_DEP =  \
	c:\anim32\showpal\stdafx.h \
	c:\anim32\showpal\dibpal.h \
	c:\anim32\showpal\dib.h


all:	$(PROJ).EXE

SHOWPAL.RES:	SHOWPAL.RC $(SHOWPAL_RCDEP)
	$(RC) $(RCFLAGS32) $(RCDEFINES) -r SHOWPAL.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

SHOWPAL.OBJ:	SHOWPAL.CPP $(SHOWPAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SHOWPAL.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

SPDOC.OBJ:	SPDOC.CPP $(SPDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPDOC.CPP

SPVIEW.OBJ:	SPVIEW.CPP $(SPVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPVIEW.CPP

DIB.OBJ:	DIB.CPP $(DIB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIB.CPP

DIBPAL.OBJ:	DIBPAL.CPP $(DIBPAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIBPAL.CPP


$(PROJ).EXE:	SHOWPAL.RES

$(PROJ).EXE:	STDAFX.OBJ SHOWPAL.OBJ MAINFRM.OBJ SPDOC.OBJ SPVIEW.OBJ DIB.OBJ DIBPAL.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ 
SHOWPAL.OBJ 
MAINFRM.OBJ 
SPDOC.OBJ 
SPVIEW.OBJ 
DIB.OBJ 
DIBPAL.OBJ 
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
SHOWPAL.RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
