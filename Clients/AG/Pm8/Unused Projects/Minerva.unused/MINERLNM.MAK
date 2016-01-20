# Microsoft Developer Studio Generated NMAKE File, Based on minerlnm.dsp
!IF "$(CFG)" == ""
CFG=minerlnm - Win32 Release
!MESSAGE No configuration specified. Defaulting to minerlnm - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "minerlnm - Win32 Release" && "$(CFG)" !=\
 "minerlnm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minerlnm.mak" CFG="minerlnm - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minerlnm - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "minerlnm - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "minerlnm - Win32 Release"

OUTDIR=.\..\..\BIN
INTDIR=.\MLNM
# Begin Custom Macros
OutDir=.\..\..\BIN
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\minerlnm.exe"

!ELSE 

ALL : "$(OUTDIR)\minerlnm.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\mabout.obj"
	-@erase "$(INTDIR)\Mchild.obj"
	-@erase "$(INTDIR)\mctrl.obj"
	-@erase "$(INTDIR)\Mdlg.obj"
	-@erase "$(INTDIR)\merror.obj"
	-@erase "$(INTDIR)\mframe.obj"
	-@erase "$(INTDIR)\minerva.obj"
	-@erase "$(INTDIR)\minerva.res"
	-@erase "$(INTDIR)\mmenu.obj"
	-@erase "$(INTDIR)\mmisc.obj"
	-@erase "$(INTDIR)\Mmru.obj"
	-@erase "$(INTDIR)\Mobmp.obj"
	-@erase "$(INTDIR)\Moim4.obj"
	-@erase "$(INTDIR)\moimstar.obj"
	-@erase "$(INTDIR)\Mopen.obj"
	-@erase "$(INTDIR)\mopng.obj"
	-@erase "$(INTDIR)\Moprgjpg.obj"
	-@erase "$(INTDIR)\Moseqjpg.obj"
	-@erase "$(INTDIR)\moutil81.obj"
	-@erase "$(INTDIR)\mowavlet.obj"
	-@erase "$(INTDIR)\Msave.obj"
	-@erase "$(INTDIR)\msimstar.obj"
	-@erase "$(INTDIR)\Msprgjpg.obj"
	-@erase "$(INTDIR)\msseqjpg.obj"
	-@erase "$(INTDIR)\msutil80.obj"
	-@erase "$(INTDIR)\mswavlet.obj"
	-@erase "$(INTDIR)\mtutil82.obj"
	-@erase "$(INTDIR)\Mxp2s.obj"
	-@erase "$(INTDIR)\mxrotate.obj"
	-@erase "$(INTDIR)\mxs2p.obj"
	-@erase "$(INTDIR)\mxzoom.obj"
	-@erase "$(INTDIR)\pic2list.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\minerlnm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W4 /WX /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)\minerlnm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\MLNM/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\minerva.res" /d "NDEBUG" /d "SSM_RES" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minerlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\lib\pnlib20m.lib ctl3d32s.lib version.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\minerlnm.pdb" /machine:I386\
 /out:"$(OUTDIR)\minerlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mabout.obj" \
	"$(INTDIR)\Mchild.obj" \
	"$(INTDIR)\mctrl.obj" \
	"$(INTDIR)\Mdlg.obj" \
	"$(INTDIR)\merror.obj" \
	"$(INTDIR)\mframe.obj" \
	"$(INTDIR)\minerva.obj" \
	"$(INTDIR)\minerva.res" \
	"$(INTDIR)\mmenu.obj" \
	"$(INTDIR)\mmisc.obj" \
	"$(INTDIR)\Mmru.obj" \
	"$(INTDIR)\Mobmp.obj" \
	"$(INTDIR)\Moim4.obj" \
	"$(INTDIR)\moimstar.obj" \
	"$(INTDIR)\Mopen.obj" \
	"$(INTDIR)\mopng.obj" \
	"$(INTDIR)\Moprgjpg.obj" \
	"$(INTDIR)\Moseqjpg.obj" \
	"$(INTDIR)\moutil81.obj" \
	"$(INTDIR)\mowavlet.obj" \
	"$(INTDIR)\Msave.obj" \
	"$(INTDIR)\msimstar.obj" \
	"$(INTDIR)\Msprgjpg.obj" \
	"$(INTDIR)\msseqjpg.obj" \
	"$(INTDIR)\msutil80.obj" \
	"$(INTDIR)\mswavlet.obj" \
	"$(INTDIR)\mtutil82.obj" \
	"$(INTDIR)\Mxp2s.obj" \
	"$(INTDIR)\mxrotate.obj" \
	"$(INTDIR)\mxs2p.obj" \
	"$(INTDIR)\mxzoom.obj" \
	"$(INTDIR)\pic2list.obj"

