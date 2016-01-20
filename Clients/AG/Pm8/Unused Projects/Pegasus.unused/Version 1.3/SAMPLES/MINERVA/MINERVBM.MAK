#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -LD:\BC45\LIB
IDE_RFLAGS = 
LLATW16_ddbddbBINbminervbmdexe =  -Twe -c -C -LF:\BC45\LIB;..\..\LIB -3 -P -P=4096
RLATW16_ddbddbBINbminervbmdexe =  -31
BLATW16_ddbddbBINbminervbmdexe = 
CNIEAT_ddbddbBINbminervbmdexe = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddbBINbminervbmdexe = -x
LEAT_ddbddbBINbminervbmdexe = $(LLATW16_ddbddbBINbminervbmdexe)
REAT_ddbddbBINbminervbmdexe = $(RLATW16_ddbddbBINbminervbmdexe)
BEAT_ddbddbBINbminervbmdexe = $(BLATW16_ddbddbBINbminervbmdexe)
CLATW16_FcbBC45bLIBbctl3dv2dlib = 
LLATW16_FcbBC45bLIBbctl3dv2dlib = 
RLATW16_FcbBC45bLIBbctl3dv2dlib = 
BLATW16_FcbBC45bLIBbctl3dv2dlib = 
CEAT_FcbBC45bLIBbctl3dv2dlib = $(CEAT_ddbddbBINbminervbmdexe) $(CLATW16_FcbBC45bLIBbctl3dv2dlib)
CNIEAT_FcbBC45bLIBbctl3dv2dlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_FcbBC45bLIBbctl3dv2dlib = -x
LEAT_FcbBC45bLIBbctl3dv2dlib = $(LEAT_ddbddbBINbminervbmdexe) $(LLATW16_FcbBC45bLIBbctl3dv2dlib)
REAT_FcbBC45bLIBbctl3dv2dlib = $(REAT_ddbddbBINbminervbmdexe) $(RLATW16_FcbBC45bLIBbctl3dv2dlib)
BEAT_FcbBC45bLIBbctl3dv2dlib = $(BEAT_ddbddbBINbminervbmdexe) $(BLATW16_FcbBC45bLIBbctl3dv2dlib)
CLATW16_ddbddblibbpicwdlib = 
LLATW16_ddbddblibbpicwdlib = 
RLATW16_ddbddblibbpicwdlib = 
BLATW16_ddbddblibbpicwdlib = 
CEAT_ddbddblibbpicwdlib = $(CEAT_ddbddbBINbminervbmdexe) $(CLATW16_ddbddblibbpicwdlib)
CNIEAT_ddbddblibbpicwdlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddblibbpicwdlib = -x
LEAT_ddbddblibbpicwdlib = $(LEAT_ddbddbBINbminervbmdexe) $(LLATW16_ddbddblibbpicwdlib)
REAT_ddbddblibbpicwdlib = $(REAT_ddbddbBINbminervbmdexe) $(RLATW16_ddbddblibbpicwdlib)
BEAT_ddbddblibbpicwdlib = $(BEAT_ddbddbBINbminervbmdexe) $(BLATW16_ddbddblibbpicwdlib)

#
# Dependency List
#
Dep_minervbm = \
   ..\..\BIN\minervbm.exe

minervbm : BccW16.cfg $(Dep_minervbm)
  echo MakeNode 

Dep_ddbddbBINbminervbmdexe = \
   BM16\mopng.obj\
   BM16\mowavlet.obj\
   BM16\mswavlet.obj\
   F:\BC45\LIB\ctl3dv2.lib\
   ..\..\lib\picw.lib\
   BM16\minerva.res\
   minerva.def\
   BM16\mabout.obj\
   BM16\mchild.obj\
   BM16\mctrl.obj\
   BM16\mdlg.obj\
   BM16\merror.obj\
   BM16\mframe.obj\
   BM16\minerva.obj\
   BM16\mmenu.obj\
   BM16\mmisc.obj\
   BM16\mmru.obj\
   BM16\mobmp.obj\
   BM16\moim4.obj\
   BM16\moimstar.obj\
   BM16\mopen.obj\
   BM16\moprgjpg.obj\
   BM16\moseqjpg.obj\
   BM16\moutil81.obj\
   BM16\msave.obj\
   BM16\msimstar.obj\
   BM16\msprgjpg.obj\
   BM16\msseqjpg.obj\
   BM16\msutil80.obj\
   BM16\mtutil82.obj\
   BM16\mxp2s.obj\
   BM16\mxrotate.obj\
   BM16\mxs2p.obj\
   BM16\mxzoom.obj

