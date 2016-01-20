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
LLATW16_ddbddbBINbminervbldexe =  -Twe -c -C -LF:\BC45\LIB;..\..\LIB -3 -P -P=4096
RLATW16_ddbddbBINbminervbldexe =  -31
BLATW16_ddbddbBINbminervbldexe = 
CNIEAT_ddbddbBINbminervbldexe = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddbBINbminervbldexe = -x
LEAT_ddbddbBINbminervbldexe = $(LLATW16_ddbddbBINbminervbldexe)
REAT_ddbddbBINbminervbldexe = $(RLATW16_ddbddbBINbminervbldexe)
BEAT_ddbddbBINbminervbldexe = $(BLATW16_ddbddbBINbminervbldexe)
CLATW16_ddbddblibbpicwdlib = 
LLATW16_ddbddblibbpicwdlib = 
RLATW16_ddbddblibbpicwdlib = 
BLATW16_ddbddblibbpicwdlib = 
CEAT_ddbddblibbpicwdlib = $(CEAT_ddbddbBINbminervbldexe) $(CLATW16_ddbddblibbpicwdlib)
CNIEAT_ddbddblibbpicwdlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddblibbpicwdlib = -x
LEAT_ddbddblibbpicwdlib = $(LEAT_ddbddbBINbminervbldexe) $(LLATW16_ddbddblibbpicwdlib)
REAT_ddbddblibbpicwdlib = $(REAT_ddbddbBINbminervbldexe) $(RLATW16_ddbddblibbpicwdlib)
BEAT_ddbddblibbpicwdlib = $(BEAT_ddbddbBINbminervbldexe) $(BLATW16_ddbddblibbpicwdlib)
CLATW16_FcbBC45bLIBbctl3dv2dlib = 
LLATW16_FcbBC45bLIBbctl3dv2dlib = 
RLATW16_FcbBC45bLIBbctl3dv2dlib = 
BLATW16_FcbBC45bLIBbctl3dv2dlib = 
CEAT_FcbBC45bLIBbctl3dv2dlib = $(CEAT_ddbddbBINbminervbldexe) $(CLATW16_FcbBC45bLIBbctl3dv2dlib)
CNIEAT_FcbBC45bLIBbctl3dv2dlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_FcbBC45bLIBbctl3dv2dlib = -x
LEAT_FcbBC45bLIBbctl3dv2dlib = $(LEAT_ddbddbBINbminervbldexe) $(LLATW16_FcbBC45bLIBbctl3dv2dlib)
REAT_FcbBC45bLIBbctl3dv2dlib = $(REAT_ddbddbBINbminervbldexe) $(RLATW16_FcbBC45bLIBbctl3dv2dlib)
BEAT_FcbBC45bLIBbctl3dv2dlib = $(BEAT_ddbddbBINbminervbldexe) $(BLATW16_FcbBC45bLIBbctl3dv2dlib)

#
# Dependency List
#
Dep_minervbl = \
   ..\..\BIN\minervbl.exe

minervbl : BccW16.cfg $(Dep_minervbl)
  echo MakeNode 

Dep_ddbddbBINbminervbldexe = \
   BL16\pic2list.obj\
   BL16\mopng.obj\
   BL16\mowavlet.obj\
   BL16\mswavlet.obj\
   ..\..\lib\picw.lib\
   F:\BC45\LIB\ctl3dv2.lib\
   minerva.def\
   BL16\minerva.res\
   BL16\mabout.obj\
   BL16\mchild.obj\
   BL16\mctrl.obj\
   BL16\mdlg.obj\
   BL16\merror.obj\
   BL16\mframe.obj\
   BL16\minerva.obj\
   BL16\mmenu.obj\
   BL16\mmisc.obj\
   BL16\mmru.obj\
   BL16\mobmp.obj\
   BL16\moim4.obj\
   BL16\moimstar.obj\
   BL16\mopen.obj\
   BL16\moprgjpg.obj\
   BL16\moseqjpg.obj\
   BL16\moutil81.obj\
   BL16\msave.obj\
   BL16\msimstar.obj\
   BL16\msprgjpg.obj\
   BL16\msseqjpg.obj\
   BL16\msutil80.obj\
   BL16\mtutil82.obj\
   BL16\mxp2s.obj\
   BL16\mxrotate.obj\
   BL16\mxs2p.obj\
   BL16\mxzoom.obj

