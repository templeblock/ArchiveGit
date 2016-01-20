PROJ = mixtest
#DEBUG = 1
CC = cl
RC = rc
CFLAGS_D_WEXE =/nologo /YX /c /G2 /Zp1 /W3 /AS /Od /DDEBUG /GA /Zi
CFLAGS_R_WEXE =/nologo /YX /c /G2 /Zp1 /W3 /AS /Ox /GA /Gs
LFLAGS_D_WEXE =/NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE /CO 
LFLAGS_R_WEXE =/NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE 
LIBS_D_WEXE = libw slibcew
LIBS_R_WEXE = libw slibcew
RCFLAGS =/NOLOGO -r
RESFLAGS =/NOLOGO  -t
DEFFILE = $(PROJ).DEF
LIBS_EXT = wavmix16.lib

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
!endif

all: $(PROJ).exe

$(PROJ).OBJ:    $(PROJ).C $(PROJ).h resource.h wavemix.h
	$(CC) $(CFLAGS) $(PROJ).C

$(PROJ).RES:    $(PROJ).RC resource.h
	$(RC) $(RCFLAGS) $(PROJ).RC

$(PROJ).EXE:: $(PROJ).RES

$(PROJ).EXE::   $(PROJ).OBJ $(DEFFILE) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
$(PROJ).OBJ
$(PROJ).EXE
$(MAPFILE)
$(LIBS) $(LIBS_EXT)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $(PROJ).RES $@

$(PROJ).EXE:: $(PROJ).RES
	$(RC) $(RESFLAGS) $(PROJ).RES $@
