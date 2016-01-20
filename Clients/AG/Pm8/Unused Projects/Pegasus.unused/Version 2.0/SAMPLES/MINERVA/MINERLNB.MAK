#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS32 =  -LD:\BC45\LIB
IDE_RFLAGS32 = 
LLATW32_ddbddbBINbminerlnbdexe =  -Tpe -aa -c -LF:\BC45\LIB;..\..\LIB -wdpl -went -wdup -w-def -wimt -wbdl -wsrf
RLATW32_ddbddbBINbminerlnbdexe =  -w32
BLATW32_ddbddbBINbminerlnbdexe = 
CNIEAT_ddbddbBINbminerlnbdexe = -IF:\BC45\INCLUDE;..\..\INCLUDE -DSSM_RES
LNIEAT_ddbddbBINbminerlnbdexe = -s
LEAT_ddbddbBINbminerlnbdexe = $(LLATW32_ddbddbBINbminerlnbdexe)
REAT_ddbddbBINbminerlnbdexe = $(RLATW32_ddbddbBINbminerlnbdexe)
BEAT_ddbddbBINbminerlnbdexe = $(BLATW32_ddbddbBINbminerlnbdexe)
CLATW16_ddbddblibbpicn20bdlib = 
LLATW16_ddbddblibbpicn20bdlib = 
RLATW16_ddbddblibbpicn20bdlib = 
BLATW16_ddbddblibbpicn20bdlib = 
CEAT_ddbddblibbpicn20bdlib = $(CEAT_ddbddbBINbminerlnbdexe) $(CLATW16_ddbddblibbpicn20bdlib)
CNIEAT_ddbddblibbpicn20bdlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -DSSM_RES
LNIEAT_ddbddblibbpicn20bdlib = -s
LEAT_ddbddblibbpicn20bdlib = $(LEAT_ddbddbBINbminerlnbdexe) $(LLATW16_ddbddblibbpicn20bdlib)
REAT_ddbddblibbpicn20bdlib = $(REAT_ddbddbBINbminerlnbdexe) $(RLATW16_ddbddblibbpicn20bdlib)
BEAT_ddbddblibbpicn20bdlib = $(BEAT_ddbddbBINbminerlnbdexe) $(BLATW16_ddbddblibbpicn20bdlib)
CLATW16_FcbBC45bLIBbctl3d32dlib = 
LLATW16_FcbBC45bLIBbctl3d32dlib = 
RLATW16_FcbBC45bLIBbctl3d32dlib = 
BLATW16_FcbBC45bLIBbctl3d32dlib = 
CEAT_FcbBC45bLIBbctl3d32dlib = $(CEAT_ddbddbBINbminerlnbdexe) $(CLATW16_FcbBC45bLIBbctl3d32dlib)
CNIEAT_FcbBC45bLIBbctl3d32dlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -DSSM_RES
LNIEAT_FcbBC45bLIBbctl3d32dlib = -s
LEAT_FcbBC45bLIBbctl3d32dlib = $(LEAT_ddbddbBINbminerlnbdexe) $(LLATW16_FcbBC45bLIBbctl3d32dlib)
REAT_FcbBC45bLIBbctl3d32dlib = $(REAT_ddbddbBINbminerlnbdexe) $(RLATW16_FcbBC45bLIBbctl3d32dlib)
BEAT_FcbBC45bLIBbctl3d32dlib = $(BEAT_ddbddbBINbminerlnbdexe) $(BLATW16_FcbBC45bLIBbctl3d32dlib)

#
# Dependency List
#
Dep_minerlnb = \
   ..\..\BIN\minerlnb.exe

minerlnb : BccW32.cfg $(Dep_minerlnb)
  echo MakeNode 

Dep_ddbddbBINbminerlnbdexe = \
   LNB\pic2list.obj\
   ..\..\lib\picn20b.lib\
   LNB\mopng.obj\
   LNB\mowavlet.obj\
   LNB\mswavlet.obj\
   F:\BC45\LIB\ctl3d32.lib\
   LNB\minerva.res\
   LNB\mabout.obj\
   LNB\mchild.obj\
   LNB\mctrl.obj\
   LNB\mdlg.obj\
   LNB\merror.obj\
   LNB\mframe.obj\
   LNB\minerva.obj\
   LNB\mmenu.obj\
   LNB\mmisc.obj\
   LNB\mmru.obj\
   LNB\mobmp.obj\
   LNB\moim4.obj\
   LNB\moimstar.obj\
   LNB\mopen.obj\
   LNB\moprgjpg.obj\
   LNB\moseqjpg.obj\
   LNB\moutil81.obj\
   LNB\msave.obj\
   LNB\msimstar.obj\
   LNB\msprgjpg.obj\
   LNB\msseqjpg.obj\
   LNB\msutil80.obj\
   LNB\mtutil82.obj\
   LNB\mxp2s.obj\
   LNB\mxrotate.obj\
   LNB\mxs2p.obj\
   LNB\mxzoom.obj