"$(OUTDIR)\minerlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

OUTDIR=.\..\..\BIN
INTDIR=.\MLNMD
# Begin Custom Macros
OutDir=.\..\..\BIN
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\minerlnm.exe"

!ELSE 

ALL : "$(OUTDIR)\minerlnm.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\mabout.obj"
	-@erase "$(INTDIR)\Mchild.obj"
	-@erase "$(INTDIR)\mctrl.obj"
	-@erase "$(INTDIR)\Mdlg.obj"
	-@erase "$(INTDIR)\merror.obj"
	-@erase "$(INTDIR)\mframe.obj"
	-@erase "$(INTDIR)\minerva.obj"
	-@erase "$(INTDIR)\minerva.res"
	-@erase "$(INTDIR)\mmenu.obj"
	-@erase "$(INTDIR)\mmisc.obj"
	-@erase "$(INTDIR)\Mmru.obj"
	-@erase "$(INTDIR)\Mobmp.obj"
	-@erase "$(INTDIR)\Moim4.obj"
	-@erase "$(INTDIR)\moimstar.obj"
	-@erase "$(INTDIR)\Mopen.obj"
	-@erase "$(INTDIR)\mopng.obj"
	-@erase "$(INTDIR)\Moprgjpg.obj"
	-@erase "$(INTDIR)\Moseqjpg.obj"
	-@erase "$(INTDIR)\moutil81.obj"
	-@erase "$(INTDIR)\mowavlet.obj"
	-@erase "$(INTDIR)\Msave.obj"
	-@erase "$(INTDIR)\msimstar.obj"
	-@erase "$(INTDIR)\Msprgjpg.obj"
	-@erase "$(INTDIR)\msseqjpg.obj"
	-@erase "$(INTDIR)\msutil80.obj"
	-@erase "$(INTDIR)\mswavlet.obj"
	-@erase "$(INTDIR)\mtutil82.obj"
	-@erase "$(INTDIR)\Mxp2s.obj"
	-@erase "$(INTDIR)\mxrotate.obj"
	-@erase "$(INTDIR)\mxs2p.obj"
	-@erase "$(INTDIR)\mxzoom.obj"
	-@erase "$(INTDIR)\pic2list.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\minerlnm.exe"
	-@erase "$(OUTDIR)\minerlnm.ilk"
	-@erase "$(OUTDIR)\minerlnm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W4 /WX /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\minerlnm.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\MLNMD/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\minerva.res" /d "_DEBUG" /d "SSM_RES" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minerlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\lib\picn20m.lib ctl3d32s.lib version.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\minerlnm.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\minerlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mabout.obj" \
	"$(INTDIR)\Mchild.obj" \
	"$(INTDIR)\mctrl.obj" \
	"$(INTDIR)\Mdlg.obj" \
	"$(INTDIR)\merror.obj" \
	"$(INTDIR)\mframe.obj" \
	"$(INTDIR)\minerva.obj" \
	"$(INTDIR)\minerva.res" \
	"$(INTDIR)\mmenu.obj" \
	"$(INTDIR)\mmisc.obj" \
	"$(INTDIR)\Mmru.obj" \
	"$(INTDIR)\Mobmp.obj" \
	"$(INTDIR)\Moim4.obj" \
	"$(INTDIR)\moimstar.obj" \
	"$(INTDIR)\Mopen.obj" \
	"$(INTDIR)\mopng.obj" \
	"$(INTDIR)\Moprgjpg.obj" \
	"$(INTDIR)\Moseqjpg.obj" \
	"$(INTDIR)\moutil81.obj" \
	"$(INTDIR)\mowavlet.obj" \
	"$(INTDIR)\Msave.obj" \
	"$(INTDIR)\msimstar.obj" \
	"$(INTDIR)\Msprgjpg.obj" \
	"$(INTDIR)\msseqjpg.obj" \
	"$(INTDIR)\msutil80.obj" \
	"$(INTDIR)\mswavlet.obj" \
	"$(INTDIR)\mtutil82.obj" \
	"$(INTDIR)\Mxp2s.obj" \
	"$(INTDIR)\mxrotate.obj" \
	"$(INTDIR)\mxs2p.obj" \
	"$(INTDIR)\mxzoom.obj" \
	"$(INTDIR)\pic2list.obj"

"$(OUTDIR)\minerlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "minerlnm - Win32 Release" || "$(CFG)" ==\
 "minerlnm - Win32 Debug"
