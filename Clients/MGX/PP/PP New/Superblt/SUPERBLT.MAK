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
!MESSAGE NMAKE /f "superblt.mak" CFG="Win32 Debug"
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

ALL : .\WinRel\superblt.lib .\WinRel\superblt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /ML /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NO_MFC" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NO_MFC" /FR$(INTDIR)/ /Fp$(OUTDIR)/"superblt.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"superblt.bsc" 
BSC32_SBRS= \
	.\WinRel\conv8.sbr \
	.\WinRel\conv4.sbr \
	.\WinRel\superblt.sbr \
	.\WinRel\conv24.sbr \
	.\WinRel\convert.sbr

.\WinRel\superblt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"superblt.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	.\WinRel\conv8.obj \
	.\WinRel\conv4.obj \
	.\WinRel\superblt.obj \
	.\WinRel\conv24.obj \
	.\WinRel\convert.obj

.\WinRel\superblt.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

ALL : .\WinDebug\superblt.lib .\WinDebug\superblt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NO_MFC" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NO_MFC" /FR$(INTDIR)/ /Fp$(OUTDIR)/"superblt.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"superblt.bsc" 
BSC32_SBRS= \
	.\WinDebug\conv8.sbr \
	.\WinDebug\conv4.sbr \
	.\WinDebug\superblt.sbr \
	.\WinDebug\conv24.sbr \
	.\WinDebug\convert.sbr

.\WinDebug\superblt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"superblt.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	.\WinDebug\conv8.obj \
	.\WinDebug\conv4.obj \
	.\WinDebug\superblt.obj \
	.\WinDebug\conv24.obj \
	.\WinDebug\convert.obj

.\WinDebug\superblt.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

SOURCE=.\conv8.cpp
DEP_CONV8=\
	.\superblt.h\
	.\convert.h\
	\pp60\mgxframe\framelib.h\
	\pp60\imgutil\types.h\
	\pp60\mgxframe\mgxframe.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\conv8.obj :  $(SOURCE)  $(DEP_CONV8) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\conv8.obj :  $(SOURCE)  $(DEP_CONV8) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\conv4.cpp
DEP_CONV4=\
	.\superblt.h\
	.\convert.h\
	\pp60\mgxframe\framelib.h\
	\pp60\imgutil\types.h\
	\pp60\mgxframe\mgxframe.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\conv4.obj :  $(SOURCE)  $(DEP_CONV4) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\conv4.obj :  $(SOURCE)  $(DEP_CONV4) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\superblt.cpp
DEP_SUPER=\
	.\superblt.h\
	\pp60\imgutil\imgutil.h\
	c:\wing\include\WinG.h\
	\pp60\mgxframe\framelib.h\
	\pp60\imgutil\types.h\
	\pp60\mgxframe\mgxframe.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\superblt.obj :  $(SOURCE)  $(DEP_SUPER) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\superblt.obj :  $(SOURCE)  $(DEP_SUPER) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\conv24.cpp
DEP_CONV2=\
	.\superblt.h\
	.\convert.h\
	\pp60\mgxframe\framelib.h\
	\pp60\imgutil\types.h\
	\pp60\mgxframe\mgxframe.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\conv24.obj :  $(SOURCE)  $(DEP_CONV2) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\conv24.obj :  $(SOURCE)  $(DEP_CONV2) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\convert.cpp
DEP_CONVE=\
	.\superblt.h\
	.\convert.h\
	\pp60\mgxframe\framelib.h\
	\pp60\imgutil\types.h\
	\pp60\mgxframe\mgxframe.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\convert.obj :  $(SOURCE)  $(DEP_CONVE) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\convert.obj :  $(SOURCE)  $(DEP_CONVE) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
