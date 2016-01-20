ORIGIN		= Symantec C++
ORIGIN_VER	= Version 6.0
VERSION		= DEBUG

PROJ		= WTWIN
APPTYPE		= WIN32S EXE
PROJTYPE	= EXE

CC		= SC
MAKE		= MAKE
RC		= RCC
HC		= HC
ASM		= SC
DISASM		= OBJ2ASM
LIBR		= IMPLIB
LNK		= LINK386
CVPK		= CVPACK

DLLS		= 

HEADERS		= dos-keyb.h error.h fixed.h  \
		framebuf.h graphics.h input.h c:\tools\sc\include\limits.h  \
		list.h render.h table.h texture.h  \
		view.h wingdll.h world.h worldfile.h  \
		wt.h wtmem.h wtwin.h 

LIBS		= comdlg32.lib winmm.lib KERNEL32.LIB GDI32.LIB USER32.LIB

DEFFILE		= WTWIN.DEF

CFLAGS		=  -Jm -mn -o+time -W- -3 -a8 -c 
HFLAGS		=  $(CFLAGS) 
LFLAGS		=  
MFLAGS		= MASTERPROJ=$(PROJ) 
RESFLAGS	=  
DEBUGGERFLAGS	=  -LOADSYMBOLS 
IMPLIBFLAGS	=  -IMPLIB -DLL 
AFLAGS		= -c
HELPFLAGS	= 

MODEL		= N

DEFINES		= /DWIN32
RCDEFINES	= 
LIBDIRS		= 
INCLUDES	= 

OBJS		=  error.OBJ  fixed.OBJ  framebuf.OBJ  \
		 gifload.OBJ  list.OBJ  render.OBJ  table.OBJ  \
		 texture.OBJ  view.OBJ  wingraph.OBJ  wininput.OBJ  \
		 world.OBJ  worldfil.OBJ  wt.OBJ  wtmem.OBJ  \
		 wingdll.OBJ  winpal.OBJ  wtwin.OBJ 

RCFILES		= 

RESFILES	= 

SYMS		= 

HELPFILES	= 

BATS		=  

.C.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.c

.CPP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cpp

.CXX.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cxx

.CP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cp

.H.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$*.sym $*.h

.HPP.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$*.sym $*.hpp

.HXX.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o$*.sym $*.hxx

.C.EXP:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -e $*.c -l$*.lst

.CPP.EXP:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -e $*.cpp -l$*.lst

.CXX.EXP:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -e $*.cxx -l$*.lst

.CP.EXP:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -e $*.cp -l$*.lst

.ASM.EXP:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -e $*.asm -l$*.lst

.OBJ.COD:
	$(DISASM) $*.OBJ >$*.cod

.EXE.COD:
	$(DISASM) $*.EXE >$*.cod

.COM.COD:
	$(DISASM) $*.COM >$*.cod

.OBJ.EXE:
	$(LNK) $(LFLAGS) @$(PROJ)sc.LNK

.OBJ.COM:
	$(LNK) $(LFLAGS) @$(PROJ)sc.LNK

.DLL.LIB:
	$(LIBR) $*.LIB $*.DLL

.RTF.HLP:
	$(HC) $(HELPFLAGS) $*.HPJ

.ASM.OBJ:
	$(ASM) $(AFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.asm

.RC.RES: 
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $*.rc

.DLG.RES:
	echo \#include "windows.h" >$$$*.rc
	echo \#include "$*.h" >>$$$*.rc
	echo \#include "$*.dlg" >>$$$*.rc
	$(RC) $(RCDEFINES) $(RESFLAGS) $$$*.rc
	-del $*.res
	-ren $$$*.res $*.res



all:		$(PROJ).$(PROJTYPE) $(POSTLINK) done

$(PROJ).$(PROJTYPE):	$(PRECOMPILE) $(OBJS) $(RCFILES) $(RESFILES) $(HELPFILES) $(POSTCOMPILE) 
			$(LNK) $(LFLAGS) @$(PROJ)sc.LNK
			-del $(PROJ).$(PROJTYPE)
			-ren $$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)
			-echo $(PROJ).$(PROJTYPE) built

done:
		-echo $(PROJ).$(PROJTYPE) done

buildall:	clean	all


clean:
		-del $(PROJ).$(PROJTYPE)
		-del SCPH.SYM
		-del error.OBJ
		-del fixed.OBJ
		-del framebuf.OBJ
		-del gifload.OBJ
		-del list.OBJ
		-del render.OBJ
		-del slice.OBJ
		-del table.OBJ
		-del texture.OBJ
		-del view.OBJ
		-del wingraph.OBJ
		-del wininput.OBJ
		-del world.OBJ
		-del worldfil.OBJ
		-del wt.OBJ
		-del wtmem.OBJ
		-del wingdll.OBJ
		-del winpal.OBJ
		-del wtwin.OBJ

cleanres:

res:		cleanres $(RCFILES) link


link:
		$(LNK) $(LFLAGS) @$(PROJ)sc.LNK
		-del $(PROJ).$(PROJTYPE)
		-ren $$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




include WTWIN.dep



