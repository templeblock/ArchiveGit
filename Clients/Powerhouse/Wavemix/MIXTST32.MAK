!include <ntwin32.mak>

NAME    = MixTest
OBJS    = mixtest.obj

EXENAME = MixTst32.exe
INC     = -I..
LIBS    = winmm.lib wavmix32.lib
DEF     = -DSTRICT

!ifdef DEBUG
CFLAGS=$(cdebug)
LFLAGS=$(linkdebug)
!else
CFLAGS= -O2
LFLAGS=
!endif

.c.obj:
        $(cc) $(cflags) $(cvars) $(CFLAGS) -Zp1 -Fc $(INC) $(DEF) $*.c

.rc.res:
        $(rc) $(rcvars) $(DEF) $(INC) -fo $*.res $*.rc

goal:  $(EXENAME)
        @echo ***** finished making $(NAME) for Win32 $(CPU) *****

$(EXENAME): $(OBJS) $(NAME).res
	$(link) $(LFLAGS) $(guiflags) -align:4096 -section:.rsrc,rw \
        -out:$(EXENAME) $(OBJS) $(NAME).res $(guilibs) $(LIBS)

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
