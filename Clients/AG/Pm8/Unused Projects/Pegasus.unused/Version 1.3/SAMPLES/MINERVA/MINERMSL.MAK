# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=minermsl - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to minermsl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "minermsl - Win32 Release" && "$(CFG)" !=\
 "minermsl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "minermsl.mak" CFG="minermsl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minermsl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "minermsl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "minermsl - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "minermsl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\BIN"
# PROP Intermediate_Dir "M32"
# PROP Target_Dir ""
OUTDIR=.\..\..\BIN
INTDIR=.\M32

ALL : "$(OUTDIR)\minermsl.exe"

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
	-@erase "$(INTDIR)\Moprgjpg.obj"
	-@erase "$(INTDIR)\Moseqjpg.obj"
	-@erase "$(INTDIR)\moutil81.obj"
	-@erase "$(INTDIR)\Msave.obj"
	-@erase "$(INTDIR)\msimstar.obj"
	-@erase "$(INTDIR)\Msprgjpg.obj"
	-@erase "$(INTDIR)\msseqjpg.obj"
	-@erase "$(INTDIR)\msutil80.obj"
	-@erase "$(INTDIR)\mtutil82.obj"
	-@erase "$(INTDIR)\Mxp2s.obj"
	-@erase "$(INTDIR)\mxrotate.obj"
	-@erase "$(INTDIR)\mxs2p.obj"
	-@erase "$(INTDIR)\mxzoom.obj"
	-@erase "$(OUTDIR)\minermsl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W4 /WX /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W4 /WX /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/minermsl.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\M32/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/minerva.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/minermsl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\lib\pn13lm.lib ..\..\lib\pn1813lm.lib ctl3d32s.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=..\..\lib\pn13lm.lib ..\..\lib\pn1813lm.lib ctl3d32s.lib\
 version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/minermsl.pdb" /machine:I386 /out:"$(OUTDIR)/minermsl.exe" 
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
	"$(INTDIR)\Moprgjpg.obj" \
	"$(INTDIR)\Moseqjpg.obj" \
	"$(INTDIR)\moutil81.obj" \
	"$(INTDIR)\Msave.obj" \
	"$(INTDIR)\msimstar.obj" \
	"$(INTDIR)\Msprgjpg.obj" \
	"$(INTDIR)\msseqjpg.obj" \
	"$(INTDIR)\msutil80.obj" \
	"$(INTDIR)\mtutil82.obj" \
	"$(INTDIR)\Mxp2s.obj" \
	"$(INTDIR)\mxrotate.obj" \
	"$(INTDIR)\mxs2p.obj" \
	"$(INTDIR)\mxzoom.obj"

"$(OUTDIR)\minermsl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "minermsl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\BIN"
# PROP Intermediate_Dir "M32"
# PROP Target_Dir ""
OUTDIR=.\..\..\BIN
INTDIR=.\M32

ALL : "$(OUTDIR)\minermsl.exe"

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
	-@erase "$(INTDIR)\Moprgjpg.obj"
	-@erase "$(INTDIR)\Moseqjpg.obj"
	-@erase "$(INTDIR)\moutil81.obj"
	-@erase "$(INTDIR)\Msave.obj"
	-@erase "$(INTDIR)\msimstar.obj"
	-@erase "$(INTDIR)\Msprgjpg.obj"
	-@erase "$(INTDIR)\msseqjpg.obj"
	-@erase "$(INTDIR)\msutil80.obj"
	-@erase "$(INTDIR)\mtutil82.obj"
	-@erase "$(INTDIR)\Mxp2s.obj"
	-@erase "$(INTDIR)\mxrotate.obj"
	-@erase "$(INTDIR)\mxs2p.obj"
	-@erase "$(INTDIR)\mxzoom.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\minermsl.exe"
	-@erase "$(OUTDIR)\minermsl.ilk"
	-@erase "$(OUTDIR)\minermsl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W4 /WX /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W4 /WX /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/minermsl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\M32/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/minerva.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/minermsl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\..\lib\pn13lm.lib ..\..\lib\pn1813lm.lib ctl3d32s.lib version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=..\..\lib\pn13lm.lib ..\..\lib\pn1813lm.lib ctl3d32s.lib\
 version.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/minermsl.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/minermsl.exe" 
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
	"$(INTDIR)\Moprgjpg.obj" \
	"$(INTDIR)\Moseqjpg.obj" \
	"$(INTDIR)\moutil81.obj" \
	"$(INTDIR)\Msave.obj" \
	"$(INTDIR)\msimstar.obj" \
	"$(INTDIR)\Msprgjpg.obj" \
	"$(INTDIR)\msseqjpg.obj" \
	"$(INTDIR)\msutil80.obj" \
	"$(INTDIR)\mtutil82.obj" \
	"$(INTDIR)\Mxp2s.obj" \
	"$(INTDIR)\mxrotate.obj" \
	"$(INTDIR)\mxs2p.obj" \
	"$(INTDIR)\mxzoom.obj"

