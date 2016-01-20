# Microsoft Visual C++ generated build script - Do not modify

PROJ = MINERVML
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\PDK\SAMPLES\MINERVA\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MINERVA.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /Zp1 /W4 /WX /Zi /AL /Gt4096 /Od /D "_DEBUG" /I "..\..\include" /GA /Fd"MINERVA.PDB"
CFLAGS_R_WEXE = /nologo /G2 /Zp1 /W4 /WX /AL /Gt4096 /O1 /D "NDEBUG" /I "..\..\include" /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:4096 /ALIGN:16 /ONERROR:NOEXE /CO /MAP  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:4096 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = ..\..\lib\picw oldnames libw llibcew ctl3ds commdlg.lib shell.lib ver.lib 
LIBS_R_WEXE = ..\..\lib\picw oldnames libw llibcew ctl3ds commdlg.lib shell.lib ver.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = MINERVA.DEF
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
SBRS = MINERVA.SBR \
		MERROR.SBR \
		MFRAME.SBR \
		MCHILD.SBR \
		MMENU.SBR \
		MMISC.SBR \
		MOPEN.SBR \
		MOSEQJPG.SBR \
		MOBMP.SBR \
		MOIM4.SBR \
		MOPRGJPG.SBR \
		MOUTIL81.SBR \
		MTUTIL82.SBR \
		MXZOOM.SBR \
		MSAVE.SBR \
		MXROTATE.SBR \
		MSSEQJPG.SBR \
		MSPRGJPG.SBR \
		MSUTIL80.SBR \
		MXP2S.SBR \
		MXS2P.SBR \
		MOIMSTAR.SBR \
		MSIMSTAR.SBR \
		MDLG.SBR \
		MMRU.SBR \
		MABOUT.SBR \
		MCTRL.SBR \
		MOWAVLET.SBR \
		MSWAVLET.SBR \
		MOPNG.SBR


MINERVA_DEP = c:\pdk\samples\minerva\ctl3d.h \
	c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mframe.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmru.h \
	c:\pdk\samples\minerva\mmisc.h


MINERVA_RCDEP = c:\pdk\samples\minerva\ctl3d.h \
	c:\pdk\samples\minerva\res\mframe.ico \
	c:\pdk\samples\minerva\res\mchild.ico \
	c:\pdk\samples\minerva\res\pegasus.ico \
	c:\pdk\samples\minerva\res\h_move.cur


MERROR_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h


MFRAME_DEP = c:\pdk\samples\minerva\ctl3d.h \
	c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mframe.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h


MCHILD_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h \
	c:\pdk\samples\minerva\mopen.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\msave.h \
	c:\pdk\samples\minerva\mmisc.h


MMENU_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mopen.h \
	c:\pdk\samples\minerva\msave.h \
	c:\pdk\samples\minerva\mabout.h \
	c:\pdk\samples\minerva\mmisc.h


MMISC_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mdlg.h


MOPEN_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mopen.h \
	c:\pdk\samples\minerva\mmru.h \
	c:\pdk\samples\minerva\mmisc.h


MOSEQJPG_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h


MOBMP_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mdlg.h


MOIM4_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h


MOPRGJPG_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h


MOUTIL81_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mchild.h


MTUTIL82_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mframe.h


MXZOOM_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MSAVE_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\msave.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mopen.h


MXROTATE_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MSSEQJPG_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MSPRGJPG_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h


MSUTIL80_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MXP2S_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h


MXS2P_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h


MOIMSTAR_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h


MSIMSTAR_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MDLG_DEP = c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\minerva.h


MMRU_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmru.h \
	c:\pdk\samples\minerva\mmisc.h


MABOUT_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mmenu.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mabout.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mdlg.h


MCTRL_DEP = c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h


MOWAVLET_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h


MSWAVLET_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h \
	c:\pdk\samples\minerva\mopen.h


MOPNG_DEP = c:\pdk\samples\minerva\mopcodes.h \
	c:\pdk\samples\minerva\mctrl.h \
	c:\pdk\samples\minerva\mdlg.h \
	c:\pdk\samples\minerva\merror.h \
	c:\pdk\samples\minerva\minerva.h \
	c:\pdk\samples\minerva\mmisc.h \
	c:\pdk\samples\minerva\mchild.h


all:	$(PROJ).EXE

