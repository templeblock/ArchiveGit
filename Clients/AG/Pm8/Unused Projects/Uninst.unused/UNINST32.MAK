# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Uninst32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Uninst32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Uninst32 - Win32 Debug" && "$(CFG)" !=\
 "Uninst32 - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "UNINST32.MAK" CFG="Uninst32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Uninst32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Uninst32 - Win32 Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Uninst32 - Win32 Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Uninst32 - Win32 Debug"

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

ALL : "$(OUTDIR)\UNINST32.exe"

CLEAN : 
	-@erase "$(INTDIR)\DDE.OBJ"
	-@erase "$(INTDIR)\deldlg.obj"
	-@erase "$(INTDIR)\Inifile.obj"
	-@erase "$(INTDIR)\progdlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Uninsdlg.obj"
	-@erase "$(INTDIR)\UNINST32.pch"
	-@erase "$(INTDIR)\uninstal.obj"
	-@erase "$(INTDIR)\uninstal.res"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\UNINST32.exe"
	-@erase "$(OUTDIR)\UNINST32.ilk"
	-@erase "$(OUTDIR)\UNINST32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/UNINST32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/uninstal.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UNINST32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/UNINST32.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/UNINST32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDE.OBJ" \
	"$(INTDIR)\deldlg.obj" \
	"$(INTDIR)\Inifile.obj" \
	"$(INTDIR)\progdlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Uninsdlg.obj" \
	"$(INTDIR)\uninstal.obj" \
	"$(INTDIR)\uninstal.res" \
	"$(INTDIR)\UTIL.OBJ"

"$(OUTDIR)\UNINST32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Uninst32 - Win32 Release"

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

ALL : "$(OUTDIR)\UNINST32.exe"

CLEAN : 
	-@erase "$(INTDIR)\DDE.OBJ"
	-@erase "$(INTDIR)\deldlg.obj"
	-@erase "$(INTDIR)\Inifile.obj"
	-@erase "$(INTDIR)\progdlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Uninsdlg.obj"
	-@erase "$(INTDIR)\UNINST32.pch"
	-@erase "$(INTDIR)\uninstal.obj"
	-@erase "$(INTDIR)\uninstal.res"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(OUTDIR)\UNINST32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/UNINST32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/uninstal.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UNINST32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/UNINST32.pdb" /machine:I386 /out:"$(OUTDIR)/UNINST32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DDE.OBJ" \
	"$(INTDIR)\deldlg.obj" \
	"$(INTDIR)\Inifile.obj" \
	"$(INTDIR)\progdlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Uninsdlg.obj" \
	"$(INTDIR)\uninstal.obj" \
	"$(INTDIR)\uninstal.res" \
	"$(INTDIR)\UTIL.OBJ"

"$(OUTDIR)\UNINST32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Uninst32 - Win32 Debug"
# Name "Uninst32 - Win32 Release"

!IF  "$(CFG)" == "Uninst32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Uninst32 - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Uninst32 - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/UNINST32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UNINST32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Uninst32 - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/UNINST32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UNINST32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\uninstal.cpp
DEP_CPP_UNINS=\
	".\deldlg.h"\
	".\inifile.h"\
	".\progdlg.h"\
	".\regstr16.h"\
	".\stdafx.h"\
	".\Uninsdlg.h"\
	".\uninstal.h"\
	".\UTIL.H"\
	

"$(INTDIR)\uninstal.obj" : $(SOURCE) $(DEP_CPP_UNINS) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Uninsdlg.cpp
DEP_CPP_UNINSD=\
	".\inifile.h"\
	".\stdafx.h"\
	".\Uninsdlg.h"\
	".\uninstal.h"\
	

"$(INTDIR)\Uninsdlg.obj" : $(SOURCE) $(DEP_CPP_UNINSD) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\uninstal.rc
DEP_RSC_UNINST=\
	".\res\uninstal.ico"\
	".\res\uninstal.rc2"\
	

"$(INTDIR)\uninstal.res" : $(SOURCE) $(DEP_RSC_UNINST) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Uninst32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Uninst32 - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UTIL.CPP
DEP_CPP_UTIL_=\
	".\stdafx.h"\
	".\UTIL.H"\
	

"$(INTDIR)\UTIL.OBJ" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DDE.CPP
DEP_CPP_DDE_C=\
	".\stdafx.h"\
	

"$(INTDIR)\DDE.OBJ" : $(SOURCE) $(DEP_CPP_DDE_C) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\deldlg.cpp
DEP_CPP_DELDL=\
	".\deldlg.h"\
	".\inifile.h"\
	".\stdafx.h"\
	".\uninstal.h"\
	

"$(INTDIR)\deldlg.obj" : $(SOURCE) $(DEP_CPP_DELDL) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Inifile.cpp
DEP_CPP_INIFI=\
	".\inifile.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Inifile.obj" : $(SOURCE) $(DEP_CPP_INIFI) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\progdlg.cpp
DEP_CPP_PROGD=\
	".\inifile.h"\
	".\progdlg.h"\
	".\stdafx.h"\
	".\uninstal.h"\
	

"$(INTDIR)\progdlg.obj" : $(SOURCE) $(DEP_CPP_PROGD) "$(INTDIR)"\
 "$(INTDIR)\UNINST32.pch"


# End Source File
# End Target
# End Project
################################################################################
