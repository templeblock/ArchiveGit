# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=msregx32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msregx32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msregx32 - Win32 Debug" && "$(CFG)" !=\
 "msregx32 - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSREGX32.MAK" CFG="msregx32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msregx32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msregx32 - Win32 Release" (based on\
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
# PROP Target_Last_Scanned "msregx32 - Win32 Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

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

ALL : "$(OUTDIR)\MSREGX32.dll"

CLEAN : 
	-@erase "$(INTDIR)\badcode.obj"
	-@erase "$(INTDIR)\BASEDLG.OBJ"
	-@erase "$(INTDIR)\BMPCTL.OBJ"
	-@erase "$(INTDIR)\bymail.obj"
	-@erase "$(INTDIR)\byphone.obj"
	-@erase "$(INTDIR)\callfail.obj"
	-@erase "$(INTDIR)\cantreg.obj"
	-@erase "$(INTDIR)\COMLINK.OBJ"
	-@erase "$(INTDIR)\congrats.obj"
	-@erase "$(INTDIR)\CPROGBAR.OBJ"
	-@erase "$(INTDIR)\Cwmf.obj"
	-@erase "$(INTDIR)\CWMFCTL.OBJ"
	-@erase "$(INTDIR)\Ddb.obj"
	-@erase "$(INTDIR)\dialopts.obj"
	-@erase "$(INTDIR)\INIFILE.OBJ"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\INTLDLL.OBJ"
	-@erase "$(INTDIR)\key.obj"
	-@erase "$(INTDIR)\later.obj"
	-@erase "$(INTDIR)\Mlsprint.obj"
	-@erase "$(INTDIR)\MSENGINE.OBJ"
	-@erase "$(INTDIR)\msmodem.obj"
	-@erase "$(INTDIR)\mspacket.obj"
	-@erase "$(INTDIR)\msregx.obj"
	-@erase "$(INTDIR)\msregx.res"
	-@erase "$(INTDIR)\MSREGX32.pch"
	-@erase "$(INTDIR)\msserial.obj"
	-@erase "$(INTDIR)\MSZMODEM.OBJ"
	-@erase "$(INTDIR)\nomodem.obj"
	-@erase "$(INTDIR)\phonenum.obj"
	-@erase "$(INTDIR)\propctl.obj"
	-@erase "$(INTDIR)\propstr.obj"
	-@erase "$(INTDIR)\regbonus.obj"
	-@erase "$(INTDIR)\regform.obj"
	-@erase "$(INTDIR)\reginfo.obj"
	-@erase "$(INTDIR)\register.obj"
	-@erase "$(INTDIR)\required.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\send.obj"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\sysinfo.obj"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\whereru.obj"
	-@erase "$(OUTDIR)\MSREGX32.dll"
	-@erase "$(OUTDIR)\MSREGX32.exp"
	-@erase "$(OUTDIR)\MSREGX32.ilk"
	-@erase "$(OUTDIR)\MSREGX32.lib"
	-@erase "$(OUTDIR)\MSREGX32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "\msdev\mfc\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "\msdev\mfc\src" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS"\
 /Fp"$(INTDIR)/MSREGX32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x0 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msregx.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MSREGX32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /def:"MSREGDLL.def" /implib:"MSREGDLL.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 .\gcpp300\msvc2x\cp30fw.lib winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=.\gcpp300\msvc2x\cp30fw.lib winmm.lib /nologo /subsystem:windows\
 /dll /incremental:yes /pdb:"$(OUTDIR)/MSREGX32.pdb" /debug /machine:I386\
 /def:".\msregx32.def" /out:"$(OUTDIR)/MSREGX32.dll"\
 /implib:"$(OUTDIR)/MSREGX32.lib" 
DEF_FILE= \
	".\msregx32.def"
LINK32_OBJS= \
	"$(INTDIR)\badcode.obj" \
	"$(INTDIR)\BASEDLG.OBJ" \
	"$(INTDIR)\BMPCTL.OBJ" \
	"$(INTDIR)\bymail.obj" \
	"$(INTDIR)\byphone.obj" \
	"$(INTDIR)\callfail.obj" \
	"$(INTDIR)\cantreg.obj" \
	"$(INTDIR)\COMLINK.OBJ" \
	"$(INTDIR)\congrats.obj" \
	"$(INTDIR)\CPROGBAR.OBJ" \
	"$(INTDIR)\Cwmf.obj" \
	"$(INTDIR)\CWMFCTL.OBJ" \
	"$(INTDIR)\Ddb.obj" \
	"$(INTDIR)\dialopts.obj" \
	"$(INTDIR)\INIFILE.OBJ" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\INTLDLL.OBJ" \
	"$(INTDIR)\key.obj" \
	"$(INTDIR)\later.obj" \
	"$(INTDIR)\Mlsprint.obj" \
	"$(INTDIR)\MSENGINE.OBJ" \
	"$(INTDIR)\msmodem.obj" \
	"$(INTDIR)\mspacket.obj" \
	"$(INTDIR)\msregx.obj" \
	"$(INTDIR)\msregx.res" \
	"$(INTDIR)\msserial.obj" \
	"$(INTDIR)\MSZMODEM.OBJ" \
	"$(INTDIR)\nomodem.obj" \
	"$(INTDIR)\phonenum.obj" \
	"$(INTDIR)\propctl.obj" \
	"$(INTDIR)\propstr.obj" \
	"$(INTDIR)\regbonus.obj" \
	"$(INTDIR)\regform.obj" \
	"$(INTDIR)\reginfo.obj" \
	"$(INTDIR)\register.obj" \
	"$(INTDIR)\required.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\send.obj" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\sysinfo.obj" \
	"$(INTDIR)\UTIL.OBJ" \
	"$(INTDIR)\whereru.obj"

"$(OUTDIR)\MSREGX32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

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

ALL : "$(OUTDIR)\MSREGX32.dll"

CLEAN : 
	-@erase "$(INTDIR)\badcode.obj"
	-@erase "$(INTDIR)\BASEDLG.OBJ"
	-@erase "$(INTDIR)\BMPCTL.OBJ"
	-@erase "$(INTDIR)\bymail.obj"
	-@erase "$(INTDIR)\byphone.obj"
	-@erase "$(INTDIR)\callfail.obj"
	-@erase "$(INTDIR)\cantreg.obj"
	-@erase "$(INTDIR)\COMLINK.OBJ"
	-@erase "$(INTDIR)\congrats.obj"
	-@erase "$(INTDIR)\CPROGBAR.OBJ"
	-@erase "$(INTDIR)\Cwmf.obj"
	-@erase "$(INTDIR)\CWMFCTL.OBJ"
	-@erase "$(INTDIR)\Ddb.obj"
	-@erase "$(INTDIR)\dialopts.obj"
	-@erase "$(INTDIR)\INIFILE.OBJ"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\INTLDLL.OBJ"
	-@erase "$(INTDIR)\key.obj"
	-@erase "$(INTDIR)\later.obj"
	-@erase "$(INTDIR)\Mlsprint.obj"
	-@erase "$(INTDIR)\MSENGINE.OBJ"
	-@erase "$(INTDIR)\msmodem.obj"
	-@erase "$(INTDIR)\mspacket.obj"
	-@erase "$(INTDIR)\msregx.obj"
	-@erase "$(INTDIR)\msregx.res"
	-@erase "$(INTDIR)\MSREGX32.pch"
	-@erase "$(INTDIR)\msserial.obj"
	-@erase "$(INTDIR)\MSZMODEM.OBJ"
	-@erase "$(INTDIR)\nomodem.obj"
	-@erase "$(INTDIR)\phonenum.obj"
	-@erase "$(INTDIR)\propctl.obj"
	-@erase "$(INTDIR)\propstr.obj"
	-@erase "$(INTDIR)\regbonus.obj"
	-@erase "$(INTDIR)\regform.obj"
	-@erase "$(INTDIR)\reginfo.obj"
	-@erase "$(INTDIR)\register.obj"
	-@erase "$(INTDIR)\required.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\send.obj"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\sysinfo.obj"
	-@erase "$(INTDIR)\UTIL.OBJ"
	-@erase "$(INTDIR)\whereru.obj"
	-@erase "$(OUTDIR)\MSREGX32.dll"
	-@erase "$(OUTDIR)\MSREGX32.exp"
	-@erase "$(OUTDIR)\MSREGX32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "\msdev\mfc\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /O1 /I "\msdev\mfc\src" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSREGX32.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x0 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/msregx.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/MSREGX32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /def:"MSREGDLL.def" /implib:"MSREGDLL.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 .\gcpp300\msvc2x\cp30fw.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=.\gcpp300\msvc2x\cp30fw.lib winmm.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/MSREGX32.pdb" /machine:I386\
 /def:".\msregx32.def" /out:"$(OUTDIR)/MSREGX32.dll"\
 /implib:"$(OUTDIR)/MSREGX32.lib" 
DEF_FILE= \
	".\msregx32.def"
LINK32_OBJS= \
	"$(INTDIR)\badcode.obj" \
	"$(INTDIR)\BASEDLG.OBJ" \
	"$(INTDIR)\BMPCTL.OBJ" \
	"$(INTDIR)\bymail.obj" \
	"$(INTDIR)\byphone.obj" \
	"$(INTDIR)\callfail.obj" \
	"$(INTDIR)\cantreg.obj" \
	"$(INTDIR)\COMLINK.OBJ" \
	"$(INTDIR)\congrats.obj" \
	"$(INTDIR)\CPROGBAR.OBJ" \
	"$(INTDIR)\Cwmf.obj" \
	"$(INTDIR)\CWMFCTL.OBJ" \
	"$(INTDIR)\Ddb.obj" \
	"$(INTDIR)\dialopts.obj" \
	"$(INTDIR)\INIFILE.OBJ" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\INTLDLL.OBJ" \
	"$(INTDIR)\key.obj" \
	"$(INTDIR)\later.obj" \
	"$(INTDIR)\Mlsprint.obj" \
	"$(INTDIR)\MSENGINE.OBJ" \
	"$(INTDIR)\msmodem.obj" \
	"$(INTDIR)\mspacket.obj" \
	"$(INTDIR)\msregx.obj" \
	"$(INTDIR)\msregx.res" \
	"$(INTDIR)\msserial.obj" \
	"$(INTDIR)\MSZMODEM.OBJ" \
	"$(INTDIR)\nomodem.obj" \
	"$(INTDIR)\phonenum.obj" \
	"$(INTDIR)\propctl.obj" \
	"$(INTDIR)\propstr.obj" \
	"$(INTDIR)\regbonus.obj" \
	"$(INTDIR)\regform.obj" \
	"$(INTDIR)\reginfo.obj" \
	"$(INTDIR)\register.obj" \
	"$(INTDIR)\required.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\send.obj" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\sysinfo.obj" \
	"$(INTDIR)\UTIL.OBJ" \
	"$(INTDIR)\whereru.obj"

"$(OUTDIR)\MSREGX32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "msregx32 - Win32 Debug"
# Name "msregx32 - Win32 Release"

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\STDAFX.H"\
	

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /I "\msdev\mfc\src" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS"\
 /Fp"$(INTDIR)/MSREGX32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MSREGX32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O1 /I "\msdev\mfc\src" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Fp"$(INTDIR)/MSREGX32.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MSREGX32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\README.TXT

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BMPCTL.CPP
DEP_CPP_BMPCT=\
	".\BMPCTL.H"\
	".\ddb.h"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\BMPCTL.OBJ" : $(SOURCE) $(DEP_CPP_BMPCT) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\register.cpp
DEP_CPP_REGIS=\
	".\BASEDLG.H"\
	".\BYMAIL.H"\
	".\BYPHONE.H"\
	".\CALLFAIL.H"\
	".\CANTREG.H"\
	".\CONGRATS.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\LATER.H"\
	".\MSPACKET.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\NOMODEM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REGINFO.H"\
	".\REGISTER.H"\
	".\SEARCH.H"\
	".\SEND.H"\
	".\STDAFX.H"\
	".\SYSINFO.H"\
	".\Util.h"\
	

"$(INTDIR)\register.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\propstr.cpp
DEP_CPP_PROPS=\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\propstr.obj" : $(SOURCE) $(DEP_CPP_PROPS) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\propctl.cpp
DEP_CPP_PROPC=\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\propctl.obj" : $(SOURCE) $(DEP_CPP_PROPC) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bymail.cpp
DEP_CPP_BYMAI=\
	".\BASEDLG.H"\
	".\BYMAIL.H"\
	".\CWMF.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MLSPRINT.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PHONENUM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REGFORM.H"\
	".\REGINFO.H"\
	".\REQUIRED.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\bymail.obj" : $(SOURCE) $(DEP_CPP_BYMAI) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\later.cpp
DEP_CPP_LATER=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\LATER.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\later.obj" : $(SOURCE) $(DEP_CPP_LATER) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\congrats.cpp
DEP_CPP_CONGR=\
	".\BASEDLG.H"\
	".\CONGRATS.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\congrats.obj" : $(SOURCE) $(DEP_CPP_CONGR) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\reginfo.cpp
DEP_CPP_REGIN=\
	".\BASEDLG.H"\
	".\DIALOPTS.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PHONENUM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REGINFO.H"\
	".\REQUIRED.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\reginfo.obj" : $(SOURCE) $(DEP_CPP_REGIN) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dialopts.cpp
DEP_CPP_DIALO=\
	".\BASEDLG.H"\
	".\DIALOPTS.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\dialopts.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\INIFILE.CPP
DEP_CPP_INIFI=\
	".\INIFILE.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\INIFILE.OBJ" : $(SOURCE) $(DEP_CPP_INIFI) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CWMFCTL.CPP
DEP_CPP_CWMFC=\
	".\CWMF.H"\
	".\CWMFCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\CWMFCTL.OBJ" : $(SOURCE) $(DEP_CPP_CWMFC) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\callfail.cpp
DEP_CPP_CALLF=\
	".\BASEDLG.H"\
	".\CALLFAIL.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\callfail.obj" : $(SOURCE) $(DEP_CPP_CALLF) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\search.cpp
DEP_CPP_SEARC=\
	".\BASEDLG.H"\
	".\COMLINK.H"\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\nasi.hpp"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSMODEM.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\MSSERIAL.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\SEARCH.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\nomodem.cpp
DEP_CPP_NOMOD=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\NOMODEM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\nomodem.obj" : $(SOURCE) $(DEP_CPP_NOMOD) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\phonenum.cpp
DEP_CPP_PHONE=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PHONENUM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\phonenum.obj" : $(SOURCE) $(DEP_CPP_PHONE) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\required.cpp
DEP_CPP_REQUI=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REQUIRED.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\required.obj" : $(SOURCE) $(DEP_CPP_REQUI) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\send.cpp
DEP_CPP_SEND_=\
	".\BASEDLG.H"\
	".\COMLINK.H"\
	".\CPROGBAR.H"\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_engine.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\filexfer.hpp"\
	".\gcpp300\h\nasi.hpp"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSENGINE.H"\
	".\MSMODEM.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\MSSERIAL.H"\
	".\MSZMODEM.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\SEND.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\send.obj" : $(SOURCE) $(DEP_CPP_SEND_) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\UTIL.CPP
DEP_CPP_UTIL_=\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\UTIL.OBJ" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mspacket.cpp
DEP_CPP_MSPAC=\
	".\MSPACKET.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\mspacket.obj" : $(SOURCE) $(DEP_CPP_MSPAC) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\COMLINK.CPP
DEP_CPP_COMLI=\
	".\COMLINK.H"\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\nasi.hpp"\
	".\STDAFX.H"\
	

"$(INTDIR)\COMLINK.OBJ" : $(SOURCE) $(DEP_CPP_COMLI) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msmodem.cpp
DEP_CPP_MSMOD=\
	".\COMLINK.H"\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\nasi.hpp"\
	".\MSMODEM.H"\
	".\MSSERIAL.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\msmodem.obj" : $(SOURCE) $(DEP_CPP_MSMOD) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msserial.cpp
DEP_CPP_MSSER=\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\nasi.hpp"\
	".\MSSERIAL.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\msserial.obj" : $(SOURCE) $(DEP_CPP_MSSER) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CPROGBAR.CPP
DEP_CPP_CPROG=\
	".\CPROGBAR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\CPROGBAR.OBJ" : $(SOURCE) $(DEP_CPP_CPROG) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSENGINE.CPP
DEP_CPP_MSENG=\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_engine.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\filexfer.hpp"\
	".\gcpp300\h\nasi.hpp"\
	".\MSENGINE.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\MSENGINE.OBJ" : $(SOURCE) $(DEP_CPP_MSENG) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSZMODEM.CPP
DEP_CPP_MSZMO=\
	".\gcpp300\h\_8250.hpp"\
	".\gcpp300\h\_comlink.hpp"\
	".\gcpp300\h\_defs386.h"\
	".\gcpp300\h\_dosisr.hpp"\
	".\gcpp300\h\_engine.hpp"\
	".\gcpp300\h\_star.hpp"\
	".\gcpp300\h\asciidef.h"\
	".\gcpp300\h\comdef.hpp"\
	".\gcpp300\h\compat20.h"\
	".\gcpp300\h\compplib.hpp"\
	".\gcpp300\h\cppcomp.h"\
	".\gcpp300\h\filexfer.hpp"\
	".\gcpp300\h\nasi.hpp"\
	".\MSZMODEM.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\MSZMODEM.OBJ" : $(SOURCE) $(DEP_CPP_MSZMO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\byphone.cpp
DEP_CPP_BYPHO=\
	".\BASEDLG.H"\
	".\BYPHONE.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\byphone.obj" : $(SOURCE) $(DEP_CPP_BYPHO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msregx.cpp
DEP_CPP_MSREG=\
	".\ALREADY.H"\
	".\BADCODE.H"\
	".\BASEDLG.H"\
	".\BMPCTL.H"\
	".\CONGRATS.H"\
	".\CPROGBAR.H"\
	".\CWMF.H"\
	".\CWMFCTL.H"\
	".\INIFILE.H"\
	".\INSTALL.H"\
	".\intl\INTLDLL.H"\
	".\KEY.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\REGBONUS.H"\
	".\REGINFO.H"\
	".\REGISTER.H"\
	".\STDAFX.H"\
	".\Util.h"\
	".\WHERERU.H"\
	

"$(INTDIR)\msregx.obj" : $(SOURCE) $(DEP_CPP_MSREG) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msregx.rc
DEP_RSC_MSREGX=\
	".\RES\BLANK8.WAV"\
	".\res\delivery.bmp"\
	".\res\dogwrite.bmp"\
	".\res\huh.bmp"\
	".\res\invite.bmp"\
	".\RES\MAILBOT.WMF"\
	".\RES\MAILMID.WMF"\
	".\RES\MAILTOP.WMF"\
	".\res\MSREGDLL.RC2"\
	".\res\phone.bmp"\
	".\res\sherlock.bmp"\
	".\res\strike.bmp"\
	".\res\sysinfo.bmp"\
	".\res\thanks.bmp"\
	".\res\wavebye.bmp"\
	".\res\womanclp.bmp"\
	

"$(INTDIR)\msregx.res" : $(SOURCE) $(DEP_RSC_MSREGX) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\msregx32.def

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regbonus.cpp
DEP_CPP_REGBO=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REGBONUS.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\regbonus.obj" : $(SOURCE) $(DEP_CPP_REGBO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\key.cpp
DEP_CPP_KEY_C=\
	".\KEY.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\key.obj" : $(SOURCE) $(DEP_CPP_KEY_C) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\badcode.cpp
DEP_CPP_BADCO=\
	".\BADCODE.H"\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\badcode.obj" : $(SOURCE) $(DEP_CPP_BADCO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\install.cpp
DEP_CPP_INSTA=\
	".\INSTALL.H"\
	".\STDAFX.H"\
	".\Util.h"\
	

"$(INTDIR)\install.obj" : $(SOURCE) $(DEP_CPP_INSTA) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BASEDLG.CPP

!IF  "$(CFG)" == "msregx32 - Win32 Debug"

DEP_CPP_BASED=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\STDAFX.H"\
	"\msdev\mfc\src\afximpl.h"\
	

"$(INTDIR)\BASEDLG.OBJ" : $(SOURCE) $(DEP_CPP_BASED) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


!ELSEIF  "$(CFG)" == "msregx32 - Win32 Release"

DEP_CPP_BASED=\
	".\BASEDLG.H"\
	".\MSREGX.H"\
	".\STDAFX.H"\
	"\msdev\mfc\src\afximpl.h"\
	

"$(INTDIR)\BASEDLG.OBJ" : $(SOURCE) $(DEP_CPP_BASED) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cantreg.cpp
DEP_CPP_CANTR=\
	".\BASEDLG.H"\
	".\CANTREG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\cantreg.obj" : $(SOURCE) $(DEP_CPP_CANTR) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ddb.cpp
DEP_CPP_DDB_C=\
	".\ddb.h"\
	".\STDAFX.H"\
	

"$(INTDIR)\Ddb.obj" : $(SOURCE) $(DEP_CPP_DDB_C) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mlsprint.cpp
DEP_CPP_MLSPR=\
	".\BASEDLG.H"\
	".\MLSPRINT.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\Mlsprint.obj" : $(SOURCE) $(DEP_CPP_MLSPR) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cwmf.cpp
DEP_CPP_CWMF_=\
	".\CWMF.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\Cwmf.obj" : $(SOURCE) $(DEP_CPP_CWMF_) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\whereru.cpp
DEP_CPP_WHERE=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGINI.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\REGINFO.H"\
	".\STDAFX.H"\
	".\Util.h"\
	".\WHERERU.H"\
	

"$(INTDIR)\whereru.obj" : $(SOURCE) $(DEP_CPP_WHERE) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sysinfo.cpp
DEP_CPP_SYSIN=\
	".\BASEDLG.H"\
	".\INIFILE.H"\
	".\intl\INTLDLL.H"\
	".\MSREG.H"\
	".\MSREGX.H"\
	".\PROPCTL.H"\
	".\PROPSTR.H"\
	".\STDAFX.H"\
	".\SYSINFO.H"\
	

"$(INTDIR)\sysinfo.obj" : $(SOURCE) $(DEP_CPP_SYSIN) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\regform.CPP
DEP_CPP_REGFO=\
	".\BASEDLG.H"\
	".\CWMF.H"\
	".\MLSPRINT.H"\
	".\REGFORM.H"\
	".\STDAFX.H"\
	

"$(INTDIR)\regform.obj" : $(SOURCE) $(DEP_CPP_REGFO) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\intl\INTLDLL.CPP
DEP_CPP_INTLD=\
	".\intl\INTLDLL.H"\
	".\intl\stdafx.h"\
	

"$(INTDIR)\INTLDLL.OBJ" : $(SOURCE) $(DEP_CPP_INTLD) "$(INTDIR)"\
 "$(INTDIR)\MSREGX32.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
