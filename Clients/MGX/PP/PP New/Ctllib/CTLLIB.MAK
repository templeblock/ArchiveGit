# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "CTLLIB.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/CTLLIB.lib $(OUTDIR)/CTLLIB.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CTLLIB" /D "STRICT" /D "C_CODE" /c
# SUBTRACT CPP /O<none> /Fr
CPP_PROJ=/nologo /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CTLLIB"\
 /D "STRICT" /D "C_CODE" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"CTLLIB.bsc" 

$(OUTDIR)/CTLLIB.bsc : $(OUTDIR)  $(BSC32_SBRS)
LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"CTLLIB.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/CHECK.OBJ \
	$(INTDIR)/DLGSEC.OBJ \
	$(INTDIR)/CTLTOOLS.OBJ \
	$(INTDIR)/GRID.OBJ \
	$(INTDIR)/CTLINIT.OBJ \
	$(INTDIR)/DLGCAT.OBJ \
	$(INTDIR)/PICKER.OBJ \
	$(INTDIR)/GRADCTL.OBJ \
	$(INTDIR)/MAPCTL.OBJ \
	$(INTDIR)/CTLUTIL.OBJ \
	$(INTDIR)/COMMON.OBJ \
	$(INTDIR)/SLIDER.OBJ \
	$(INTDIR)/LISTBOX.OBJ \
	$(INTDIR)/HISTOCTL.OBJ \
	$(INTDIR)/RADIO.OBJ \
	$(INTDIR)/PATCH.OBJ \
	$(INTDIR)/ICON.OBJ \
	$(INTDIR)/TAGCTL.OBJ \
	$(INTDIR)/STICK.OBJ \
	$(INTDIR)/STEXT.OBJ \
	$(INTDIR)/SPIN.OBJ \
	$(INTDIR)/DLGITEM.OBJ \
	$(INTDIR)/LUTCTL.OBJ \
	$(INTDIR)/TABDLG.OBJ

$(OUTDIR)/CTLLIB.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/CTLLIB.lib $(OUTDIR)/CTLLIB.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "CTLLIB" /D "STRICT" /D "C_CODE" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /Zp4 /MT /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "CTLLIB" /D "STRICT" /D "C_CODE" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"CTLLIB.bsc" 

$(OUTDIR)/CTLLIB.bsc : $(OUTDIR)  $(BSC32_SBRS)
LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"CTLLIB.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/CHECK.OBJ \
	$(INTDIR)/DLGSEC.OBJ \
	$(INTDIR)/CTLTOOLS.OBJ \
	$(INTDIR)/GRID.OBJ \
	$(INTDIR)/CTLINIT.OBJ \
	$(INTDIR)/DLGCAT.OBJ \
	$(INTDIR)/PICKER.OBJ \
	$(INTDIR)/GRADCTL.OBJ \
	$(INTDIR)/MAPCTL.OBJ \
	$(INTDIR)/CTLUTIL.OBJ \
	$(INTDIR)/COMMON.OBJ \
	$(INTDIR)/SLIDER.OBJ \
	$(INTDIR)/LISTBOX.OBJ \
	$(INTDIR)/HISTOCTL.OBJ \
	$(INTDIR)/RADIO.OBJ \
	$(INTDIR)/PATCH.OBJ \
	$(INTDIR)/ICON.OBJ \
	$(INTDIR)/TAGCTL.OBJ \
	$(INTDIR)/STICK.OBJ \
	$(INTDIR)/STEXT.OBJ \
	$(INTDIR)/SPIN.OBJ \
	$(INTDIR)/DLGITEM.OBJ \
	$(INTDIR)/LUTCTL.OBJ \
	$(INTDIR)/TABDLG.OBJ

$(OUTDIR)/CTLLIB.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\CHECK.CPP
DEP_CHECK=\
	.\CTLUTIL.H\
	.\CHECK.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\CHECK.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/CHECK.OBJ :  $(SOURCE)  $(DEP_CHECK) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DLGSEC.C
DEP_DLGSE=\
	.\DEBUG.H\
	.\DYNDLG.H\
	.\WINXEXT.H\
	.\DLGCAT.H\
	.\DLGSEC.H\
	.\DLGRES.H

