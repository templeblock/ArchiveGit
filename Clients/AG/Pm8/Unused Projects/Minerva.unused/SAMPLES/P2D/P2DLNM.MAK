# Microsoft Developer Studio Generated NMAKE File, Based on p2dlnm.dsp
!IF "$(CFG)" == ""
CFG=p2dlnm - Win32 Release
!MESSAGE No configuration specified. Defaulting to p2dlnm - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "p2dlnm - Win32 Release" && "$(CFG)" != "p2dlnm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "p2dlnm.mak" CFG="p2dlnm - Win32 Release"
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

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

OUTDIR=.\..\..\bin
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\p2dlnm.exe"

!ELSE 

ALL : "$(OUTDIR)\p2dlnm.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\p2dlnm.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Xetest.obj"
	-@erase "$(OUTDIR)\p2dlnm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O1 /I "..\..\include" /D "NDEBUG" /D PC_WATCOM=0\
 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__" /Fp"$(INTDIR)\p2dlnm.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\p2dlnm.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\p2dlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\p2dlnm.pdb" /machine:I386 /out:"$(OUTDIR)\p2dlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\p2dlnm.res" \
	"$(INTDIR)\Xetest.obj" \
	"..\..\Lib\pn1320lm.lib" \
	"..\..\Lib\pn20lm.lib"

"$(OUTDIR)\p2dlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

OUTDIR=.\..\..\bin
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\p2dlnm.exe"

!ELSE 

ALL : "$(OUTDIR)\p2dlnm.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\p2dlnm.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Xetest.obj"
	-@erase "$(OUTDIR)\p2dlnm.exe"
	-@erase "$(OUTDIR)\p2dlnm.ilk"
	-@erase "$(OUTDIR)\p2dlnm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 PC_WATCOM=0 /D "WIN32" /D "_CONSOLE" /D "WINDOWS" /D "__FLAT__"\
 /Fp"$(INTDIR)\p2dlnm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\p2dlnm.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\p2dlnm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\p2dlnm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\p2dlnm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\p2dlnm.res" \
	"$(INTDIR)\Xetest.obj" \
	"..\..\Lib\pn1320lm.lib" \
	"..\..\Lib\pn20lm.lib"

"$(OUTDIR)\p2dlnm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "p2dlnm - Win32 Release" || "$(CFG)" == "p2dlnm - Win32 Debug"
SOURCE=.\p2dlnm.rc

"$(INTDIR)\p2dlnm.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Xetest.c

!IF  "$(CFG)" == "p2dlnm - Win32 Release"

DEP_CPP_XETES=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	{$(INCLUDE)}"bmp.h"\
	{$(INCLUDE)}"stdtypes.h"\
	

"$(INTDIR)\Xetest.obj" : $(SOURCE) $(DEP_CPP_XETES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "p2dlnm - Win32 Debug"

DEP_CPP_XETES=\
	"..\..\include\errors.h"\
	"..\..\include\pic.h"\
	{$(INCLUDE)}"bmp.h"\
	{$(INCLUDE)}"internal.h"\
	{$(INCLUDE)}"picclib.h"\
	{$(INCLUDE)}"stdtypes.h"\
	

"$(INTDIR)\Xetest.obj" : $(SOURCE) $(DEP_CPP_XETES) "$(INTDIR)"


!ENDIF 


!ENDIF 