..\..\BIN\minervbl.exe : $(Dep_ddbddbBINbminervbldexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_ddbddbBINbminervbldexe) $(LNIEAT_ddbddbBINbminervbldexe) +
F:\BC45\LIB\c0wl.obj+
BL16\pic2list.obj+
BL16\mopng.obj+
BL16\mowavlet.obj+
BL16\mswavlet.obj+
BL16\mabout.obj+
BL16\mchild.obj+
BL16\mctrl.obj+
BL16\mdlg.obj+
BL16\merror.obj+
BL16\mframe.obj+
BL16\minerva.obj+
BL16\mmenu.obj+
BL16\mmisc.obj+
BL16\mmru.obj+
BL16\mobmp.obj+
BL16\moim4.obj+
BL16\moimstar.obj+
BL16\mopen.obj+
BL16\moprgjpg.obj+
BL16\moseqjpg.obj+
BL16\moutil81.obj+
BL16\msave.obj+
BL16\msimstar.obj+
BL16\msprgjpg.obj+
BL16\msseqjpg.obj+
BL16\msutil80.obj+
BL16\mtutil82.obj+
BL16\mxp2s.obj+
BL16\mxrotate.obj+
BL16\mxs2p.obj+
BL16\mxzoom.obj
$<,$*
..\..\lib\picw.lib+
F:\BC45\LIB\ctl3dv2.lib+
F:\BC45\LIB\import.lib+
F:\BC45\LIB\mathwl.lib+
F:\BC45\LIB\cwl.lib
minerva.def
|
   $(BRC) BL16\minerva.res $<

BL16\pic2list.obj :  pic2list.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ pic2list.c
|

BL16\mopng.obj :  mopng.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mopng.c
|

BL16\mowavlet.obj :  mowavlet.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mowavlet.c
|

BL16\mswavlet.obj :  mswavlet.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mswavlet.c
|

BL16\minerva.res :  minerva.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -R -FO$@ minerva.rc

BL16\mabout.obj :  mabout.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mabout.c
|

BL16\mchild.obj :  mchild.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mchild.c
|

BL16\mctrl.obj :  mctrl.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mctrl.c
|

BL16\mdlg.obj :  mdlg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mdlg.c
|

BL16\merror.obj :  merror.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ merror.c
|

BL16\mframe.obj :  mframe.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mframe.c
|

BL16\minerva.obj :  minerva.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ minerva.c
|

BL16\mmenu.obj :  mmenu.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mmenu.c
|

BL16\mmisc.obj :  mmisc.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mmisc.c
|

BL16\mmru.obj :  mmru.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mmru.c
|

BL16\mobmp.obj :  mobmp.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mobmp.c
|

BL16\moim4.obj :  moim4.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ moim4.c
|

BL16\moimstar.obj :  moimstar.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ moimstar.c
|

BL16\mopen.obj :  mopen.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mopen.c
|

BL16\moprgjpg.obj :  moprgjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ moprgjpg.c
|

BL16\moseqjpg.obj :  moseqjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ moseqjpg.c
|

BL16\moutil81.obj :  moutil81.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ moutil81.c
|

BL16\msave.obj :  msave.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ msave.c
|

BL16\msimstar.obj :  msimstar.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ msimstar.c
|

BL16\msprgjpg.obj :  msprgjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ msprgjpg.c
|

BL16\msseqjpg.obj :  msseqjpg.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ msseqjpg.c
|

BL16\msutil80.obj :  msutil80.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ msutil80.c
|

BL16\mtutil82.obj :  mtutil82.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mtutil82.c
|

BL16\mxp2s.obj :  mxp2s.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mxp2s.c
|

BL16\mxrotate.obj :  mxrotate.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mxrotate.c
|

BL16\mxs2p.obj :  mxs2p.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mxs2p.c
|

BL16\mxzoom.obj :  mxzoom.c
  $(BCC)   -P- -c @&&|
 $(CEAT_ddbddbBINbminervbldexe) $(CNIEAT_ddbddbBINbminervbldexe) -o$@ mxzoom.c
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=minervbl.csm
-ml
-WS
-wcln
-w-sig
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
-w-eff
-Ff=32767
-Ff-
-dc
| $@


