!include <ntwin32.mak>

NAME    = WavMix32
OBJS    = wavemix.obj

EXENAME = $(NAME).dll
RESFILE = wavemix.res
DEFFILE = $(NAME).def
INC     =
LIBS    = winmm.lib
DEF     =

!ifdef DEBUG
CFLAGS=$(cdebug)
LFLAGS=$(linkdebug)
!else
CFLAGS= -O2
LFLAGS=
!endif

.c.obj:
       $(cc) $(cflags) $(cvars) $(CFLAGS) -Fc $(INC) $(DEF) $*.c

.rc.res:
        $(rc) $(rcvars) $(DEF) $(INC) -fo $*.res $*.rc

goal:  $(EXENAME)
        @echo ***** finished making $(NAME) for Win32 $(CPU) *****

$(EXENAME): $(OBJS) $(RESFILE) $(DEFFILE)
        lib /def:$(DEFFILE)
        $(link) @<<
        -dll
        -def:$(DEFFILE)
		$(LFLAGS)
        $(guilflags)
        -align:4096
        -entry:DLLEntryPoint$(DLLENTRY)
        -out:$(EXENAME)
        $(OBJS)
        $(RESFILE)
        $(guilibsdll)
        $(LIBS)
<<

#       $(NAME)32.exp

clean:
	 del *.obj
	 del *.pdb
	 del *.res
	 del *.rbj
	 del *.exp
	 del *.lst
	 del *.sym
	 del *.map
	 del *.exe
         del *.sbr
         del *.sbt
         del *.cod
