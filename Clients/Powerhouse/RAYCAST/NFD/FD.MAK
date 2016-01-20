# Microsoft Visual C++ generated build script - Do not modify

PROJ = FD
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\C-SRC\WINDOWS\FD\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /Ob2 /OV9 /D "_DEBUG" /FR /GA /Fd"FD.PDB" 
CFLAGS_R_WEXE = /nologo /Gs /G3 /W3 /AL /O2 /Ob2 /OV9 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /MAP /LINE  
LIBS_D_WEXE = lafxcwd oldnames libw llibcew wing commdlg.lib mmsystem.lib shell.lib 
LIBS_R_WEXE = lafxcw oldnames libw llibcew wing commdlg.lib mmsystem.lib shell.lib 
RCFLAGS = /nologo /z 
RESFLAGS = /nologo /t 
RUNFLAGS = 
DEFFILE = FD.DEF
OBJS_EXT = ACK\ACKASM.OBJ 
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
SBRS = STDAFX.SBR \
		FDDOC.SBR \
		FDMM.SBR \
		FDSTATUS.SBR \
		FDUSER.SBR \
		FDVIEW.SBR \
		MAINFRM.SBR \
		FD.SBR \
		TBLT.SBR \
		UTILS.SBR \
		ACKBKGD.SBR \
		ACKDATA.SBR \
		ACKDOOR.SBR \
		ACKIFF.SBR \
		ACKINIT.SBR \
		ACKLDBMP.SBR \
		ACKPOV.SBR \
		ACKUTIL.SBR \
		ACKVIEW.SBR \
		ACKWRAP.SBR \
		FDMAP.SBR


ACKASM_DEP = 

FD_RCDEP = e:\c-src\windows\fd\res\fd.ico \
	e:\c-src\windows\fd\res\fd.rc2


STDAFX_DEP = e:\c-src\windows\fd\stdafx.h


FDDOC_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fdmap.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


FDMM_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\fdmm.h \
	e:\c-src\windows\fd\voldlg.h


FDSTATUS_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\utils.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


FDUSER_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\fdmm.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fdmap.h \
	e:\c-src\windows\fd\utils.h \
	d:\msvc\include\wing.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h \
	e:\c-src\windows\fd\ack\acksqr.cpp


FDVIEW_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\fdmm.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fdmap.h \
	e:\c-src\windows\fd\voldlg.h \
	e:\c-src\windows\fd\utils.h \
	d:\msvc\include\wing.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


MAINFRM_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\mainfrm.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h


FD_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\mainfrm.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\fdstatus.h \
	e:\c-src\windows\fd\fdmap.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


TBLT_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	d:\msvc\include\wing.h \
	e:\c-src\windows\fd\utils.h


UTILS_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\utils.h


ACKBKGD_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


ACKDATA_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h


ACKDOOR_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


ACKIFF_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h \
	e:\c-src\windows\fd\ack\iff.h


ACKINIT_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


ACKLDBMP_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


ACKPOV_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h \
	e:\c-src\windows\fd\ack\acksqr.cpp \
	e:\c-src\windows\fd\ack\ackray.cpp


ACKUTIL_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


ACKVIEW_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h \
	e:\c-src\windows\fd\ack\ackvray.cpp \
	e:\c-src\windows\fd\ack\acksqr.cpp


ACKWRAP_DEP = e:\c-src\windows\fd\ack\stdafx.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


FDMAP_DEP = e:\c-src\windows\fd\stdafx.h \
	e:\c-src\windows\fd\fd.h \
	e:\c-src\windows\fd\fdmap.h \
	e:\c-src\windows\fd\fddoc.h \
	e:\c-src\windows\fd\fdview.h \
	e:\c-src\windows\fd\utils.h \
	e:\c-src\windows\fd\ack\ack3d.h \
	e:\c-src\windows\fd\ack\ackeng.h \
	e:\c-src\windows\fd\ack\ackext.h


all:	$(PROJ).EXE $(PROJ).BSC

FD.RES:	FD.RC $(FD_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r FD.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

FDDOC.OBJ:	FDDOC.CPP $(FDDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDDOC.CPP

FDMM.OBJ:	FDMM.CPP $(FDMM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDMM.CPP

FDSTATUS.OBJ:	FDSTATUS.CPP $(FDSTATUS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDSTATUS.CPP

FDUSER.OBJ:	FDUSER.CPP $(FDUSER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDUSER.CPP

FDVIEW.OBJ:	FDVIEW.CPP $(FDVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDVIEW.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

FD.OBJ:	FD.CPP $(FD_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FD.CPP

TBLT.OBJ:	TBLT.CPP $(TBLT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TBLT.CPP

UTILS.OBJ:	UTILS.CPP $(UTILS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTILS.CPP

ACKBKGD.OBJ:	ACK\ACKBKGD.CPP $(ACKBKGD_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKBKGD.CPP

ACKDATA.OBJ:	ACK\ACKDATA.CPP $(ACKDATA_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKDATA.CPP

ACKDOOR.OBJ:	ACK\ACKDOOR.CPP $(ACKDOOR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKDOOR.CPP

ACKIFF.OBJ:	ACK\ACKIFF.CPP $(ACKIFF_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKIFF.CPP

ACKINIT.OBJ:	ACK\ACKINIT.CPP $(ACKINIT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKINIT.CPP

ACKLDBMP.OBJ:	ACK\ACKLDBMP.CPP $(ACKLDBMP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKLDBMP.CPP

ACKPOV.OBJ:	ACK\ACKPOV.CPP $(ACKPOV_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKPOV.CPP

ACKUTIL.OBJ:	ACK\ACKUTIL.CPP $(ACKUTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKUTIL.CPP

ACKVIEW.OBJ:	ACK\ACKVIEW.CPP $(ACKVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKVIEW.CPP

ACKWRAP.OBJ:	ACK\ACKWRAP.CPP $(ACKWRAP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c ACK\ACKWRAP.CPP

FDMAP.OBJ:	FDMAP.CPP $(FDMAP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FDMAP.CPP


$(PROJ).EXE::	FD.RES

$(PROJ).EXE::	STDAFX.OBJ FDDOC.OBJ FDMM.OBJ FDSTATUS.OBJ FDUSER.OBJ FDVIEW.OBJ MAINFRM.OBJ \
	FD.OBJ TBLT.OBJ UTILS.OBJ ACKBKGD.OBJ ACKDATA.OBJ ACKDOOR.OBJ ACKIFF.OBJ ACKINIT.OBJ \
	ACKLDBMP.OBJ ACKPOV.OBJ ACKUTIL.OBJ ACKVIEW.OBJ ACKWRAP.OBJ FDMAP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
FDDOC.OBJ +
FDMM.OBJ +
FDSTATUS.OBJ +
FDUSER.OBJ +
FDVIEW.OBJ +
MAINFRM.OBJ +
FD.OBJ +
TBLT.OBJ +
UTILS.OBJ +
ACKBKGD.OBJ +
ACKDATA.OBJ +
ACKDOOR.OBJ +
ACKIFF.OBJ +
ACKINIT.OBJ +
ACKLDBMP.OBJ +
ACKPOV.OBJ +
ACKUTIL.OBJ +
ACKVIEW.OBJ +
ACKWRAP.OBJ +
FDMAP.OBJ +
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
	$(RC) $(RESFLAGS) FD.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	FD.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) FD.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
