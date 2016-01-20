# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=MSPCK32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to MSPCK32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MSPCK32 - Win32 Release" && "$(CFG)" !=\
 "MSPCK32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSPCK32.MAK" CFG="MSPCK32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MSPCK32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MSPCK32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "MSPCK32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "MSPCK32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\MSPCK32.dll"

CLEAN : 
	-@erase "$(INTDIR)\MSPACKET.OBJ"
	-@erase "$(INTDIR)\MSPCK32.pch"
	-@erase "$(INTDIR)\MSPCKDLL.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(OUTDIR)\MSPCK32.dll"
	-@erase "$(OUTDIR)\MSPCK32.exp"
	-@erase "$(OUTDIR)\MSPCK32.lib"
	-@erase ".\FULL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSPCK32.pch" /Yu"STDAFX.H"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MSPCK32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib ldllcew.lib /nologo /subsystem:windows /dll /map:"FULL" /machine:IX86
# ADD LINK32 /nologo /subsystem:windows /dll /map:"FULL" /machine:IX86
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/MSPCK32.pdb" /map:"FULL" /machine:IX86 /def:".\mspck32.def"\
 /out:"$(OUTDIR)/MSPCK32.dll" /implib:"$(OUTDIR)/MSPCK32.lib" 
DEF_FILE= \
	".\mspck32.def"
LINK32_OBJS= \
	"$(INTDIR)\MSPACKET.OBJ" \
	"$(INTDIR)\MSPCKDLL.OBJ" \
	"$(INTDIR)\STDAFX.OBJ"

"$(OUTDIR)\MSPCK32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MSPCK32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\MSPCK32.dll"

CLEAN : 
	-@erase "$(INTDIR)\MSPACKET.OBJ"
	-@erase "$(INTDIR)\MSPCK32.pch"
	-@erase "$(INTDIR)\MSPCKDLL.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\MSPCK32.dll"
	-@erase "$(OUTDIR)\MSPCK32.exp"
	-@erase "$(OUTDIR)\MSPCK32.ilk"
	-@erase "$(OUTDIR)\MSPCK32.lib"
	-@erase "$(OUTDIR)\MSPCK32.pdb"
	-@erase ".\FULL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"STDAFX.H" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSPCK32.pch" /Yu"STDAFX.H"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MSPCK32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib ldllcew.lib /nologo /subsystem:windows /dll /map:"FULL" /debug /machine:IX86
# ADD LINK32 /nologo /subsystem:windows /dll /map:"FULL" /debug /machine:IX86
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/MSPCK32.pdb" /map:"FULL" /debug /machine:IX86\
 /def:".\mspck32.def" /out:"$(OUTDIR)/MSPCK32.dll"\
 /implib:"$(OUTDIR)/MSPCK32.lib" 
DEF_FILE= \
	".\mspck32.def"
LINK32_OBJS= \
	"$(INTDIR)\MSPACKET.OBJ" \
	"$(INTDIR)\MSPCKDLL.OBJ" \
	"$(INTDIR)\STDAFX.OBJ"

"$(OUTDIR)\MSPCK32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "MSPCK32 - Win32 Release"
# Name "MSPCK32 - Win32 Debug"

!IF  "$(CFG)" == "MSPCK32 - Win32 Release"

!ELSEIF  "$(CFG)" == "MSPCK32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "MSPCK32 - Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSPCK32.pch" /Yc"STDAFX.H"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MSPCK32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "MSPCK32 - Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSPCK32.pch" /Yc"STDAFX.H"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MSPCK32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSPCKDLL.CPP
DEP_CPP_MSPCK=\
	".\inifile.h"\
	".\mspacket.h"\
	".\MSPCKDLL.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MSPCKDLL.OBJ" : $(SOURCE) $(DEP_CPP_MSPCK) "$(INTDIR)"\
 "$(INTDIR)\MSPCK32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSPACKET.CPP
DEP_CPP_MSPAC=\
	".\mspacket.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MSPACKET.OBJ" : $(SOURCE) $(DEP_CPP_MSPAC) "$(INTDIR)"\
 "$(INTDIR)\MSPCK32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mspck32.def

!IF  "$(CFG)" == "MSPCK32 - Win32 Release"

!ELSEIF  "$(CFG)" == "MSPCK32 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
