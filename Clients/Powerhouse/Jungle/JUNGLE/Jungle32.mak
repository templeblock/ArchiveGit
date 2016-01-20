# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Jungle32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Jungle32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Jungle32 - Win32 Release" && "$(CFG)" !=\
 "Jungle32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Jungle32.mak" CFG="Jungle32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Jungle32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Jungle32 - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Jungle32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\rel32"
# PROP Intermediate_Dir ".\rel32"
OUTDIR=.\rel32
INTDIR=.\rel32

ALL : ".\jungle32.exe"

CLEAN : 
	-@erase "$(INTDIR)\Bcontrol.obj"
	-@erase "$(INTDIR)\bmulti.obj"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\gcontrol.obj"
	-@erase "$(INTDIR)\hcontrol.obj"
	-@erase "$(INTDIR)\hslider.obj"
	-@erase "$(INTDIR)\httables.obj"
	-@erase "$(INTDIR)\jungle.obj"
	-@erase "$(INTDIR)\Jungle.res"
	-@erase "$(INTDIR)\Mci.obj"
	-@erase "$(INTDIR)\Mcontrol.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\offscren.obj"
	-@erase "$(INTDIR)\pal.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\Phapp.obj"
	-@erase "$(INTDIR)\puzzle2.obj"
	-@erase "$(INTDIR)\puzzle3.obj"
	-@erase "$(INTDIR)\puzzle4.obj"
	-@erase "$(INTDIR)\puzzle5.obj"
	-@erase "$(INTDIR)\randseq.obj"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Scene.obj"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\speedchk.obj"
	-@erase "$(INTDIR)\Sprite.obj"
	-@erase "$(INTDIR)\Transit.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\Vcontrol.obj"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\Videodrw.obj"
	-@erase "$(INTDIR)\Vreco.obj"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(INTDIR)\Wintools.obj"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase ".\jungle32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /G4 /W3 /GX /Ox /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /D "VOICE_RECO" /c
# SUBTRACT CPP /X /Fr
CPP_PROJ=/nologo /G4 /ML /W3 /GX /Ox /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /D "VOICE_RECO" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\.\rel32/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "NDEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\gameguts" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Jungle.res" /i "..\gameguts" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Jungle32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows /machine:IX86 /out:".\jungle32.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/jungle32.pdb" /machine:IX86\
 /out:".\jungle32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Bcontrol.obj" \
	"$(INTDIR)\bmulti.obj" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\gcontrol.obj" \
	"$(INTDIR)\hcontrol.obj" \
	"$(INTDIR)\hslider.obj" \
	"$(INTDIR)\httables.obj" \
	"$(INTDIR)\jungle.obj" \
	"$(INTDIR)\Jungle.res" \
	"$(INTDIR)\Mci.obj" \
	"$(INTDIR)\Mcontrol.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\offscren.obj" \
	"$(INTDIR)\pal.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\Phapp.obj" \
	"$(INTDIR)\puzzle2.obj" \
	"$(INTDIR)\puzzle3.obj" \
	"$(INTDIR)\puzzle4.obj" \
	"$(INTDIR)\puzzle5.obj" \
	"$(INTDIR)\randseq.obj" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\Scene.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\speedchk.obj" \
	"$(INTDIR)\Sprite.obj" \
	"$(INTDIR)\Transit.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\Vcontrol.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\Videodrw.obj" \
	"$(INTDIR)\Vreco.obj" \
	"$(INTDIR)\vtable.obj" \
	"$(INTDIR)\Wintools.obj" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\vfw32.lib" \
	"..\Gameguts\wavmix32.lib" \
	"..\Gameguts\wing32.lib" \
	".\Icwapi32.lib"

".\jungle32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\dbg32"
# PROP Intermediate_Dir ".\dbg32"
OUTDIR=.\dbg32
INTDIR=.\dbg32

ALL : ".\jungle32.exe" "$(OUTDIR)\Jungle32.bsc"