$(INTDIR)/DLGSEC.OBJ :  $(SOURCE)  $(DEP_DLGSE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTLTOOLS.CPP
DEP_CTLTO=\
	.\CTLUTIL.H\
	.\CTLTOOLS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/CTLTOOLS.OBJ :  $(SOURCE)  $(DEP_CTLTO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GRID.CPP
DEP_GRID_=\
	.\CTLUTIL.H\
	.\GRID.H\
	\pp\frame\SUPERLIB.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\FRAMELIB.H\
	.\GRID.RH\
	.\COMMON.RH\
	\pp\frame\FIXED.H

$(INTDIR)/GRID.OBJ :  $(SOURCE)  $(DEP_GRID_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTLINIT.CPP
DEP_CTLIN=\
	.\CTLUTIL.H\
	.\CTLLIB.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\RADIO.H\
	.\CHECK.H\
	.\SLIDER.H\
	.\SPIN.H\
	.\ICON.H\
	.\PICKER.H\
	.\GRID.H\
	.\LISTBOX.H\
	.\DLGITEM.H\
	.\CTLTOOLS.H\
	.\STICK.H\
	.\LUTCTL.H\
	.\MAPCTL.H\
	.\HISTOCTL.H\
	.\GRADCTL.H\
	.\PATCH.H\
	.\TAGCTL.H\
	.\STEXT.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	.\RADIO.RH\
	.\CHECK.RH\
	.\SLIDER.RH\
	.\SPIN.RH\
	\pp\frame\FIXED.H\
	.\ICON.RH\
	.\PICKER.RH\
	.\GRID.RH\
	.\LISTBOX.RH\
	.\STICK.RH\
	.\LUTCTL.RH

$(INTDIR)/CTLINIT.OBJ :  $(SOURCE)  $(DEP_CTLIN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DLGCAT.C
DEP_DLGCA=\
	.\DEBUG.H\
	.\DLGCAT.H\
	.\DLGRES.H\
	.\WINXEXT.H

$(INTDIR)/DLGCAT.OBJ :  $(SOURCE)  $(DEP_DLGCA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PICKER.CPP
DEP_PICKE=\
	.\CTLUTIL.H\
	.\PICKER.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\PICKER.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/PICKER.OBJ :  $(SOURCE)  $(DEP_PICKE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GRADCTL.CPP
DEP_GRADC=\
	.\CTLUTIL.H\
	.\GRADCTL.H\
	.\GRADCTL.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	\pp\frame\FIXED.H\
	.\COMMON.RH

$(INTDIR)/GRADCTL.OBJ :  $(SOURCE)  $(DEP_GRADC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAPCTL.CPP
DEP_MAPCT=\
	.\CTLUTIL.H\
	.\MAPCTL.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/MAPCTL.OBJ :  $(SOURCE)  $(DEP_MAPCT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTLUTIL.CPP
DEP_CTLUT=\
	.\CTLUTIL.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/CTLUTIL.OBJ :  $(SOURCE)  $(DEP_CTLUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COMMON.CPP
DEP_COMMO=\
	.\CTLUTIL.H\
	.\CTLTOOLS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/COMMON.OBJ :  $(SOURCE)  $(DEP_COMMO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SLIDER.CPP
DEP_SLIDE=\
	.\CTLUTIL.H\
	.\SLIDER.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\SLIDER.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/SLIDER.OBJ :  $(SOURCE)  $(DEP_SLIDE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LISTBOX.CPP
DEP_LISTB=\
	\pp\frame\FRAMELIB.H\
	.\LISTBOX.H\
	\pp\lib\TYPES.H\
	\pp\frame\FIXED.H\
	.\LISTBOX.RH

$(INTDIR)/LISTBOX.OBJ :  $(SOURCE)  $(DEP_LISTB) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HISTOCTL.CPP
DEP_HISTO=\
	.\CTLUTIL.H\
	.\HISTOCTL.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/HISTOCTL.OBJ :  $(SOURCE)  $(DEP_HISTO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RADIO.CPP
DEP_RADIO=\
	.\CTLUTIL.H\
	.\RADIO.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\RADIO.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/RADIO.OBJ :  $(SOURCE)  $(DEP_RADIO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PATCH.CPP
DEP_PATCH=\
	.\CTLUTIL.H\
	.\CTLTOOLS.H\
	.\PATCH.H\
	.\PATCH.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	\pp\frame\FIXED.H\
	.\COMMON.RH

$(INTDIR)/PATCH.OBJ :  $(SOURCE)  $(DEP_PATCH) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICON.CPP
DEP_ICON_=\
	.\CTLUTIL.H\
	.\CTLTOOLS.H\
	.\ICON.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\ICON.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/ICON.OBJ :  $(SOURCE)  $(DEP_ICON_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TAGCTL.CPP
DEP_TAGCT=\
	.\CTLUTIL.H\
	.\TAGCTL.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/TAGCTL.OBJ :  $(SOURCE)  $(DEP_TAGCT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STICK.CPP
DEP_STICK=\
	.\CTLUTIL.H\
	.\CTLTOOLS.H\
	\pp\frame\MACROS.H\
	.\STICK.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\STICK.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/STICK.OBJ :  $(SOURCE)  $(DEP_STICK) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STEXT.CPP
DEP_STEXT=\
	.\CTLUTIL.H\
	.\STEXT.H\
	.\STEXT.RH\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/STEXT.OBJ :  $(SOURCE)  $(DEP_STEXT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SPIN.CPP
DEP_SPIN_=\
	.\CTLUTIL.H\
	.\SPIN.H\
	\pp\frame\MACROS.H\
	.\DLGITEM.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\SPIN.RH\
	\pp\frame\FIXED.H\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H

$(INTDIR)/SPIN.OBJ :  $(SOURCE)  $(DEP_SPIN_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DLGITEM.CPP
DEP_DLGIT=\
	\pp\frame\FRAMELIB.H\
	.\CTLUTIL.H\
	.\DLGITEM.H\
	\pp\lib\TYPES.H\
	\pp\frame\FIXED.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\COMMON.RH

$(INTDIR)/DLGITEM.OBJ :  $(SOURCE)  $(DEP_DLGIT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LUTCTL.CPP
DEP_LUTCT=\
	.\CTLUTIL.H\
	.\LUTCTL.H\
	\pp\frame\MACROS.H\
	\pp\lib\TYPES.H\
	.\COMMON.H\
	\pp\frame\SUPERLIB.H\
	.\LUTCTL.RH\
	.\COMMON.RH\
	\pp\frame\FRAMELIB.H\
	\pp\frame\FIXED.H

$(INTDIR)/LUTCTL.OBJ :  $(SOURCE)  $(DEP_LUTCT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TABDLG.C
DEP_TABDL=\
	.\_tabdlg.h\
	.\tools.h\
	.\tabdlg.h

$(INTDIR)/TABDLG.OBJ :  $(SOURCE)  $(DEP_TABDL) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