MINERVA.OBJ:	MINERVA.C $(MINERVA_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c MINERVA.C

MINERVA.RES:	MINERVA.RC $(MINERVA_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MINERVA.RC

MERROR.OBJ:	MERROR.C $(MERROR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MERROR.C

MFRAME.OBJ:	MFRAME.C $(MFRAME_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MFRAME.C

MCHILD.OBJ:	MCHILD.C $(MCHILD_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MCHILD.C

MMENU.OBJ:	MMENU.C $(MMENU_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MMENU.C

MMISC.OBJ:	MMISC.C $(MMISC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MMISC.C

MOPEN.OBJ:	MOPEN.C $(MOPEN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOPEN.C

MOSEQJPG.OBJ:	MOSEQJPG.C $(MOSEQJPG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOSEQJPG.C

MOBMP.OBJ:	MOBMP.C $(MOBMP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOBMP.C

MOIM4.OBJ:	MOIM4.C $(MOIM4_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOIM4.C

MOPRGJPG.OBJ:	MOPRGJPG.C $(MOPRGJPG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOPRGJPG.C

MOUTIL81.OBJ:	MOUTIL81.C $(MOUTIL81_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOUTIL81.C

MTUTIL82.OBJ:	MTUTIL82.C $(MTUTIL82_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MTUTIL82.C

MXZOOM.OBJ:	MXZOOM.C $(MXZOOM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MXZOOM.C

MSAVE.OBJ:	MSAVE.C $(MSAVE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSAVE.C

MXROTATE.OBJ:	MXROTATE.C $(MXROTATE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MXROTATE.C

MSSEQJPG.OBJ:	MSSEQJPG.C $(MSSEQJPG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSSEQJPG.C

MSPRGJPG.OBJ:	MSPRGJPG.C $(MSPRGJPG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSPRGJPG.C

MSUTIL80.OBJ:	MSUTIL80.C $(MSUTIL80_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSUTIL80.C

MXP2S.OBJ:	MXP2S.C $(MXP2S_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MXP2S.C

MXS2P.OBJ:	MXS2P.C $(MXS2P_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MXS2P.C

MOIMSTAR.OBJ:	MOIMSTAR.C $(MOIMSTAR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOIMSTAR.C

MSIMSTAR.OBJ:	MSIMSTAR.C $(MSIMSTAR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSIMSTAR.C

MDLG.OBJ:	MDLG.C $(MDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MDLG.C

MMRU.OBJ:	MMRU.C $(MMRU_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MMRU.C

MABOUT.OBJ:	MABOUT.C $(MABOUT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MABOUT.C

MCTRL.OBJ:	MCTRL.C $(MCTRL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MCTRL.C

MOWAVLET.OBJ:	MOWAVLET.C $(MOWAVLET_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOWAVLET.C

MSWAVLET.OBJ:	MSWAVLET.C $(MSWAVLET_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MSWAVLET.C

MOPNG.OBJ:	MOPNG.C $(MOPNG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MOPNG.C


$(PROJ).EXE::	MINERVA.RES

$(PROJ).EXE::	MINERVA.OBJ MERROR.OBJ MFRAME.OBJ MCHILD.OBJ MMENU.OBJ MMISC.OBJ MOPEN.OBJ \
	MOSEQJPG.OBJ MOBMP.OBJ MOIM4.OBJ MOPRGJPG.OBJ MOUTIL81.OBJ MTUTIL82.OBJ MXZOOM.OBJ \
	MSAVE.OBJ MXROTATE.OBJ MSSEQJPG.OBJ MSPRGJPG.OBJ MSUTIL80.OBJ MXP2S.OBJ MXS2P.OBJ \
	MOIMSTAR.OBJ MSIMSTAR.OBJ MDLG.OBJ MMRU.OBJ MABOUT.OBJ MCTRL.OBJ MOWAVLET.OBJ MSWAVLET.OBJ \
	MOPNG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MINERVA.OBJ +
MERROR.OBJ +
MFRAME.OBJ +
MCHILD.OBJ +
MMENU.OBJ +
MMISC.OBJ +
MOPEN.OBJ +
MOSEQJPG.OBJ +
MOBMP.OBJ +
MOIM4.OBJ +
MOPRGJPG.OBJ +
MOUTIL81.OBJ +
MTUTIL82.OBJ +
MXZOOM.OBJ +
MSAVE.OBJ +
MXROTATE.OBJ +
MSSEQJPG.OBJ +
MSPRGJPG.OBJ +
MSUTIL80.OBJ +
MXP2S.OBJ +
MXS2P.OBJ +
MOIMSTAR.OBJ +
MSIMSTAR.OBJ +
MDLG.OBJ +
MMRU.OBJ +
MABOUT.OBJ +
MCTRL.OBJ +
MOWAVLET.OBJ +
MSWAVLET.OBJ +
MOPNG.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
f:\msvc\lib\+
f:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MINERVA.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	MINERVA.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MINERVA.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