CLEAN : 
	-@erase "$(INTDIR)\Bcontrol.obj"
	-@erase "$(INTDIR)\Bcontrol.sbr"
	-@erase "$(INTDIR)\bmulti.obj"
	-@erase "$(INTDIR)\bmulti.sbr"
	-@erase "$(INTDIR)\credits.obj"
	-@erase "$(INTDIR)\credits.sbr"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\dialog.sbr"
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\Dib.sbr"
	-@erase "$(INTDIR)\gcontrol.obj"
	-@erase "$(INTDIR)\gcontrol.sbr"
	-@erase "$(INTDIR)\hcontrol.obj"
	-@erase "$(INTDIR)\hcontrol.sbr"
	-@erase "$(INTDIR)\hslider.obj"
	-@erase "$(INTDIR)\hslider.sbr"
	-@erase "$(INTDIR)\httables.obj"
	-@erase "$(INTDIR)\httables.sbr"
	-@erase "$(INTDIR)\jungle.obj"
	-@erase "$(INTDIR)\Jungle.res"
	-@erase "$(INTDIR)\jungle.sbr"
	-@erase "$(INTDIR)\Mci.obj"
	-@erase "$(INTDIR)\Mci.sbr"
	-@erase "$(INTDIR)\Mcontrol.obj"
	-@erase "$(INTDIR)\Mcontrol.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\offscren.obj"
	-@erase "$(INTDIR)\offscren.sbr"
	-@erase "$(INTDIR)\pal.obj"
	-@erase "$(INTDIR)\pal.sbr"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\Parser.sbr"
	-@erase "$(INTDIR)\Phapp.obj"
	-@erase "$(INTDIR)\Phapp.sbr"
	-@erase "$(INTDIR)\puzzle2.obj"
	-@erase "$(INTDIR)\puzzle2.sbr"
	-@erase "$(INTDIR)\puzzle3.obj"
	-@erase "$(INTDIR)\puzzle3.sbr"
	-@erase "$(INTDIR)\puzzle4.obj"
	-@erase "$(INTDIR)\puzzle4.sbr"
	-@erase "$(INTDIR)\puzzle5.obj"
	-@erase "$(INTDIR)\puzzle5.sbr"
	-@erase "$(INTDIR)\randseq.obj"
	-@erase "$(INTDIR)\randseq.sbr"
	-@erase "$(INTDIR)\Rcontrol.obj"
	-@erase "$(INTDIR)\Rcontrol.sbr"
	-@erase "$(INTDIR)\Scene.obj"
	-@erase "$(INTDIR)\Scene.sbr"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\script.sbr"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\Sound.sbr"
	-@erase "$(INTDIR)\speedchk.obj"
	-@erase "$(INTDIR)\speedchk.sbr"
	-@erase "$(INTDIR)\Sprite.obj"
	-@erase "$(INTDIR)\Sprite.sbr"
	-@erase "$(INTDIR)\Transit.obj"
	-@erase "$(INTDIR)\Transit.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\Vcontrol.obj"
	-@erase "$(INTDIR)\Vcontrol.sbr"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\video.sbr"
	-@erase "$(INTDIR)\Videodrw.obj"
	-@erase "$(INTDIR)\Videodrw.sbr"
	-@erase "$(INTDIR)\Vreco.obj"
	-@erase "$(INTDIR)\Vreco.sbr"
	-@erase "$(INTDIR)\vtable.obj"
	-@erase "$(INTDIR)\vtable.sbr"
	-@erase "$(INTDIR)\Wintools.obj"
	-@erase "$(INTDIR)\Wintools.sbr"
	-@erase "$(INTDIR)\Worlds.obj"
	-@erase "$(INTDIR)\Worlds.sbr"
	-@erase "$(OUTDIR)\Jungle32.bsc"
	-@erase "$(OUTDIR)\jungle32.pdb"
	-@erase ".\DEBUG32.IDB"
	-@erase ".\DEBUG32.PDB"
	-@erase ".\jungle32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G3 /MT /W3 /GX /Zi /Od /Ob1 /Gy /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /FR /YX /Fd"DEBUG.PDB" /c
