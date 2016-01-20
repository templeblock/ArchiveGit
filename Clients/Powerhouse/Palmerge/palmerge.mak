# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=palmerge - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to palmerge - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "palmerge - Win32 Debug" && "$(CFG)" !=\
 "palmerge - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "palmerge.mak" CFG="palmerge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "palmerge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "palmerge - Win32 Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "palmerge - Win32 Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "palmerge - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 4
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\palmerge.exe"

CLEAN : 
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\Pal.obj"
	-@erase "$(INTDIR)\palio.obj"
	-@erase "$(INTDIR)\palmedlg.obj"
	-@erase "$(INTDIR)\palmerge.obj"
	-@erase "$(INTDIR)\palmerge.pch"
	-@erase "$(INTDIR)\palmerge.res"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\palmerge.exe"
	-@erase "$(OUTDIR)\palmerge.ilk"
	-@erase "$(OUTDIR)\palmerge.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/palmerge.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/palmerge.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/palmerge.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/palmerge.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\Pal.obj" \
	"$(INTDIR)\palio.obj" \
	"$(INTDIR)\palmedlg.obj" \
	"$(INTDIR)\palmerge.obj" \
	"$(INTDIR)\palmerge.res" \
	"$(INTDIR)\stdafx.obj" \
	".\Wing32.lib"

"$(OUTDIR)\palmerge.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 4
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\palmerge.exe"

CLEAN : 
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\Pal.obj"
	-@erase "$(INTDIR)\palio.obj"
	-@erase "$(INTDIR)\palmedlg.obj"
	-@erase "$(INTDIR)\palmerge.obj"
	-@erase "$(INTDIR)\palmerge.pch"
	-@erase "$(INTDIR)\palmerge.res"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(OUTDIR)\palmerge.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/palmerge.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/palmerge.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/palmerge.pdb" /machine:I386 /out:"$(OUTDIR)/palmerge.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\Pal.obj" \
	"$(INTDIR)\palio.obj" \
	"$(INTDIR)\palmedlg.obj" \
	"$(INTDIR)\palmerge.obj" \
	"$(INTDIR)\palmerge.res" \
	"$(INTDIR)\stdafx.obj" \
	".\Wing32.lib"

"$(OUTDIR)\palmerge.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

MTL_PROJ=
################################################################################
# Begin Target

# Name "palmerge - Win32 Debug"
# Name "palmerge - Win32 Release"

!IF  "$(CFG)" == "palmerge - Win32 Debug"

!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "palmerge - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/palmerge.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\palmerge.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/palmerge.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\palmerge.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palmerge.cpp

!IF  "$(CFG)" == "palmerge - Win32 Debug"

DEP_CPP_PALME=\
	".\Dib.h"\
	".\pal.h"\
	".\palio.h"\
	".\palmedlg.h"\
	".\palmerge.h"\
	".\Phtypes.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palmerge.obj" : $(SOURCE) $(DEP_CPP_PALME) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

DEP_CPP_PALME=\
	".\Dib.h"\
	".\pal.h"\
	".\palio.h"\
	".\palmedlg.h"\
	".\palmerge.h"\
	".\Phtypes.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palmerge.obj" : $(SOURCE) $(DEP_CPP_PALME) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palmedlg.cpp
DEP_CPP_PALMED=\
	".\palmedlg.h"\
	".\palmerge.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palmedlg.obj" : $(SOURCE) $(DEP_CPP_PALMED) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\palmerge.rc
DEP_RSC_PALMER=\
	".\res\palmerge.ico"\
	".\res\palmerge.rc2"\
	

"$(INTDIR)\palmerge.res" : $(SOURCE) $(DEP_RSC_PALMER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "palmerge - Win32 Debug"

!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\palio.cpp
DEP_CPP_PALIO=\
	".\palio.h"\
	".\stdafx.h"\
	

"$(INTDIR)\palio.obj" : $(SOURCE) $(DEP_CPP_PALIO) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pal.cpp
DEP_CPP_PAL_C=\
	".\pal.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dib.cpp

!IF  "$(CFG)" == "palmerge - Win32 Debug"

DEP_CPP_DIB_C=\
	".\Dib.h"\
	".\Phtypes.h"\
	".\stdafx.h"\
	".\Wing.h"\
	

"$(INTDIR)\Dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

DEP_CPP_DIB_C=\
	".\Dib.h"\
	".\Phtypes.h"\
	".\stdafx.h"\
	".\Wing.h"\
	

"$(INTDIR)\Dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\palmerge.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Wing32.lib

!IF  "$(CFG)" == "palmerge - Win32 Debug"

!ELSEIF  "$(CFG)" == "palmerge - Win32 Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
