# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=msreg32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msreg32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msreg32 - Win32 Debug" && "$(CFG)" !=\
 "msreg32 - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msreg32.mak" CFG="msreg32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msreg32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msreg32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "msreg32 - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "msreg32 - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\msreg32.dll"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\msreg32.dll"
	-@erase ".\WinDebug\COMMAND.OBJ"
	-@erase ".\WinDebug\DRIVEMGR.OBJ"
	-@erase ".\WinDebug\MSREGDLL.OBJ"
	-@erase ".\WinDebug\msreg32.ilk"
	-@erase ".\WinDebug\msreg32.lib"
	-@erase ".\WinDebug\msreg32.exp"
	-@erase ".\WinDebug\msreg32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/msreg32.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x0 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msreg32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /def:"MSREGDLL.def" /implib:"MSREGDLL.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map
LINK32_FLAGS=user32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/msreg32.pdb" /debug /machine:I386 /def:".\MSREG32.DEF"\
 /out:"$(OUTDIR)/msreg32.dll" /implib:"$(OUTDIR)/msreg32.lib" 
DEF_FILE= \
	".\MSREG32.DEF"
LINK32_OBJS= \
	"$(INTDIR)/COMMAND.OBJ" \
	"$(INTDIR)/DRIVEMGR.OBJ" \
	"$(INTDIR)/MSREGDLL.OBJ"

"$(OUTDIR)\msreg32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msreg32 - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\msreg32.dll"

CLEAN : 
	-@erase ".\WinRel\msreg32.dll"
	-@erase ".\WinRel\COMMAND.OBJ"
	-@erase ".\WinRel\DRIVEMGR.OBJ"
	-@erase ".\WinRel\MSREGDLL.OBJ"
	-@erase ".\WinRel\msreg32.lib"
	-@erase ".\WinRel\msreg32.exp"
	-@erase ".\WinRel\msreg32.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /W3 /O1 /Ob0 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /O1 /Ob0 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/msreg32.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE RSC /l 0x0 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msreg32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:"MSREGDLL.def" /implib:"MSREGDLL.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib /nologo /subsystem:windows /dll /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=user32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/msreg32.pdb" /map:"$(INTDIR)/msreg32.map" /machine:I386\
 /def:".\MSREG32.DEF" /out:"$(OUTDIR)/msreg32.dll"\
 /implib:"$(OUTDIR)/msreg32.lib" 
DEF_FILE= \
	".\MSREG32.DEF"
LINK32_OBJS= \
	"$(INTDIR)/COMMAND.OBJ" \
	"$(INTDIR)/DRIVEMGR.OBJ" \
	"$(INTDIR)/MSREGDLL.OBJ"

"$(OUTDIR)\msreg32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=

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

# Name "msreg32 - Win32 Debug"
# Name "msreg32 - Win32 Release"

!IF  "$(CFG)" == "msreg32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "msreg32 - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\MSREGDLL.CPP
DEP_CPP_MSREG=\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGENT.H"\
	".\COMMAND.H"\
	".\DRIVEMGR.H"\
	

"$(INTDIR)\MSREGDLL.OBJ" : $(SOURCE) $(DEP_CPP_MSREG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\COMMAND.CPP
DEP_CPP_COMMA=\
	".\COMMAND.H"\
	

"$(INTDIR)\COMMAND.OBJ" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSREG32.DEF

!IF  "$(CFG)" == "msreg32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "msreg32 - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DRIVEMGR.CPP
DEP_CPP_DRIVE=\
	".\DRIVEMGR.H"\
	

"$(INTDIR)\DRIVEMGR.OBJ" : $(SOURCE) $(DEP_CPP_DRIVE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
