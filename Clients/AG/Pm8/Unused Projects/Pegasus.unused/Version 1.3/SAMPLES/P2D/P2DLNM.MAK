# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=p2dlnm - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to p2dlnm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "p2dlnm - Win32 Release" && "$(CFG)" != "p2dlnm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "p2dlnm.mak" CFG="p2dlnm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "p2dlnm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "p2dlnm - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "p2dlnm - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\..\..\bin
INTDIR=.\Release

ALL : "$(OUTDIR)\p2dlnm.exe"

CLEAN : 
	-@erase "$(INTDIR)\p2dlnm.res"
	-@erase "$(INTDIR)\Xetest.obj"
	-@erase "$(OUTDIR)\p2dlnm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O1 /I "..\..\include" /D "NDEBUG" /D PC_WATCOM=0 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O1 /I "..\..\include" /D "NDEBUG" /D PC_WATCOM=0\
 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__" /Fp"$(INTDIR)/p2dlnm.pch"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/p2dlnm.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/p2dlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/p2dlnm.pdb" /machine:I386 /out:"$(OUTDIR)/p2dlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\p2dlnm.res" \
	"$(INTDIR)\Xetest.obj" \
	"..\..\Lib\pn1313lm.lib" \
	"..\..\Lib\pn13lm.lib"

"$(OUTDIR)\p2dlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\..\..\bin
INTDIR=.\Debug

ALL : "$(OUTDIR)\p2dlnm.exe"

CLEAN : 
	-@erase "$(INTDIR)\p2dlnm.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\Xetest.obj"
	-@erase "$(OUTDIR)\p2dlnm.exe"
	-@erase "$(OUTDIR)\p2dlnm.ilk"
	-@erase "$(OUTDIR)\p2dlnm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "_DEBUG" /D PC_WATCOM=0 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 PC_WATCOM=0 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__"\
 /Fp"$(INTDIR)/p2dlnm.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/p2dlnm.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/p2dlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/p2dlnm.pdb" /debug /machine:I386 /out:"$(OUTDIR)/p2dlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\p2dlnm.res" \
	"$(INTDIR)\Xetest.obj" \
	"..\..\Lib\pn1313lm.lib" \
	"..\..\Lib\pn13lm.lib"

"$(OUTDIR)\p2dlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "p2dlnm - Win32 Release"
# Name "p2dlnm - Win32 Debug"

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\p2dlnm.rc

"$(INTDIR)\p2dlnm.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Xetest.c
DEP_CPP_XETES=\
	"..\..\include\bmp.h"\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	"..\..\include\picclib.h"\
	"..\..\include\stdtypes.h"\
	

"$(INTDIR)\Xetest.obj" : $(SOURCE) $(DEP_CPP_XETES) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Pegasus\Lib\pn13lm.lib

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Pegasus\Lib\pn1313lm.lib

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
