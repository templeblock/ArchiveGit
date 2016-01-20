# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=alias - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to alias - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "alias - Win32 Debug" && "$(CFG)" != "alias - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cpo4.mak" CFG="alias - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "alias - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "alias - Win32 Release" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "alias - Win32 Debug"
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "alias - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 4
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\cpo4.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\cpo4.pch"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\cpo4.exe"
	-@erase ".\WinDebug\TGARDR.OBJ"
	-@erase ".\WinDebug\quantize.obj"
	-@erase ".\WinDebug\ALIAS.OBJ"
	-@erase ".\WinDebug\DIB.OBJ"
	-@erase ".\WinDebug\TGAWRT.OBJ"
	-@erase ".\WinDebug\PAL.OBJ"
	-@erase ".\WinDebug\palio.obj"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\aliasdlg.obj"
	-@erase ".\WinDebug\alias.res"
	-@erase ".\WinDebug\cpo4.ilk"
	-@erase ".\WinDebug\cpo4.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "CPO" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "CPO" /Fp"$(INTDIR)/cpo4.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/alias.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cpo4.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib wing32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=winmm.lib wing32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/cpo4.pdb" /debug /machine:I386 /out:"$(OUTDIR)/cpo4.exe" 
LINK32_OBJS= \
	"$(INTDIR)/TGARDR.OBJ" \
	"$(INTDIR)/quantize.obj" \
	"$(INTDIR)/ALIAS.OBJ" \
	"$(INTDIR)/DIB.OBJ" \
	"$(INTDIR)/TGAWRT.OBJ" \
	"$(INTDIR)/PAL.OBJ" \
	"$(INTDIR)/palio.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/aliasdlg.obj" \
	"$(INTDIR)/alias.res" \
	"C:\Wing\Lib\Wing32.lib"

"$(OUTDIR)\cpo4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "alias - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 4
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\cpo4.exe"

CLEAN : 
	-@erase ".\WinRel\cpo4.exe"
	-@erase ".\WinRel\ALIAS.OBJ"
	-@erase ".\WinRel\cpo4.pch"
	-@erase ".\WinRel\palio.obj"
	-@erase ".\WinRel\aliasdlg.obj"
	-@erase ".\WinRel\PAL.OBJ"
	-@erase ".\WinRel\quantize.obj"
	-@erase ".\WinRel\TGAWRT.OBJ"
	-@erase ".\WinRel\TGARDR.OBJ"
	-@erase ".\WinRel\DIB.OBJ"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\alias.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "CPO" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /D "CPO" /Fp"$(INTDIR)/cpo4.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/alias.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cpo4.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib wing32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=winmm.lib wing32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/cpo4.pdb" /machine:I386 /out:"$(OUTDIR)/cpo4.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ALIAS.OBJ" \
	"$(INTDIR)/palio.obj" \
	"$(INTDIR)/aliasdlg.obj" \
	"$(INTDIR)/PAL.OBJ" \
	"$(INTDIR)/quantize.obj" \
	"$(INTDIR)/TGAWRT.OBJ" \
	"$(INTDIR)/TGARDR.OBJ" \
	"$(INTDIR)/DIB.OBJ" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/alias.res" \
	"C:\Wing\Lib\Wing32.lib"

"$(OUTDIR)\cpo4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "alias - Win32 Debug"
# Name "alias - Win32 Release"

!IF  "$(CFG)" == "alias - Win32 Debug"

!ELSEIF  "$(CFG)" == "alias - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "alias - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /D "CPO" /Fp"$(INTDIR)/cpo4.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cpo4.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "alias - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /D "CPO" /Fp"$(INTDIR)/cpo4.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cpo4.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ALIAS.CPP
DEP_CPP_ALIAS=\
	".\stdafx.h"\
	".\alias.h"\
	".\aliasdlg.h"\
	".\tgardr.h"\
	".\tgawrt.h"\
	".\palio.h"\
	".\pal.h"\
	".\quantize.h"\
	".\DIB.H"\
	".\PHTYPES.H"\
	

"$(INTDIR)\ALIAS.OBJ" : $(SOURCE) $(DEP_CPP_ALIAS) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\aliasdlg.cpp
DEP_CPP_ALIASD=\
	".\stdafx.h"\
	".\alias.h"\
	".\aliasdlg.h"\
	".\quantize.h"\
	".\DIB.H"\
	".\PHTYPES.H"\
	

"$(INTDIR)\aliasdlg.obj" : $(SOURCE) $(DEP_CPP_ALIASD) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\alias.rc
DEP_RSC_ALIAS_=\
	".\res\alias.ico"\
	".\res\alias.rc2"\
	

"$(INTDIR)\alias.res" : $(SOURCE) $(DEP_RSC_ALIAS_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "alias - Win32 Debug"

!ELSEIF  "$(CFG)" == "alias - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIB.CPP
DEP_CPP_DIB_C=\
	".\stdafx.h"\
	".\DIB.H"\
	{$(INCLUDE)}"\Wing.h"\
	".\PHTYPES.H"\
	

"$(INTDIR)\DIB.OBJ" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TGARDR.CPP
DEP_CPP_TGARD=\
	".\stdafx.h"\
	".\PHTYPES.H"\
	".\TARGA.H"\
	".\DIB.H"\
	

"$(INTDIR)\TGARDR.OBJ" : $(SOURCE) $(DEP_CPP_TGARD) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TGAWRT.CPP
DEP_CPP_TGAWR=\
	".\stdafx.h"\
	".\PHTYPES.H"\
	".\TARGA.H"\
	".\DIB.H"\
	

"$(INTDIR)\TGAWRT.OBJ" : $(SOURCE) $(DEP_CPP_TGAWR) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\quantize.cpp
DEP_CPP_QUANT=\
	".\stdafx.h"\
	".\quantize.h"\
	

"$(INTDIR)\quantize.obj" : $(SOURCE) $(DEP_CPP_QUANT) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\palio.cpp
DEP_CPP_PALIO=\
	".\stdafx.h"\
	".\palio.h"\
	

"$(INTDIR)\palio.obj" : $(SOURCE) $(DEP_CPP_PALIO) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PAL.CPP
DEP_CPP_PAL_C=\
	".\stdafx.h"\
	".\pal.h"\
	

"$(INTDIR)\PAL.OBJ" : $(SOURCE) $(DEP_CPP_PAL_C) "$(INTDIR)"\
 "$(INTDIR)\cpo4.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=C:\Wing\Lib\Wing32.lib

!IF  "$(CFG)" == "alias - Win32 Debug"

!ELSEIF  "$(CFG)" == "alias - Win32 Release"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
