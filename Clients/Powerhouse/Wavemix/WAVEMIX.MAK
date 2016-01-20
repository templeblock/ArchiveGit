#DEBUG = 1

RCFLAGS =/NOLOGO -r
RESFLAGS =/NOLOGO -t

MAPFILE = nul
DLL_OBJS = \
	   wavemix.OBJ\
	   LIBENTRY.OBJ\
	   mixit.obj

DLL_LIBS = sdllcew libw mmsystem

######################## inference rules ####################
.asm.obj:
!ifdef DEBUG
    ml /c /Cp /W3 /Zi /DDEBUG $*.asm
!else
    ml /c /Cp /W3 $*.asm
!endif

all:    wavmix16.dll wavmix16.lib

wavemix.obj: wavemix.h wavemix.def wavemix.c wavemix.mak
!ifdef DEBUG
       cl -c -W3 -ASw -Gswc -Od -Zpei -YX -DDEBUG wavemix.c
!else
#   note: should really use -GD for dlls and _export and _loadds to avoid load ds
#         on functions that don't need it  
    cl -c -W3 -ASw -Gswc -O2 -YX wavemix.c
!endif

libentry.obj: libentry.asm
    masm -Mx -W2 libentry.asm;

mixit.obj: mixit.asm
    ml /c /Cp /W3 /Zi /Fl /Sc /DDEBUG mixit.asm

wavmix16.lib: wavemix.def wavmix16.dll
	implib /NOWEP wavmix16.lib wavmix16.dll

wavemix.res: wavemix.rc
	rc $(RCFLAGS) wavemix.rc

wavmix16.dll: $(DLL_OBJS) wavemix.res
!ifdef DEBUG
	Link /MAP /NOD /NOE /CO $(DLL_OBJS), wavmix16.dll, wavemix.map, $(DLL_LIBS), wavemix.def
!else
	Link /MAP /NOD /NOE $(DLL_OBJS), wavmix16.dll, wavemix.map, $(DLL_LIBS), wavemix.def
!endif
	mapsym wavemix.map
	rc $(RESFLAGS) wavemix.res wavmix16.dll


#######  use these commands to profile the dll #######
#
# steps
#  1. nmake prep
#  2. profilew $(PROJ).exe [parameters]
#  3. nmake report
#

Prep:
 prep /ft /p wavemix.dll /ot mixtest.pbt /oi mixtest.pbi

Report:
 prep /it mixtest.pbt /io mixtest.pbo /ot mixtest.pbt
 plist /st mixtest.pbt > mixtest.out

 del mixtest.pbo
 del mixtest.pbi
