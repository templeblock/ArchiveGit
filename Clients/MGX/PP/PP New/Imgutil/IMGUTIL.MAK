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
!MESSAGE NMAKE /f "imgutil.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/imgutil.lib $(OUTDIR)/imgutil.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "C_CODE" /FR /c /Tp
CPP_PROJ=/nologo /MD /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WIN32" /D "C_CODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"imgutil.pch" /Fo$(INTDIR)/ /c\
 /Tp 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"imgutil.bsc" 
BSC32_SBRS= \
	$(INTDIR)/QUICK.SBR \
	$(INTDIR)/MAKEMAP.SBR \
	$(INTDIR)/tform.sbr \
	$(INTDIR)/map.sbr \
	$(INTDIR)/STRINGS.SBR \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/fileio.sbr \
	$(INTDIR)/util.sbr \
	$(INTDIR)/gamma.sbr

$(OUTDIR)/imgutil.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"imgutil.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/QUICK.OBJ \
	$(INTDIR)/MAKEMAP.OBJ \
	$(INTDIR)/tform.obj \
	$(INTDIR)/map.obj \
	$(INTDIR)/STRINGS.OBJ \
	$(INTDIR)/memory.obj \
	$(INTDIR)/fileio.obj \
	$(INTDIR)/util.obj \
	$(INTDIR)/gamma.obj

$(OUTDIR)/imgutil.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

ALL : $(OUTDIR)/imgutil.lib $(OUTDIR)/imgutil.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WIN32" /D "C_CODE" /FR /c /Tp
CPP_PROJ=/nologo /MD /W3 /GX /Z7 /YX /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WIN32" /D "C_CODE" /FR$(INTDIR)/ /Fp$(OUTDIR)/"imgutil.pch" /Fo$(INTDIR)/\
 /c /Tp 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"imgutil.bsc" 
BSC32_SBRS= \
	$(INTDIR)/QUICK.SBR \
	$(INTDIR)/MAKEMAP.SBR \
	$(INTDIR)/tform.sbr \
	$(INTDIR)/map.sbr \
	$(INTDIR)/STRINGS.SBR \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/fileio.sbr \
	$(INTDIR)/util.sbr \
	$(INTDIR)/gamma.sbr

$(OUTDIR)/imgutil.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"imgutil.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/QUICK.OBJ \
	$(INTDIR)/MAKEMAP.OBJ \
	$(INTDIR)/tform.obj \
	$(INTDIR)/map.obj \
	$(INTDIR)/STRINGS.OBJ \
	$(INTDIR)/memory.obj \
	$(INTDIR)/fileio.obj \
	$(INTDIR)/util.obj \
	$(INTDIR)/gamma.obj

$(OUTDIR)/imgutil.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

SOURCE=.\QUICK.CPP
DEP_QUICK=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/QUICK.OBJ :  $(SOURCE)  $(DEP_QUICK) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAKEMAP.CPP
DEP_MAKEM=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/MAKEMAP.OBJ :  $(SOURCE)  $(DEP_MAKEM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tform.cpp
DEP_TFORM=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/tform.obj :  $(SOURCE)  $(DEP_TFORM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\map.cpp
DEP_MAP_C=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/map.obj :  $(SOURCE)  $(DEP_MAP_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STRINGS.CPP
DEP_STRIN=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/STRINGS.OBJ :  $(SOURCE)  $(DEP_STRIN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memory.cpp
DEP_MEMOR=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileio.cpp
DEP_FILEI=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/fileio.obj :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\util.cpp
DEP_UTIL_=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/util.obj :  $(SOURCE)  $(DEP_UTIL_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\gamma.cpp
DEP_GAMMA=\
	.\imgutil.h\
	.\types.h\
	\pp60\mgxframe\mgxframe.h

$(INTDIR)/gamma.obj :  $(SOURCE)  $(DEP_GAMMA) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