# ADD CPP /nologo /G4 /W3 /Gm /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /D "VOICE_RECO" /Fr /Fd"DEBUG32.PDB" /c
# SUBTRACT CPP /X
CPP_PROJ=/nologo /G4 /MLd /W3 /Gm /GX /Zi /Od /Ob2 /Gy /I "..\gameguts" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_MBCS" /D "VOICE_RECO"\
 /Fr"$(INTDIR)/" /Fo"$(INTDIR)/" /Fd"DEBUG32.PDB" /c 
CPP_OBJS=.\.\dbg32/
CPP_SBRS=.\.\dbg32/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /I "..\gameguts" /D "_DEBUG" /win32
MTL_PROJ=/nologo /I "..\gameguts" /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\gameguts" /d "_DEBUG"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Jungle.res" /i "..\gameguts" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Jungle32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Bcontrol.sbr" \
	"$(INTDIR)\bmulti.sbr" \
	"$(INTDIR)\credits.sbr" \
	"$(INTDIR)\dialog.sbr" \
	"$(INTDIR)\Dib.sbr" \
	"$(INTDIR)\gcontrol.sbr" \
	"$(INTDIR)\hcontrol.sbr" \
	"$(INTDIR)\hslider.sbr" \
	"$(INTDIR)\httables.sbr" \
	"$(INTDIR)\jungle.sbr" \
	"$(INTDIR)\Mci.sbr" \
	"$(INTDIR)\Mcontrol.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\offscren.sbr" \
	"$(INTDIR)\pal.sbr" \
	"$(INTDIR)\Parser.sbr" \
	"$(INTDIR)\Phapp.sbr" \
	"$(INTDIR)\puzzle2.sbr" \
	"$(INTDIR)\puzzle3.sbr" \
	"$(INTDIR)\puzzle4.sbr" \
	"$(INTDIR)\puzzle5.sbr" \
	"$(INTDIR)\randseq.sbr" \
	"$(INTDIR)\Rcontrol.sbr" \
	"$(INTDIR)\Scene.sbr" \
	"$(INTDIR)\script.sbr" \
	"$(INTDIR)\Sound.sbr" \
	"$(INTDIR)\speedchk.sbr" \
	"$(INTDIR)\Sprite.sbr" \
	"$(INTDIR)\Transit.sbr" \
	"$(INTDIR)\utils.sbr" \
	"$(INTDIR)\Vcontrol.sbr" \
	"$(INTDIR)\video.sbr" \
	"$(INTDIR)\Videodrw.sbr" \
	"$(INTDIR)\Vreco.sbr" \
	"$(INTDIR)\vtable.sbr" \
	"$(INTDIR)\Wintools.sbr" \
	"$(INTDIR)\Worlds.sbr"