..\..\BIN\minervbm.exe : $(Dep_ddbddbBINbminervbmdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_ddbddbBINbminervbmdexe) $(LNIEAT_ddbddbBINbminervbmdexe) +
F:\BC45\LIB\c0wm.obj+
BM16\mopng.obj+
BM16\mowavlet.obj+
BM16\mswavlet.obj+
BM16\mabout.obj+
BM16\mchild.obj+
BM16\mctrl.obj+
BM16\mdlg.obj+
BM16\merror.obj+
BM16\mframe.obj+
BM16\minerva.obj+
BM16\mmenu.obj+
BM16\mmisc.obj+
BM16\mmru.obj+
BM16\mobmp.obj+
BM16\moim4.obj+
BM16\moimstar.obj+
BM16\mopen.obj+
BM16\moprgjpg.obj+
BM16\moseqjpg.obj+
BM16\moutil81.obj+
BM16\msave.obj+
BM16\msimstar.obj+
BM16\msprgjpg.obj+
BM16\msseqjpg.obj+
BM16\msutil80.obj+
BM16\mtutil82.obj+
BM16\mxp2s.obj+
BM16\mxrotate.obj+
BM16\mxs2p.obj+
BM16\mxzoom.obj
$<,$*
F:\BC45\LIB\ctl3dv2.lib+
..\..\lib\picw.lib+
F:\BC45\LIB\import.lib+
F:\BC45\LIB\mathwm.lib+
F:\BC45\LIB\cwm.lib
minerva.def
|
   $(BRC) BM16\minerva.res $<

BM16\mopng.obj :  mopng.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mopng.c
|

BM16\mowavlet.obj :  mowavlet.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mowavlet.c
|

BM16\mswavlet.obj :  mswavlet.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mswavlet.c
|

BM16\minerva.res :  minerva.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -R -FO$@ minerva.rc

BM16\mabout.obj :  mabout.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mabout.c
|

BM16\mchild.obj :  mchild.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mchild.c
|

BM16\mctrl.obj :  mctrl.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mctrl.c
|

BM16\mdlg.obj :  mdlg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mdlg.c
|

BM16\merror.obj :  merror.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ merror.c
|

BM16\mframe.obj :  mframe.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mframe.c
|

BM16\minerva.obj :  minerva.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ minerva.c
|

BM16\mmenu.obj :  mmenu.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mmenu.c
|

BM16\mmisc.obj :  mmisc.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mmisc.c
|

BM16\mmru.obj :  mmru.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mmru.c
|

BM16\mobmp.obj :  mobmp.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mobmp.c
|

BM16\moim4.obj :  moim4.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ moim4.c
|

BM16\moimstar.obj :  moimstar.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ moimstar.c
|

BM16\mopen.obj :  mopen.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mopen.c
|

BM16\moprgjpg.obj :  moprgjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ moprgjpg.c
|

BM16\moseqjpg.obj :  moseqjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ moseqjpg.c
|

BM16\moutil81.obj :  moutil81.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ moutil81.c
|

BM16\msave.obj :  msave.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ msave.c
|

BM16\msimstar.obj :  msimstar.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ msimstar.c
|

BM16\msprgjpg.obj :  msprgjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ msprgjpg.c
|

BM16\msseqjpg.obj :  msseqjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ msseqjpg.c
|

BM16\msutil80.obj :  msutil80.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ msutil80.c
|

BM16\mtutil82.obj :  mtutil82.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mtutil82.c
|

BM16\mxp2s.obj :  mxp2s.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mxp2s.c
|

BM16\mxrotate.obj :  mxrotate.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mxrotate.c
|

BM16\mxs2p.obj :  mxs2p.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mxs2p.c
|

BM16\mxzoom.obj :  mxzoom.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbmdexe) $(CNIEAT_ddbddbBINbminervbmdexe) -o$@ mxzoom.c
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=minervab.csm
-mm
-WS
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
-H
-w-eff
-dc-
-wpar
| $@