"$(OUTDIR)\minermsl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "minermsl - Win32 Release"
# Name "minermsl - Win32 Debug"

!IF  "$(CFG)" == "minermsl - Win32 Release"

!ELSEIF  "$(CFG)" == "minermsl - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\mxzoom.c
DEP_CPP_MXZOO=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mchild.c
DEP_CPP_MCHIL=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mctrl.c
DEP_CPP_MCTRL=\
	"..\..\include\bmp.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mctrl.obj" : $(SOURCE) $(DEP_CPP_MCTRL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mdlg.c
DEP_CPP_MDLG_=\
	"..\..\include\bmp.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mdlg.obj" : $(SOURCE) $(DEP_CPP_MDLG_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\merror.c
DEP_CPP_MERRO=\
	"..\..\include\bmp.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\merror.obj" : $(SOURCE) $(DEP_CPP_MERRO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mframe.c
DEP_CPP_MFRAM=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\minerva.c
DEP_CPP_MINER=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\ctl3d.h"\
	".\Mchild.h"\
	".\mctrl.h"\
	".\merror.h"\
	".\Mframe.h"\
	".\minerva.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\minerva.obj" : $(SOURCE) $(DEP_CPP_MINER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\minervab.csm

!IF  "$(CFG)" == "minermsl - Win32 Release"

!ELSEIF  "$(CFG)" == "minermsl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Minervbl.csm

!IF  "$(CFG)" == "minermsl - Win32 Release"

!ELSEIF  "$(CFG)" == "minermsl - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mmenu.c
DEP_CPP_MMENU=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mmisc.c
DEP_CPP_MMISC=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mmisc.obj" : $(SOURCE) $(DEP_CPP_MMISC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mmru.c
DEP_CPP_MMRU_=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mmru.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mmru.obj" : $(SOURCE) $(DEP_CPP_MMRU_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mobmp.c
DEP_CPP_MOBMP=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Moim4.c
DEP_CPP_MOIM4=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moim4.obj" : $(SOURCE) $(DEP_CPP_MOIM4) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\moimstar.c
DEP_CPP_MOIMS=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\moimstar.obj" : $(SOURCE) $(DEP_CPP_MOIMS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mopen.c
DEP_CPP_MOPEN=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Moprgjpg.c
DEP_CPP_MOPRG=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Moprgjpg.obj" : $(SOURCE) $(DEP_CPP_MOPRG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Moseqjpg.c
DEP_CPP_MOSEQ=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\moutil81.c
DEP_CPP_MOUTI=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msave.c
DEP_CPP_MSAVE=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msimstar.c
DEP_CPP_MSIMS=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\msimstar.obj" : $(SOURCE) $(DEP_CPP_MSIMS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msprgjpg.c
DEP_CPP_MSPRG=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Msprgjpg.obj" : $(SOURCE) $(DEP_CPP_MSPRG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msseqjpg.c
DEP_CPP_MSSEQ=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msutil80.c
DEP_CPP_MSUTI=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\mdlg.h"\
	".\merror.h"\
	".\minerva.h"\
	".\Mmisc.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\msutil80.obj" : $(SOURCE) $(DEP_CPP_MSUTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mtutil82.c
DEP_CPP_MTUTI=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mxp2s.c
DEP_CPP_MXP2S=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\Mxp2s.obj" : $(SOURCE) $(DEP_CPP_MXP2S) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mxrotate.c
DEP_CPP_MXROT=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mxrotate.obj" : $(SOURCE) $(DEP_CPP_MXROT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mxs2p.c
DEP_CPP_MXS2P=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\stdtypes.h"\
	".\mctrl.h"\
	".\minerva.h"\
	".\Mopcodes.h"\
	

"$(INTDIR)\mxs2p.obj" : $(SOURCE) $(DEP_CPP_MXS2P) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mabout.c
DEP_CPP_MABOU=\
	"..\..\include\bmp.h"\
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


# End Source File
################################################################################
# Begin Source File

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


# End Source File
# End Target
# End Project
################################################################################
