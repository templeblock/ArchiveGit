# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=msrun32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msrun32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msrun32 - Win32 Release" && "$(CFG)" !=\
 "msrun32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msrun32.mak" CFG="msrun32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msrun32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "msrun32 - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "msrun32 - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "msrun32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Winrel"
# PROP Intermediate_Dir "Winrel"
# PROP Target_Dir ""
OUTDIR=.\Winrel
INTDIR=.\Winrel

ALL : "$(OUTDIR)\msrun32.exe"

CLEAN : 
	-@erase ".\Winrel\msrun32.exe"
	-@erase ".\Winrel\Drivemgr.obj"
	-@erase ".\Winrel\msrun.obj"
	-@erase ".\Winrel\msrun.res"
	-@erase ".\Winrel\globatom.obj"
	-@erase ".\Winrel\msrun32.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
# ADD CPP /nologo /G3 /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /G3 /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/msrun32.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Winrel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msrun.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msrun32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /map /machine:IX86
# ADD LINK32 oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x2800 /subsystem:windows /map /machine:IX86
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /stack:0x2800 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/msrun32.pdb" /map:"$(INTDIR)/msrun32.map"\
 /machine:IX86 /out:"$(OUTDIR)/msrun32.exe" 
LINK32_OBJS= \
	".\Winrel\Drivemgr.obj" \
	".\Winrel\msrun.obj" \
	".\Winrel\globatom.obj" \
	".\Winrel\msrun.res"

"$(OUTDIR)\msrun32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msrun32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Windebug"
# PROP Intermediate_Dir "Windebug"
# PROP Target_Dir ""
OUTDIR=.\Windebug
INTDIR=.\Windebug

ALL : "$(OUTDIR)\msrun32.exe"

CLEAN : 
	-@erase ".\Windebug\vc40.pdb"
	-@erase ".\Windebug\vc40.idb"
	-@erase ".\Windebug\msrun32.exe"
	-@erase ".\Windebug\Drivemgr.obj"
	-@erase ".\Windebug\msrun.obj"
	-@erase ".\Windebug\msrun.res"
	-@erase ".\Windebug\globatom.obj"
	-@erase ".\Windebug\msrun32.ilk"
	-@erase ".\Windebug\msrun32.pdb"
	-@erase ".\Windebug\msrun32.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/msrun32.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Windebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msrun.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msrun32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /map /debug /machine:IX86
# ADD LINK32 oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x2800 /subsystem:windows /map /debug /machine:IX86
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /stack:0x2800 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/msrun32.pdb" /map:"$(INTDIR)/msrun32.map"\
 /debug /machine:IX86 /out:"$(OUTDIR)/msrun32.exe" 
LINK32_OBJS= \
	".\Windebug\Drivemgr.obj" \
	".\Windebug\msrun.obj" \
	".\Windebug\globatom.obj" \
	".\Windebug\msrun.res"

"$(OUTDIR)\msrun32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "msrun32 - Win32 Release"
# Name "msrun32 - Win32 Debug"

!IF  "$(CFG)" == "msrun32 - Win32 Release"

!ELSEIF  "$(CFG)" == "msrun32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Drivemgr.cpp
DEP_CPP_DRIVE=\
	".\drivemgr.h"\
	

"$(INTDIR)\Drivemgr.obj" : $(SOURCE) $(DEP_CPP_DRIVE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msrun.CPP
DEP_CPP_MSRUN=\
	".\msregini.h"\
	".\drivemgr.h"\
	".\globatom.h"\
	

"$(INTDIR)\msrun.obj" : $(SOURCE) $(DEP_CPP_MSRUN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msrun.rc
DEP_RSC_MSRUN_=\
	".\res\treasure.ico"\
	".\res\newprod.ico"\
	

"$(INTDIR)\msrun.res" : $(SOURCE) $(DEP_RSC_MSRUN_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\globatom.cpp
DEP_CPP_GLOBA=\
	".\globatom.h"\
	

"$(INTDIR)\globatom.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