"$(OUTDIR)\Jungle32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winmm.lib /nologo /stack:0x10240 /subsystem:windows /debug /machine:IX86
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows /incremental:no /debug /machine:IX86 /out:".\jungle32.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /stack:0x10240 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/jungle32.pdb" /debug /machine:IX86\
 /out:".\jungle32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Bcontrol.obj" \
	"$(INTDIR)\bmulti.obj" \
	"$(INTDIR)\credits.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\gcontrol.obj" \
	"$(INTDIR)\hcontrol.obj" \
	"$(INTDIR)\hslider.obj" \
	"$(INTDIR)\httables.obj" \
	"$(INTDIR)\jungle.obj" \
	"$(INTDIR)\Jungle.res" \
	"$(INTDIR)\Mci.obj" \
	"$(INTDIR)\Mcontrol.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\offscren.obj" \
	"$(INTDIR)\pal.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\Phapp.obj" \
	"$(INTDIR)\puzzle2.obj" \
	"$(INTDIR)\puzzle3.obj" \
	"$(INTDIR)\puzzle4.obj" \
	"$(INTDIR)\puzzle5.obj" \
	"$(INTDIR)\randseq.obj" \
	"$(INTDIR)\Rcontrol.obj" \
	"$(INTDIR)\Scene.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\speedchk.obj" \
	"$(INTDIR)\Sprite.obj" \
	"$(INTDIR)\Transit.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\Vcontrol.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\Videodrw.obj" \
	"$(INTDIR)\Vreco.obj" \
	"$(INTDIR)\vtable.obj" \
	"$(INTDIR)\Wintools.obj" \
	"$(INTDIR)\Worlds.obj" \
	"..\Gameguts\vfw32.lib" \
	"..\Gameguts\wavmix32.lib" \
	"..\Gameguts\wing32.lib" \
	".\Icwapi32.lib"

".\jungle32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Jungle32 - Win32 Release"
# Name "Jungle32 - Win32 Debug"

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\vfw32.lib

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\wavmix32.lib

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\wing32.lib

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Vreco.cpp
DEP_CPP_VRECO=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\icwapi16.h"\
	".\icwapi32.h"\
	".\vmapi.h"\
	".\vreco.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Vreco.obj" : $(SOURCE) $(DEP_CPP_VRECO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\Vreco.obj" : $(SOURCE) $(DEP_CPP_VRECO) "$(INTDIR)"

