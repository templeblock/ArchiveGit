CC  = cl -c -W3 -AS -G3 -Gs -GA -Zp -Oxwt -I\wing $(DEF)
ASM = masm -Mx -D?SMALL=1
LINK= link /CO/NOPACKC/NOE/NOD/LI/MAP/AL:16
DEF =-DDEBUG -DSTRICT
RC  = rc

OBJDIR = slibobj
OBJ = $(OBJDIR)\fixed32.obj	$(OBJDIR)\dib.obj \
	$(OBJDIR)\clear.obj	$(OBJDIR)\clear32.obj \
	$(OBJDIR)\blt.obj	$(OBJDIR)\blt32.obj \
	$(OBJDIR)\tblt.obj	$(OBJDIR)\tblt32.obj \
	$(OBJDIR)\stretch.obj	$(OBJDIR)\stret32.obj \
	$(OBJDIR)\tstretch.obj	$(OBJDIR)\tstret32.obj \
	$(OBJDIR)\warp.obj	$(OBJDIR)\warp32.obj	\
	$(OBJDIR)\playrle.obj	$(OBJDIR)\df.obj

LIBS = libw llibcew commdlg

.cpp{$(OBJDIR)}.obj:
	$(CC) /Fo$*.obj $<

.c{$(OBJDIR)}.obj:
	$(CC) /Fo$*.obj $<

.asm{$(OBJDIR)}.obj:
	$(ASM) $<, $*.obj;

goal: slibdib.lib

slibdib.lib : $(OBJ)
	if exist slibdib.lib del slibdib.lib
	lib slibdib.lib +$(OBJ);

clean:
	if exist *.res del *.res
	if exist *.err del *.err
	if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	if exist *.map del *.map
	if exist *.sym del *.sym
	if exist *.cod del *.cod
	if exist *.pdb del *.pdb



