# Microsoft Visual C++ generated build script - Do not modify

PROJ = wtwin
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVC32s
ORIGIN_VER = 1.00
PROJPATH = C:\misc\gamedev\wtwin\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = 
FIRSTCPP = 
RC = rc
CFLAGS_D_WEXE32 = /nologo /D_X86_ /DWIN32 /W3 /FR /YX /D_DEBUG /Zi /D_WINDOWS 
CFLAGS_R_WEXE32 = /nologo /D_X86_ /DWIN32 /W3 /FR /YX /O2 /DNDEBUG /D_WINDOWS 
LFLAGS_D_WEXE32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_R_WEXE32 = /NOLOGO /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WEXE32 = 
LIBS_R_WEXE32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG /DWIN32
R_RCDEFINES32 = -dNDEBUG /DWIN32
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE32)
LFLAGS = $(LFLAGS_D_WEXE32)
LIBS = $(LIBS_D_WEXE32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WEXE32)
LFLAGS = $(LFLAGS_R_WEXE32)
LIBS = $(LIBS_R_WEXE32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif

all:	$(PROJ).EXE

######################

error.OBJ:	error.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c error.C

fixed.OBJ:	fixed.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c fixed.C

framebuf.OBJ:	framebuf.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c framebuf.C

gifload.OBJ:	gifload.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c gifload.C

list.OBJ:	list.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c list.C

render.OBJ:	render.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c render.C

table.OBJ:	table.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c table.C

texture.OBJ:	texture.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c texture.C

view.OBJ:	view.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c view.C

wingraph.OBJ:	wingraph.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c wingraph.C

wininput.OBJ:	wininput.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c wininput.C

world.OBJ:	world.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c world.C

worldfil.OBJ:	worldfil.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c worldfil.C

wt.OBJ:	wt.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c wt.C

wtmem.OBJ:	wtmem.C
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c wtmem.C

wingdll.OBJ:	wingdll.CPP
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c wingdll.CPP

winpal.OBJ:	winpal.CPP
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c winpal.CPP

wtwin.OBJ:	wtwin.CPP
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c wtwin.CPP

###########################

OBJS		=  error.OBJ  fixed.OBJ  framebuf.OBJ  \
		 gifload.OBJ  list.OBJ  render.OBJ  table.OBJ  \
		 texture.OBJ  view.OBJ  wingraph.OBJ  wininput.OBJ  \
		 world.OBJ  worldfil.OBJ  wt.OBJ  wtmem.OBJ  \
		 wingdll.OBJ  winpal.OBJ  wtwin.OBJ 


$(PROJ).res: $(PROJ).rc
        $(RC) -r $(PROJ).rc

$(PROJ).EXE:	$(OBJS) $(OBJS_EXT) $(LIBS_EXT) $(PROJ).RES
	echo >NUL @<<$(PROJ).CRF
error.OBJ
fixed.OBJ
framebuf.OBJ
gifload.OBJ
list.OBJ
render.OBJ
table.OBJ
texture.OBJ
view.OBJ
wingraph.OBJ
wininput.OBJ
world.OBJ
worldfil.OBJ
wt.OBJ
wtmem.OBJ
wingdll.OBJ
winpal.OBJ
wtwin.OBJ
$(OBJS_EXT)
-OUT:$(PROJ).EXE
$(MAPFILE_OPTION)
WING32.LIB
WINMM.LIB
COMDLG32.LIB
$(PROJ).RES
$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