"$(INTDIR)\Vreco.sbr" : $(SOURCE) $(DEP_CPP_VRECO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\puzzle2.cpp
DEP_CPP_PUZZL=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\jungle.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\puzzle2.obj" : $(SOURCE) $(DEP_CPP_PUZZL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\puzzle2.obj" : $(SOURCE) $(DEP_CPP_PUZZL) "$(INTDIR)"

"$(INTDIR)\puzzle2.sbr" : $(SOURCE) $(DEP_CPP_PUZZL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\puzzle3.cpp
DEP_CPP_PUZZLE=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\jungle.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\puzzle3.obj" : $(SOURCE) $(DEP_CPP_PUZZLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\puzzle3.obj" : $(SOURCE) $(DEP_CPP_PUZZLE) "$(INTDIR)"

"$(INTDIR)\puzzle3.sbr" : $(SOURCE) $(DEP_CPP_PUZZLE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\puzzle4.cpp
DEP_CPP_PUZZLE4=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\jungle.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\puzzle4.obj" : $(SOURCE) $(DEP_CPP_PUZZLE4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\puzzle4.obj" : $(SOURCE) $(DEP_CPP_PUZZLE4) "$(INTDIR)"

"$(INTDIR)\puzzle4.sbr" : $(SOURCE) $(DEP_CPP_PUZZLE4) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\puzzle5.cpp
DEP_CPP_PUZZLE5=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\jungle.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\puzzle5.obj" : $(SOURCE) $(DEP_CPP_PUZZLE5) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\puzzle5.obj" : $(SOURCE) $(DEP_CPP_PUZZLE5) "$(INTDIR)"

"$(INTDIR)\puzzle5.sbr" : $(SOURCE) $(DEP_CPP_PUZZLE5) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jungle.cpp
DEP_CPP_JUNGL=\
	"..\gameguts\control.h"\
	"..\gameguts\credits.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\randseq.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\jungle.h"\
	".\vreco.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\jungle.obj" : $(SOURCE) $(DEP_CPP_JUNGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


"$(INTDIR)\jungle.obj" : $(SOURCE) $(DEP_CPP_JUNGL) "$(INTDIR)"

"$(INTDIR)\jungle.sbr" : $(SOURCE) $(DEP_CPP_JUNGL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Bcontrol.cpp
DEP_CPP_BCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Bcontrol.obj" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Bcontrol.obj" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Bcontrol.sbr" : $(SOURCE) $(DEP_CPP_BCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\bmulti.cpp
DEP_CPP_BMULT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\bmulti.obj" : $(SOURCE) $(DEP_CPP_BMULT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\bmulti.obj" : $(SOURCE) $(DEP_CPP_BMULT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\bmulti.sbr" : $(SOURCE) $(DEP_CPP_BMULT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\credits.cpp
DEP_CPP_CREDI=\
	"..\gameguts\credits.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\parser.h"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\credits.obj" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\credits.obj" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\credits.sbr" : $(SOURCE) $(DEP_CPP_CREDI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\dialog.cpp
DEP_CPP_DIALO=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dialog.sbr" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Dib.cpp
DEP_CPP_DIB_C=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Dib.sbr" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\gcontrol.cpp
DEP_CPP_GCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\gcontrol.obj" : $(SOURCE) $(DEP_CPP_GCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\gcontrol.obj" : $(SOURCE) $(DEP_CPP_GCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\gcontrol.sbr" : $(SOURCE) $(DEP_CPP_GCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\hcontrol.cpp
DEP_CPP_HCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\hcontrol.obj" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\hcontrol.obj" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hcontrol.sbr" : $(SOURCE) $(DEP_CPP_HCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\hslider.cpp
DEP_CPP_HSLID=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\hslider.obj" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\hslider.obj" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\hslider.sbr" : $(SOURCE) $(DEP_CPP_HSLID) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\httables.cpp
DEP_CPP_HTTAB=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\httables.obj" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\httables.obj" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httables.sbr" : $(SOURCE) $(DEP_CPP_HTTAB) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Mci.cpp
DEP_CPP_MCI_C=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Mci.obj" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Mci.obj" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Mci.sbr" : $(SOURCE) $(DEP_CPP_MCI_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Mcontrol.cpp
DEP_CPP_MCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Mcontrol.obj" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Mcontrol.obj" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Mcontrol.sbr" : $(SOURCE) $(DEP_CPP_MCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\memory.cpp
DEP_CPP_MEMOR=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\offscren.cpp
DEP_CPP_OFFSC=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\offscren.obj" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\offscren.obj" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\offscren.sbr" : $(SOURCE) $(DEP_CPP_OFFSC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\pal.cpp
DEP_CPP_PAL_C=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\pal.obj" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pal.sbr" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Parser.cpp
DEP_CPP_PARSE=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\parser.h"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Parser.sbr" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Phapp.cpp
DEP_CPP_PHAPP=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Phapp.obj" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Phapp.obj" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Phapp.sbr" : $(SOURCE) $(DEP_CPP_PHAPP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\randseq.cpp
DEP_CPP_RANDS=\
	"..\gameguts\randseq.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\randseq.obj" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\randseq.obj" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\randseq.sbr" : $(SOURCE) $(DEP_CPP_RANDS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Rcontrol.cpp
DEP_CPP_RCONT=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\..\Gameguts\worlds.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Rcontrol.obj" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Rcontrol.obj" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Rcontrol.sbr" : $(SOURCE) $(DEP_CPP_RCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Scene.cpp
DEP_CPP_SCENE=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Scene.obj" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Scene.obj" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Scene.sbr" : $(SOURCE) $(DEP_CPP_SCENE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\script.cpp
DEP_CPP_SCRIP=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\script.h"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\script.sbr" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Sound.cpp
DEP_CPP_SOUND=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Sound.sbr" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\speedchk.cpp
DEP_CPP_SPEED=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\speedchk.obj" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\speedchk.obj" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\speedchk.sbr" : $(SOURCE) $(DEP_CPP_SPEED) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Sprite.cpp
DEP_CPP_SPRIT=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\script.h"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Sprite.obj" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Sprite.obj" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Sprite.sbr" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Transit.cpp
DEP_CPP_TRANS=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\randseq.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\transit.h"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Transit.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Transit.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Transit.sbr" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\utils.cpp
DEP_CPP_UTILS=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\utils.obj" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\utils.obj" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\utils.sbr" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Vcontrol.cpp
DEP_CPP_VCONT=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Vcontrol.obj" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Vcontrol.obj" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Vcontrol.sbr" : $(SOURCE) $(DEP_CPP_VCONT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\video.cpp
DEP_CPP_VIDEO=\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\video.obj" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\video.obj" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\video.sbr" : $(SOURCE) $(DEP_CPP_VIDEO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Videodrw.cpp
DEP_CPP_VIDEOD=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	"..\jungle\jungleid.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Videodrw.obj" : $(SOURCE) $(DEP_CPP_VIDEOD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Videodrw.obj" : $(SOURCE) $(DEP_CPP_VIDEOD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Videodrw.sbr" : $(SOURCE) $(DEP_CPP_VIDEOD) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\vtable.cpp
DEP_CPP_VTABL=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\parser.h"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\vtable.obj" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\vtable.obj" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vtable.sbr" : $(SOURCE) $(DEP_CPP_VTABL) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Wintools.cpp
DEP_CPP_WINTO=\
	"..\gameguts\commonid.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\wing.h"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Wintools.obj" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Wintools.obj" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Wintools.sbr" : $(SOURCE) $(DEP_CPP_WINTO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Dev\Archive\Jungle\Gameguts\Worlds.cpp
DEP_CPP_WORLD=\
	"..\gameguts\control.h"\
	"..\gameguts\phtypes.h"\
	"..\gameguts\proto.h"\
	"..\gameguts\scene.h"\
	"..\gameguts\sound.h"\
	".\..\Gameguts\BCONTROL.P"\
	".\..\Gameguts\BMULTI.P"\
	".\..\Gameguts\DIALOG.P"\
	".\..\Gameguts\DIB.H"\
	".\..\Gameguts\GCONTROL.P"\
	".\..\Gameguts\HCONTROL.P"\
	".\..\Gameguts\HSLIDER.P"\
	".\..\Gameguts\HTTABLES.P"\
	".\..\Gameguts\MACROS.H"\
	".\..\Gameguts\MCI.P"\
	".\..\Gameguts\MCONTROL.P"\
	".\..\Gameguts\MEMORY.P"\
	".\..\Gameguts\OFFSCREN.H"\
	".\..\Gameguts\PAL.P"\
	".\..\Gameguts\PHAPP.H"\
	".\..\Gameguts\PORT.H"\
	".\..\Gameguts\rcontrol.p"\
	".\..\Gameguts\SPEEDCHK.P"\
	".\..\Gameguts\SPRITE.H"\
	".\..\Gameguts\TOON.H"\
	".\..\Gameguts\TRANSIT.P"\
	".\..\Gameguts\UTILS.P"\
	".\..\Gameguts\VCONTROL.P"\
	".\..\Gameguts\VIDEO.P"\
	".\..\Gameguts\videodrw.p"\
	".\..\Gameguts\VTABLE.H"\
	".\..\Gameguts\VTABLE.P"\
	".\..\Gameguts\WAVEMIX.H"\
	".\..\Gameguts\WINTOOLS.P"\
	".\..\Gameguts\WINTOON.H"\
	".\..\Gameguts\worlds.h"\
	

!IF  "$(CFG)" == "Jungle32 - Win32 Release"


"$(INTDIR)\Worlds.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Worlds.obj" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Worlds.sbr" : $(SOURCE) $(DEP_CPP_WORLD) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Icwapi32.lib

!IF  "$(CFG)" == "Jungle32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Jungle32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Jungle.rc
DEP_RSC_JUNGLE=\
	"..\gameguts\commonid.h"\
	"..\gameguts\control.h"\
	"..\gameguts\resource.h"\
	"..\jungle\jungleid.h"\
	

"$(INTDIR)\Jungle.res" : $(SOURCE) $(DEP_RSC_JUNGLE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
