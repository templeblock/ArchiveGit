# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug" && "$(CFG)" !=\
 "Win32 Middle"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "BROWSER.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Middle" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Release"
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

ALL : $(OUTDIR)/BROWSER.lib $(OUTDIR)/BROWSER.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c /Tp
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo$(INTDIR)/ /c /Tp 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"BROWSER.bsc" 

$(OUTDIR)/BROWSER.bsc : $(OUTDIR)  $(BSC32_SBRS)
LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"BROWSER.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/BSRITEMS.OBJ \
	$(INTDIR)/BROWSER.OBJ \
	$(INTDIR)/THMBSR.OBJ \
	$(INTDIR)/BSRPORT.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/THMMGR.OBJ \
	$(INTDIR)/HANDLEBS.OBJ \
	$(INTDIR)/BSRUTIL.OBJ \
	$(INTDIR)/EXTBSR.OBJ \
	$(INTDIR)/OLDTHM.OBJ \
	$(INTDIR)/DBFMGR.OBJ \
	$(INTDIR)/CTHMMGR.OBJ \
	$(INTDIR)/BSRDLGS.OBJ

$(OUTDIR)/BROWSER.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

ALL : $(OUTDIR)/BROWSER.lib $(OUTDIR)/BROWSER.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c /Tp
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo$(INTDIR)/ /c /Tp 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"BROWSER.bsc" 

$(OUTDIR)/BROWSER.bsc : $(OUTDIR)  $(BSC32_SBRS)
LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"BROWSER.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/BSRITEMS.OBJ \
	$(INTDIR)/BROWSER.OBJ \
	$(INTDIR)/THMBSR.OBJ \
	$(INTDIR)/BSRPORT.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/THMMGR.OBJ \
	$(INTDIR)/HANDLEBS.OBJ \
	$(INTDIR)/BSRUTIL.OBJ \
	$(INTDIR)/EXTBSR.OBJ \
	$(INTDIR)/OLDTHM.OBJ \
	$(INTDIR)/DBFMGR.OBJ \
	$(INTDIR)/CTHMMGR.OBJ \
	$(INTDIR)/BSRDLGS.OBJ

$(OUTDIR)/BROWSER.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Middle"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_Mi"
# PROP BASE Intermediate_Dir "Win32_Mi"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/BROWSER.lib $(OUTDIR)/BROWSER.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c /Tp
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c /Tp
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"WinRel/" /c /Tp 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"BROWSER.bsc" 

$(OUTDIR)/BROWSER.bsc : $(OUTDIR)  $(BSC32_SBRS)
LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:"WinRel\BROWSER.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/BSRITEMS.OBJ \
	$(INTDIR)/BROWSER.OBJ \
	$(INTDIR)/THMBSR.OBJ \
	$(INTDIR)/BSRPORT.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/THMMGR.OBJ \
	$(INTDIR)/HANDLEBS.OBJ \
	$(INTDIR)/BSRUTIL.OBJ \
	$(INTDIR)/EXTBSR.OBJ \
	$(INTDIR)/OLDTHM.OBJ \
	$(INTDIR)/DBFMGR.OBJ \
	$(INTDIR)/CTHMMGR.OBJ \
	$(INTDIR)/BSRDLGS.OBJ

$(OUTDIR)/BROWSER.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

