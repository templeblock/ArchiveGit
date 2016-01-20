# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mgxframe.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Release"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

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

ALL : $(OUTDIR)/mgxframe.dll $(OUTDIR)/mgxframe.map $(OUTDIR)/mgxframe.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "C_CODE" /D "_WINDLL" /D "USE_CFILE" /D "_MBCS" /FR /c /Tp
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WIN32" /D "C_CODE" /D "_WINDLL" /D "USE_CFILE" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mgxframe.pch" /Fo$(INTDIR)/ /c /Tp 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mgxframe.bsc" 
BSC32_SBRS= \
	$(INTDIR)/frmmini.sbr \
	$(INTDIR)/frmdtcon.sbr \
	$(INTDIR)/fixed.sbr \
	$(INTDIR)/frmsmple.sbr \
	$(INTDIR)/frmmain.sbr \
	$(INTDIR)/frmdib.sbr \
	$(INTDIR)/palette.sbr \
	$(INTDIR)/frmpixel.sbr \
	$(INTDIR)/frmutils.sbr \
	$(INTDIR)/frmhsl.sbr \
	$(INTDIR)/frameold.sbr

$(OUTDIR)/mgxframe.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /SUBSYSTEM:windows /DLL /MAP /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /SUBSYSTEM:windows /DLL\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"mgxframe.pdb" /MAP:$(INTDIR)/"mgxframe.map"\
 /MACHINE:I386 /DEF:".\MGXFRAME.DEF" /OUT:$(OUTDIR)/"mgxframe.dll"\
 /IMPLIB:$(OUTDIR)/"mgxframe.lib" 
DEF_FILE=.\MGXFRAME.DEF
LINK32_OBJS= \
	$(INTDIR)/frmmini.obj \
	$(INTDIR)/frmdtcon.obj \
	$(INTDIR)/fixed.obj \
	$(INTDIR)/frmsmple.obj \
	$(INTDIR)/frmmain.obj \
	$(INTDIR)/frmdib.obj \
	$(INTDIR)/palette.obj \
	$(INTDIR)/frmpixel.obj \
	$(INTDIR)/frmutils.obj \
	$(INTDIR)/frmhsl.obj \
	$(INTDIR)/frameold.obj

$(OUTDIR)/mgxframe.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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

ALL : $(OUTDIR)/mgxframe.dll $(OUTDIR)/mgxframe.map $(OUTDIR)/mgxframe.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "C_CODE" /D "_WINDLL" /D "USE_CFILE" /D "_MBCS" /FR /c /Tp
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WIN32" /D "C_CODE" /D "_WINDLL" /D "USE_CFILE" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"mgxframe.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"mgxframe.pdb" /c /Tp 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"mgxframe.bsc" 
BSC32_SBRS= \
	$(INTDIR)/frmmini.sbr \
	$(INTDIR)/frmdtcon.sbr \
	$(INTDIR)/fixed.sbr \
	$(INTDIR)/frmsmple.sbr \
	$(INTDIR)/frmmain.sbr \
	$(INTDIR)/frmdib.sbr \
	$(INTDIR)/palette.sbr \
	$(INTDIR)/frmpixel.sbr \
	$(INTDIR)/frmutils.sbr \
	$(INTDIR)/frmhsl.sbr \
	$(INTDIR)/frameold.sbr

$(OUTDIR)/mgxframe.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /SUBSYSTEM:windows /DLL /INCREMENTAL:no /MAP /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /SUBSYSTEM:windows /DLL\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"mgxframe.pdb" /MAP:$(INTDIR)/"mgxframe.map"\
 /DEBUG /MACHINE:I386 /DEF:".\MGXFRAME.DEF" /OUT:$(OUTDIR)/"mgxframe.dll"\
 /IMPLIB:$(OUTDIR)/"mgxframe.lib" 
DEF_FILE=.\MGXFRAME.DEF
LINK32_OBJS= \
	$(INTDIR)/frmmini.obj \
	$(INTDIR)/frmdtcon.obj \
	$(INTDIR)/fixed.obj \
	$(INTDIR)/frmsmple.obj \
	$(INTDIR)/frmmain.obj \
	$(INTDIR)/frmdib.obj \
	$(INTDIR)/palette.obj \
	$(INTDIR)/frmpixel.obj \
	$(INTDIR)/frmutils.obj \
	$(INTDIR)/frmhsl.obj \
	$(INTDIR)/frameold.obj

$(OUTDIR)/mgxframe.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE=.\frmmini.cpp
DEP_FRMMI=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmmini.obj :  $(SOURCE)  $(DEP_FRMMI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmdtcon.cpp
DEP_FRMDT=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmdtcon.obj :  $(SOURCE)  $(DEP_FRMDT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fixed.cpp
DEP_FIXED=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/fixed.obj :  $(SOURCE)  $(DEP_FIXED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmsmple.cpp
DEP_FRMSM=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmsmple.obj :  $(SOURCE)  $(DEP_FRMSM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmmain.cpp
DEP_FRMMA=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmmain.obj :  $(SOURCE)  $(DEP_FRMMA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmdib.cpp
DEP_FRMDI=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmdib.obj :  $(SOURCE)  $(DEP_FRMDI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palette.cpp
DEP_PALET=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/palette.obj :  $(SOURCE)  $(DEP_PALET) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmpixel.cpp
DEP_FRMPI=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmpixel.obj :  $(SOURCE)  $(DEP_FRMPI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MGXFRAME.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmutils.cpp
DEP_FRMUT=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmutils.obj :  $(SOURCE)  $(DEP_FRMUT) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frmhsl.cpp
DEP_FRMHS=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frmhsl.obj :  $(SOURCE)  $(DEP_FRMHS) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\frameold.cpp
DEP_FRAME=\
	.\MGXFRAME.H\
	.\FRAMEINT.H

$(INTDIR)/frameold.obj :  $(SOURCE)  $(DEP_FRAME) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
