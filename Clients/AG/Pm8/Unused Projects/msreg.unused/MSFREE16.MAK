# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSFREE16
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
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = MSFREE.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /YX /Od /D "_DEBUG" /FR /GA /Fd"MSFREE16.PDB" /Fp"MSFREE16.PCH"
CFLAGS_R_WEXE = /nologo /G3 /Gy /W3 /Gf /AM /YX /O1 /D "NDEBUG" /FR /GA /Fp"MSFREE16.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = mafxcwd oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = mafxcw oldnames libw mlibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MSFREE16.DEF
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
SBRS = MSFREE.SBR \
		CMFADDON.SBR \
		CCTL3D.SBR


MSFREE_DEP = c:\msreg\msfrafx.h \
	c:\msreg\msfree.h \
	c:\msreg\cmfaddon.h \
	c:\msreg\cctl3d.h \
	c:\msreg\pmwmm\enabled.h


CMFADDON_DEP = c:\msreg\msfrafx.h \
	c:\msreg\cmfaddon.h


CCTL3D_DEP = c:\msreg\msfrafx.h \
	c:\msreg\cctl3d.h \
	l:\ctl3d\include\ctl3d.h


MSFREE_RCDEP = c:\msreg\res\msfree.ico


all:	$(PROJ).EXE $(PROJ).BSC

MSFREE.OBJ:	MSFREE.CPP $(MSFREE_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MSFREE.CPP

CMFADDON.OBJ:	CMFADDON.CPP $(CMFADDON_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CMFADDON.CPP

CCTL3D.OBJ:	CCTL3D.CPP $(CCTL3D_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CCTL3D.CPP

MSFREE.RES:	MSFREE.RC $(MSFREE_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MSFREE.RC


$(PROJ).EXE::	MSFREE.RES

$(PROJ).EXE::	MSFREE.OBJ CMFADDON.OBJ CCTL3D.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MSFREE.OBJ +
CMFADDON.OBJ +
CCTL3D.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
v:\msvc153\lib\+
v:\msvc153\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MSFREE.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	MSFREE.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MSFREE.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