SOURCE=.\mabout.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MABOU=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mabout.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mabout.obj" : $(SOURCE) $(DEP_CPP_MABOU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MABOU=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mabout.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mabout.obj" : $(SOURCE) $(DEP_CPP_MABOU) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mchild.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MCHIL=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\Mchild.obj" : $(SOURCE) $(DEP_CPP_MCHIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MCHIL=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\Mchild.obj" : $(SOURCE) $(DEP_CPP_MCHIL) "$(INTDIR)"


!ENDIF 

SOURCE=.\mctrl.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MCTRL=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mctrl.obj" : $(SOURCE) $(DEP_CPP_MCTRL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MCTRL=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mctrl.obj" : $(SOURCE) $(DEP_CPP_MCTRL) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mdlg.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MDLG_=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mdlg.obj" : $(SOURCE) $(DEP_CPP_MDLG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MDLG_=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mdlg.obj" : $(SOURCE) $(DEP_CPP_MDLG_) "$(INTDIR)"


!ENDIF 

SOURCE=.\merror.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MERRO=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\merror.obj" : $(SOURCE) $(DEP_CPP_MERRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MERRO=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\merror.obj" : $(SOURCE) $(DEP_CPP_MERRO) "$(INTDIR)"


!ENDIF 

SOURCE=.\mframe.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MFRAM=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\ctl3d.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mframe.obj" : $(SOURCE) $(DEP_CPP_MFRAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MFRAM=\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\ctl3d.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mframe.obj" : $(SOURCE) $(DEP_CPP_MFRAM) "$(INTDIR)"


!ENDIF 

SOURCE=.\minerva.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MINER=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\ctl3d.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\minerva.obj" : $(SOURCE) $(DEP_CPP_MINER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MINER=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\ctl3d.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\minerva.obj" : $(SOURCE) $(DEP_CPP_MINER) "$(INTDIR)"


!ENDIF 

SOURCE=.\minerva.rc
DEP_RSC_MINERV=\
	".\ctl3d.h"\
	".\RES\3DCHECK.BMP"\
	".\RES\H_MOVE.CUR"\
	".\RES\MCHILD.ICO"\
	".\RES\MFRAME.ICO"\
	".\RES\PEGASUS.ICO"\
	

"$(INTDIR)\minerva.res" : $(SOURCE) $(DEP_RSC_MINERV) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\mmenu.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MMENU=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mabout.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\mmenu.obj" : $(SOURCE) $(DEP_CPP_MMENU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MMENU=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mabout.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\mmenu.obj" : $(SOURCE) $(DEP_CPP_MMENU) "$(INTDIR)"


!ENDIF 

SOURCE=.\mmisc.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MMISC=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mmisc.obj" : $(SOURCE) $(DEP_CPP_MMISC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MMISC=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mmisc.obj" : $(SOURCE) $(DEP_CPP_MMISC) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mmru.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MMRU_=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mmru.obj" : $(SOURCE) $(DEP_CPP_MMRU_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MMRU_=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mmru.obj" : $(SOURCE) $(DEP_CPP_MMRU_) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mobmp.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOBMP=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mobmp.obj" : $(SOURCE) $(DEP_CPP_MOBMP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOBMP=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mobmp.obj" : $(SOURCE) $(DEP_CPP_MOBMP) "$(INTDIR)"


!ENDIF 

SOURCE=.\Moim4.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOIM4=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moim4.obj" : $(SOURCE) $(DEP_CPP_MOIM4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOIM4=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moim4.obj" : $(SOURCE) $(DEP_CPP_MOIM4) "$(INTDIR)"


!ENDIF 

SOURCE=.\moimstar.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOIMS=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\moimstar.obj" : $(SOURCE) $(DEP_CPP_MOIMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOIMS=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\moimstar.obj" : $(SOURCE) $(DEP_CPP_MOIMS) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mopen.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOPEN=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\Mopen.obj" : $(SOURCE) $(DEP_CPP_MOPEN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOPEN=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\Mopen.obj" : $(SOURCE) $(DEP_CPP_MOPEN) "$(INTDIR)"


!ENDIF 

SOURCE=.\mopng.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOPNG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mopng.obj" : $(SOURCE) $(DEP_CPP_MOPNG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOPNG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mopng.obj" : $(SOURCE) $(DEP_CPP_MOPNG) "$(INTDIR)"


!ENDIF 

SOURCE=.\Moprgjpg.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOPRG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moprgjpg.obj" : $(SOURCE) $(DEP_CPP_MOPRG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOPRG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moprgjpg.obj" : $(SOURCE) $(DEP_CPP_MOPRG) "$(INTDIR)"


!ENDIF 

