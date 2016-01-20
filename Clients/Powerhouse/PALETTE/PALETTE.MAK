# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "PALETTE.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "WinRel"
OUTDIR=.
INTDIR=.\WinRel

ALL : $(OUTDIR)/PALETTE.exe $(OUTDIR)/PALETTE.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"PALETTE.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"PALETTE.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"PALETTE.bsc" 
BSC32_SBRS= \
	$(INTDIR)/STDAFX.SBR \
	$(INTDIR)/PALETTE.SBR \
	$(INTDIR)/MAINFRM.SBR \
	$(INTDIR)/PALDOC.SBR \
	$(INTDIR)/PALVW.SBR \
	$(INTDIR)/DIBPAL.SBR \
	$(INTDIR)/DIB.SBR \
	$(INTDIR)/COLORDLG.SBR

$(OUTDIR)/PALETTE.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcw.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
# ADD LINK32 nafxcw.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:IX86
LINK32_FLAGS=nafxcw.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"PALETTE.pdb" /MACHINE:IX86\
 /OUT:$(OUTDIR)/"PALETTE.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/PALETTE.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/PALETTE.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/PALDOC.OBJ \
	$(INTDIR)/PALVW.OBJ \
	$(INTDIR)/DIBPAL.OBJ \
	$(INTDIR)/DIB.OBJ \
	$(INTDIR)/COLORDLG.OBJ

$(OUTDIR)/PALETTE.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.
INTDIR=.\WinDebug

ALL : $(OUTDIR)/PALETTE.exe $(OUTDIR)/PALETTE.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_MBCS" /FR /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"PALETTE.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"PALETTE.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"PALETTE.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"PALETTE.bsc" 
BSC32_SBRS= \
	$(INTDIR)/STDAFX.SBR \
	$(INTDIR)/PALETTE.SBR \
	$(INTDIR)/MAINFRM.SBR \
	$(INTDIR)/PALDOC.SBR \
	$(INTDIR)/PALVW.SBR \
	$(INTDIR)/DIBPAL.SBR \
	$(INTDIR)/DIB.SBR \
	$(INTDIR)/COLORDLG.SBR

$(OUTDIR)/PALETTE.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcwd.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
# ADD LINK32 nafxcwd.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:IX86
LINK32_FLAGS=nafxcwd.lib olecli32.lib olesvr32.lib winmm.lib /NOLOGO\
 /SUBSYSTEM:windows /INCREMENTAL:yes /PDB:$(OUTDIR)/"PALETTE.pdb" /DEBUG\
 /MACHINE:IX86 /OUT:$(OUTDIR)/"PALETTE.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/PALETTE.res \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/PALETTE.OBJ \
	$(INTDIR)/MAINFRM.OBJ \
	$(INTDIR)/PALDOC.OBJ \
	$(INTDIR)/PALVW.OBJ \
	$(INTDIR)/DIBPAL.OBJ \
	$(INTDIR)/DIB.OBJ \
	$(INTDIR)/COLORDLG.OBJ

$(OUTDIR)/PALETTE.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\PALETTE.RC
DEP_PALET=\
	.\RES\PALETTE.ICO\
	.\RES\TOOLBAR.BMP\
	.\RES\PALETTE.RC2

$(INTDIR)/PALETTE.res :  $(SOURCE)  $(DEP_PALET) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\STDAFX.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"PALETTE.pch" /Yc"STDAFX.H"\
 /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"PALETTE.pch" /Yc"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"PALETTE.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALETTE.CPP
DEP_PALETT=\
	.\STDAFX.H\
	.\PALETTE.H\
	.\DIB.H\
	.\DIBPAL.H\
	.\MAINFRM.H\
	.\PALDOC.H\
	.\PALVW.H

$(INTDIR)/PALETTE.OBJ :  $(SOURCE)  $(DEP_PALETT) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAINFRM.CPP
DEP_MAINF=\
	.\STDAFX.H\
	.\PALETTE.H\
	.\MAINFRM.H

$(INTDIR)/MAINFRM.OBJ :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALDOC.CPP
DEP_PALDO=\
	.\STDAFX.H\
	.\PALETTE.H\
	.\DIB.H\
	.\DIBPAL.H\
	.\PALDOC.H

$(INTDIR)/PALDOC.OBJ :  $(SOURCE)  $(DEP_PALDO) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PALVW.CPP
DEP_PALVW=\
	.\STDAFX.H\
	.\PALETTE.H\
	.\DIB.H\
	.\DIBPAL.H\
	.\PALDOC.H\
	.\PALVW.H

$(INTDIR)/PALVW.OBJ :  $(SOURCE)  $(DEP_PALVW) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIBPAL.CPP
DEP_DIBPA=\
	.\STDAFX.H\
	.\DIBPAL.H\
	.\DIB.H

$(INTDIR)/DIBPAL.OBJ :  $(SOURCE)  $(DEP_DIBPA) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIB.CPP
DEP_DIB_C=\
	.\STDAFX.H\
	.\DIB.H

$(INTDIR)/DIB.OBJ :  $(SOURCE)  $(DEP_DIB_C) $(INTDIR) $(INTDIR)/STDAFX.OBJ

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COLORDLG.CPP
DEP_COLOR=\
	.\STDAFX.H\
	.\DIB.H\
	.\DIBPAL.H\
	.\PALETTE.H

$(INTDIR)/COLORDLG.OBJ :  $(SOURCE)  $(DEP_COLOR) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ

# End Source File
# End Group
# End Project
################################################################################