..\..\BIN\minerlnb.exe : $(Dep_ddbddbBINbminerlnbdexe)
  $(TLINK32) @&&|
 /v $(IDE_LFLAGS32) $(LEAT_ddbddbBINbminerlnbdexe) $(LNIEAT_ddbddbBINbminerlnbdexe) +
F:\BC45\LIB\c0w32.obj+
LNB\pic2list.obj+
LNB\mopng.obj+
LNB\mowavlet.obj+
LNB\mswavlet.obj+
LNB\mabout.obj+
LNB\mchild.obj+
LNB\mctrl.obj+
LNB\mdlg.obj+
LNB\merror.obj+
LNB\mframe.obj+
LNB\minerva.obj+
LNB\mmenu.obj+
LNB\mmisc.obj+
LNB\mmru.obj+
LNB\mobmp.obj+
LNB\moim4.obj+
LNB\moimstar.obj+
LNB\mopen.obj+
LNB\moprgjpg.obj+
LNB\moseqjpg.obj+
LNB\moutil81.obj+
LNB\msave.obj+
LNB\msimstar.obj+
LNB\msprgjpg.obj+
LNB\msseqjpg.obj+
LNB\msutil80.obj+
LNB\mtutil82.obj+
LNB\mxp2s.obj+
LNB\mxrotate.obj+
LNB\mxs2p.obj+
LNB\mxzoom.obj
$<,$*
..\..\lib\picn20b.lib+
F:\BC45\LIB\ctl3d32.lib+
F:\BC45\LIB\import32.lib+
F:\BC45\LIB\cw32.lib

|
   $(BRC32) LNB\minerva.res $<

LNB\pic2list.obj :  pic2list.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ pic2list.c
|

LNB\mopng.obj :  mopng.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mopng.c
|

LNB\mowavlet.obj :  mowavlet.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mowavlet.c
|

LNB\mswavlet.obj :  mswavlet.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mswavlet.c
|

LNB\minerva.res :  minerva.rc
  $(BRC32) $(IDE_RFLAGS32) $(REAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -R -FO$@ minerva.rc

LNB\mabout.obj :  mabout.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mabout.c
|

LNB\mchild.obj :  mchild.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mchild.c
|

LNB\mctrl.obj :  mctrl.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mctrl.c
|

LNB\mdlg.obj :  mdlg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mdlg.c
|

LNB\merror.obj :  merror.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ merror.c
|

LNB\mframe.obj :  mframe.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mframe.c
|

LNB\minerva.obj :  minerva.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ minerva.c
|

LNB\mmenu.obj :  mmenu.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mmenu.c
|

LNB\mmisc.obj :  mmisc.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mmisc.c
|

LNB\mmru.obj :  mmru.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mmru.c
|

LNB\mobmp.obj :  mobmp.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mobmp.c
|

LNB\moim4.obj :  moim4.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ moim4.c
|

LNB\moimstar.obj :  moimstar.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ moimstar.c
|

LNB\mopen.obj :  mopen.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mopen.c
|

LNB\moprgjpg.obj :  moprgjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ moprgjpg.c
|

LNB\moseqjpg.obj :  moseqjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ moseqjpg.c
|

LNB\moutil81.obj :  moutil81.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ moutil81.c
|

LNB\msave.obj :  msave.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ msave.c
|

LNB\msimstar.obj :  msimstar.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ msimstar.c
|

LNB\msprgjpg.obj :  msprgjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ msprgjpg.c
|

LNB\msseqjpg.obj :  msseqjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ msseqjpg.c
|

LNB\msutil80.obj :  msutil80.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ msutil80.c
|

LNB\mtutil82.obj :  mtutil82.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mtutil82.c
|

LNB\mxp2s.obj :  mxp2s.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mxp2s.c
|

LNB\mxrotate.obj :  mxrotate.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mxrotate.c
|

LNB\mxs2p.obj :  mxs2p.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mxs2p.c
|

LNB\mxzoom.obj :  mxzoom.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerlnbdexe) $(CNIEAT_ddbddbBINbminerlnbdexe) -o$@ mxzoom.c
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=minervab.csm
-W
-wcln
-wsig
-wucp
-wbbf
-wpin
-wnak
-wpre
-wdef
-wnod
-wamb
-w-use
-wstv
-wasm
-wamp
-wobs
-wpch
-w-msg
-w-aus
-w-eff
| $@