SOURCE=.\Moseqjpg.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOSEQ=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moseqjpg.obj" : $(SOURCE) $(DEP_CPP_MOSEQ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOSEQ=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moseqjpg.obj" : $(SOURCE) $(DEP_CPP_MOSEQ) "$(INTDIR)"


!ENDIF 

SOURCE=.\moutil81.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\moutil81.obj" : $(SOURCE) $(DEP_CPP_MOUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\moutil81.obj" : $(SOURCE) $(DEP_CPP_MOUTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\mowavlet.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MOWAV=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mowavlet.obj" : $(SOURCE) $(DEP_CPP_MOWAV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MOWAV=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mowavlet.obj" : $(SOURCE) $(DEP_CPP_MOWAV) "$(INTDIR)"


!ENDIF 

SOURCE=.\Msave.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSAVE=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\Msave.obj" : $(SOURCE) $(DEP_CPP_MSAVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSAVE=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmenu.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\Msave.h"\
	

"$(INTDIR)\Msave.obj" : $(SOURCE) $(DEP_CPP_MSAVE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msimstar.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSIMS=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msimstar.obj" : $(SOURCE) $(DEP_CPP_MSIMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSIMS=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msimstar.obj" : $(SOURCE) $(DEP_CPP_MSIMS) "$(INTDIR)"


!ENDIF 

SOURCE=.\Msprgjpg.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSPRG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Msprgjpg.obj" : $(SOURCE) $(DEP_CPP_MSPRG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSPRG=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Msprgjpg.obj" : $(SOURCE) $(DEP_CPP_MSPRG) "$(INTDIR)"


!ENDIF 

SOURCE=.\msseqjpg.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSSEQ=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msseqjpg.obj" : $(SOURCE) $(DEP_CPP_MSSEQ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSSEQ=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msseqjpg.obj" : $(SOURCE) $(DEP_CPP_MSSEQ) "$(INTDIR)"


!ENDIF 

SOURCE=.\msutil80.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msutil80.obj" : $(SOURCE) $(DEP_CPP_MSUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\msutil80.obj" : $(SOURCE) $(DEP_CPP_MSUTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\mswavlet.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MSWAV=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mswavlet.obj" : $(SOURCE) $(DEP_CPP_MSWAV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MSWAV=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mswavlet.obj" : $(SOURCE) $(DEP_CPP_MSWAV) "$(INTDIR)"


!ENDIF 

SOURCE=.\mtutil82.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MTUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mtutil82.obj" : $(SOURCE) $(DEP_CPP_MTUTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MTUTI=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mtutil82.obj" : $(SOURCE) $(DEP_CPP_MTUTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\Mxp2s.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MXP2S=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mxp2s.obj" : $(SOURCE) $(DEP_CPP_MXP2S) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MXP2S=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mxp2s.obj" : $(SOURCE) $(DEP_CPP_MXP2S) "$(INTDIR)"


!ENDIF 

SOURCE=.\mxrotate.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MXROT=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	"..\..\include\tiff.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	".\pic2list.h"\
	

"$(INTDIR)\mxrotate.obj" : $(SOURCE) $(DEP_CPP_MXROT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MXROT=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mxrotate.obj" : $(SOURCE) $(DEP_CPP_MXROT) "$(INTDIR)"


!ENDIF 

SOURCE=.\mxs2p.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MXS2P=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mxs2p.obj" : $(SOURCE) $(DEP_CPP_MXS2P) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MXS2P=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mxs2p.obj" : $(SOURCE) $(DEP_CPP_MXS2P) "$(INTDIR)"


!ENDIF 

SOURCE=.\mxzoom.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_MXZOO=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mxzoom.obj" : $(SOURCE) $(DEP_CPP_MXZOO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_MXZOO=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	".\Mopen.h"\
	

"$(INTDIR)\mxzoom.obj" : $(SOURCE) $(DEP_CPP_MXZOO) "$(INTDIR)"


!ENDIF 

SOURCE=.\pic2list.c

!IF  "$(CFG)" == "minerlnm - Win32 Release"

DEP_CPP_PIC2L=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	"..\..\include\tiff.h"\
	".\pic2list.h"\
	

"$(INTDIR)\pic2list.obj" : $(SOURCE) $(DEP_CPP_PIC2L) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minerlnm - Win32 Debug"

DEP_CPP_PIC2L=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\pic2file.h"\
	"..\..\include\stdtypes.h"\
	"..\..\include\tiff.h"\
	".\pic2list.h"\
	{$(INCLUDE)}"internal.h"\
	{$(INCLUDE)}"picclib.h"\
	

"$(INTDIR)\pic2list.obj" : $(SOURCE) $(DEP_CPP_PIC2L) "$(INTDIR)"


!ENDIF 


!ENDIF 