SOURCE=.\BSRITEMS.C
DEP_BSRIT=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	\pp\frame\MACROS.H\
	.\BSRTYPES.H\
	.\HANDLEBS.H\
	.\BROWSER.H\
	.\BSRID.H\
	.\BSRITEMS.H\
	.\BSRUTIL.H\
	.\DBFMGR.H\
	.\CTHMMGR.H\
	.\BSRDLGS.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/BSRITEMS.OBJ :  $(SOURCE)  $(DEP_BSRIT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/BSRITEMS.OBJ :  $(SOURCE)  $(DEP_BSRIT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/BSRITEMS.OBJ :  $(SOURCE)  $(DEP_BSRIT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rcs.rc
# End Source File
################################################################################
# Begin Source File

SOURCE=.\BROWSER.C
DEP_BROWS=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BROWSER.H\
	.\BSRPORT.H\
	.\BSRID.H\
	.\BSRTYPES.H\
	.\HANDLEBS.H\
	.\BSRITEMS.H\
	.\BSRUTIL.H\
	.\BSRDLGS.H\
	.\THMBSR.H\
	.\CTHMMGR.H\
	.\EXTBSR.H\
	\pp\pp\inc\PPAFX.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\SVRDOC.H\
	\pp\pp\inc\MDIFRAME.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/BROWSER.OBJ :  $(SOURCE)  $(DEP_BROWS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/BROWSER.OBJ :  $(SOURCE)  $(DEP_BROWS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/BROWSER.OBJ :  $(SOURCE)  $(DEP_BROWS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\THMBSR.C
DEP_THMBS=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BSRTYPES.H\
	.\HANDLEBS.H\
	.\BSRID.H\
	.\THMBSR.H\
	.\BSRUTIL.H\
	.\CTHMMGR.H\
	.\BROWSER.H\
	.\DBFMGR.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/THMBSR.OBJ :  $(SOURCE)  $(DEP_THMBS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/THMBSR.OBJ :  $(SOURCE)  $(DEP_THMBS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/THMBSR.OBJ :  $(SOURCE)  $(DEP_THMBS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BSRPORT.C
DEP_BSRPO=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BSRTYPES.H\
	.\BSRUTIL.H\
	.\BSRPORT.H\
	.\BSRDLGS.H\
	.\HANDLEBS.H\
	.\BROWSER.H\
	.\BSRID.H\
	.\BSRITEMS.H\
	.\CTHMMGR.H\
	.\DBFMGR.H\
	.\THMMGR.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/BSRPORT.OBJ :  $(SOURCE)  $(DEP_BSRPO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/BSRPORT.OBJ :  $(SOURCE)  $(DEP_BSRPO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/BSRPORT.OBJ :  $(SOURCE)  $(DEP_BSRPO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SEARCH.C
DEP_SEARC=\
	\pp\pp\inc\PP.H\
	.\BSRTYPES.H\
	.\BSRUTIL.H\
	.\HANDLEBS.H\
	.\BROWSER.H\
	.\BSRID.H\
	.\BSRITEMS.H\
	.\DBFMGR.H\
	.\THMMGR.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\THMMGR.C
DEP_THMMG=\
	\pp\pp\inc\PP.H\
	.\THMMGR.H\
	.\BROWSER.H\
	.\BSRUTIL.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\ID.H\
	.\OLDTHM.H\
	\pp\pp\inc\ROUTINES.H\
	.\BSRTYPES.H\
	\pp\pp\inc\COMMONID.H\
	.\BSRID.H\
	\pp\lib\TYPES.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\FRAMELIB.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/THMMGR.OBJ :  $(SOURCE)  $(DEP_THMMG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/THMMGR.OBJ :  $(SOURCE)  $(DEP_THMMG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/THMMGR.OBJ :  $(SOURCE)  $(DEP_THMMG) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HANDLEBS.C
DEP_HANDL=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BSRTYPES.H\
	.\BSRUTIL.H\
	.\HANDLEBS.H\
	.\BROWSER.H\
	.\BSRID.H\
	.\BSRDLGS.H\
	.\BSRITEMS.H\
	.\DBFMGR.H\
	.\CTHMMGR.H\
	.\THMMGR.H\
	\pp\pp\inc\PPAFX.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\SVRDOC.H\
	\pp\pp\inc\MDIFRAME.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/HANDLEBS.OBJ :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/HANDLEBS.OBJ :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/HANDLEBS.OBJ :  $(SOURCE)  $(DEP_HANDL) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BSRUTIL.C
DEP_BSRUT=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BSRTYPES.H\
	.\BROWSER.H\
	.\BSRID.H\
	.\BSRUTIL.H\
	.\DBFMGR.H\
	.\CTHMMGR.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/BSRUTIL.OBJ :  $(SOURCE)  $(DEP_BSRUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/BSRUTIL.OBJ :  $(SOURCE)  $(DEP_BSRUT) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/BSRUTIL.OBJ :  $(SOURCE)  $(DEP_BSRUT) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EXTBSR.C
DEP_EXTBS=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BROWSER.H\
	.\BSRPORT.H\
	.\BSRID.H\
	.\BSRTYPES.H\
	.\HANDLEBS.H\
	.\BSRITEMS.H\
	.\BSRUTIL.H\
	.\BSRDLGS.H\
	.\THMBSR.H\
	.\CTHMMGR.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/EXTBSR.OBJ :  $(SOURCE)  $(DEP_EXTBS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/EXTBSR.OBJ :  $(SOURCE)  $(DEP_EXTBS) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/EXTBSR.OBJ :  $(SOURCE)  $(DEP_EXTBS) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OLDTHM.C
DEP_OLDTH=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\THMMGR.H\
	.\OLDTHM.H\
	.\BSRUTIL.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	.\BSRID.H\
	.\BSRTYPES.H\
	\pp\lib\TYPES.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\FRAMELIB.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	.\BROWSER.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/OLDTHM.OBJ :  $(SOURCE)  $(DEP_OLDTH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/OLDTHM.OBJ :  $(SOURCE)  $(DEP_OLDTH) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/OLDTHM.OBJ :  $(SOURCE)  $(DEP_OLDTH) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DBFMGR.C
DEP_DBFMG=\
	\pp\pp\inc\PP.H\
	.\BSRTYPES.H\
	.\DBFMGR.H\
	.\BSRUTIL.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	.\BROWSER.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/DBFMGR.OBJ :  $(SOURCE)  $(DEP_DBFMG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/DBFMGR.OBJ :  $(SOURCE)  $(DEP_DBFMG) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/DBFMGR.OBJ :  $(SOURCE)  $(DEP_DBFMG) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTHMMGR.C
DEP_CTHMM=\
	\pp\pp\inc\PP.H\
	.\THMMGR.H\
	.\CTHMMGR.H\
	.\BSRUTIL.H\
	\pp\pp\inc\ROUTINES.H\
	.\BSRTYPES.H\
	\pp\lib\TYPES.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\FRAMELIB.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	.\BROWSER.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/CTHMMGR.OBJ :  $(SOURCE)  $(DEP_CTHMM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/CTHMMGR.OBJ :  $(SOURCE)  $(DEP_CTHMM) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/CTHMMGR.OBJ :  $(SOURCE)  $(DEP_CTHMM) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BSRDLGS.C
DEP_BSRDL=\
	\pp\pp\inc\PP.H\
	\pp\pp\inc\ID.H\
	.\BSRID.H\
	.\BSRTYPES.H\
	.\BSRUTIL.H\
	.\HANDLEBS.H\
	.\BROWSER.H\
	.\BSRITEMS.H\
	.\DBFMGR.H\
	.\CTHMMGR.H\
	.\BSRDLGS.H\
	\pp\pp\inc\PPAFX.H\
	\pp\pp\inc\ROUTINES.H\
	\pp\pp\inc\COMMONID.H\
	\pp\lib\TYPES.H\
	\pp\frame\FRAMELIB.H\
	\pp\pp\inc\SVRDOC.H\
	\pp\pp\inc\MDIFRAME.H\
	\pp\pp\inc\LIST.H\
	\pp\frame\SUPERLIB.H\
	\pp\pp\inc\ROOT.H\
	\pp\pp\inc\PPLIST.H\
	\pp\pp\inc\POLY.H\
	\pp\pp\inc\EDITPOLY.H\
	\pp\pp\inc\IMAGE.H\
	\pp\pp\inc\APP.H\
	\pp\pp\inc\APPSTYLE.H\
	\pp\pp\inc\AVI.H\
	\pp\pp\inc\SHAPEDIT.H\
	\pp\pp\inc\FILEOPT.H\
	\pp\frame\MACROS.H\
	\pp\pp\inc\ENGINE.H\
	\pp\pp\inc\CLRMNG.H\
	\pp\pp\inc\CMDPARMS.H\
	\pp\pp\inc\OBJEDIT.H\
	\pp\pp\inc\GAUSS.H\
	\pp\pp\inc\TSELECT.H\
	\pp\pp\inc\TCONTROL.H\
	\pp\pp\inc\BRUSH.H\
	\pp\pp\inc\TDRAW.H\
	\pp\pp\inc\IMGIO.H\
	\pp\pp\inc\TIFFIO.H\
	\pp\pp\inc\OBJIO.H\
	\pp\pp\inc\DIALOG.H\
	\pp\pp\inc\CONTROLS.H\
	\pp\pp\inc\TASKMAN.H\
	\pp\pp\inc\MAP.H\
	\pp\pp\inc\DATA.H\
	\pp\pp\inc\IMGCMD.H\
	\pp\pp\inc\THREAD.H\
	\pp\ctllib\CTLLIB.H\
	\pp\pp\inc\CMDMACRO.H\
	\pp\pp\inc\MAPPROCS.H\
	\pp\pp\inc\FITPOLY.H\
	\pp\pp\inc\DLGMGR.H\
	\pp\pp\inc\WINIMAGE.H\
	\pp\frame\FIXED.H\
	\pp\pp\inc\OBJECT.H\
	\pp\pp\inc\CMDLIST.H\
	\pp\pp\inc\PP5IO.H\
	\pp\ctllib\COMMON.H\
	\pp\ctllib\RADIO.H\
	\pp\ctllib\CHECK.H\
	\pp\ctllib\SLIDER.H\
	\pp\ctllib\SPIN.H\
	\pp\ctllib\ICON.H\
	\pp\ctllib\PICKER.H\
	\pp\ctllib\GRID.H\
	\pp\ctllib\LISTBOX.H\
	\pp\ctllib\DLGITEM.H\
	\pp\ctllib\CTLTOOLS.H\
	\pp\ctllib\STICK.H\
	\pp\ctllib\LUTCTL.H\
	\pp\ctllib\MAPCTL.H\
	\pp\ctllib\HISTOCTL.H\
	\pp\ctllib\GRADCTL.H\
	\pp\ctllib\PATCH.H\
	\pp\ctllib\TAGCTL.H\
	\pp\ctllib\STEXT.H\
	\pp\ctllib\DYNDLG.H\
	\pp\pp\inc\PIXMAP.H\
	\pp\pp\inc\MASK.H\
	\pp\pp\inc\CURSORS.H\
	\pp\ctllib\COMMON.RH\
	\pp\ctllib\RADIO.RH\
	\pp\ctllib\CHECK.RH\
	\pp\ctllib\SLIDER.RH\
	\pp\ctllib\SPIN.RH\
	\pp\ctllib\ICON.RH\
	\pp\ctllib\PICKER.RH\
	\pp\ctllib\GRID.RH\
	\pp\ctllib\LISTBOX.RH\
	\pp\ctllib\STICK.RH\
	\pp\ctllib\LUTCTL.RH\
	\pp\ctllib\WINXEXT.H\
	\pp\ctllib\DLGCAT.H\
	\pp\ctllib\DLGSEC.H\
	\pp\ctllib\DLGRES.H

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/BSRDLGS.OBJ :  $(SOURCE)  $(DEP_BSRDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/BSRDLGS.OBJ :  $(SOURCE)  $(DEP_BSRDL) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Middle"

$(INTDIR)/BSRDLGS.OBJ :  $(SOURCE)  $(DEP_BSRDL) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
