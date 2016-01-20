CC  = cl -c -W3 -ASw -G3 -Gs -GD -Zp -Oxwt $(DEF)
ASM = masm -Mx -D?SMALL=1
LINK= link /CO/NOPACKC/NOE/NOD/LI/MAP/AL:16
DEF =-DDEBUG -DSTRICT
RC  = rc

OBJDIR = sdllobj
OBJ1 = $(OBJDIR)\$(NAME).obj	$(OBJDIR)\htpal.obj
OBJ2 = $(OBJDIR)\fixed32.obj	$(OBJDIR)\dib.obj \
	$(OBJDIR)\clear.obj	$(OBJDIR)\clear32.obj \
	$(OBJDIR)\blt.obj	$(OBJDIR)\blt32.obj	\
	$(OBJDIR)\tblt.obj	$(OBJDIR)\tblt32.obj	\
	$(OBJDIR)\stretch.obj	$(OBJDIR)\stret32.obj \
	$(OBJDIR)\tstretch.obj	$(OBJDIR)\tstret32.obj \
	$(OBJDIR)\warp.obj	$(OBJDIR)\warp32.obj	\
	$(OBJDIR)\playrle.obj	$(OBJDIR)\df.obj


OBJ  = $(OBJ1) $(OBJ2)

LIBS = libw sdllcew commdlg

.cpp{$(OBJDIR)}.obj:
	$(CC) /Fo$*.obj $<

.c{$(OBJDIR)}.obj:
	$(CC) /Fo$*.obj $<

.asm{$(OBJDIR)}.obj:
	$(ASM) $<, $*.obj;

goal: sdlldib.lib

sdlldib.lib : $(OBJ2)
	if exist sdlldib.lib del sdlldib.lib
	lib sdlldib.lib +$(OBJ2);

clean:
	if exist $(NAME).exe del $(NAME).exe
	if exist *.res del *.res
	if exist *.err del *.err
	if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	if exist *.map del *.map
	if exist *.sym del *.sym
	if exist *.cod del *.cod
	if exist *.pdb del *.pdb



