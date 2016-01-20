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
LLATW32_ddbddbBINbminerb32dexe =  -Tpe -aa -c -LF:\BC45\LIB;..\..\LIB
RLATW32_ddbddbBINbminerb32dexe =  -w32
BLATW32_ddbddbBINbminerb32dexe = 
CNIEAT_ddbddbBINbminerb32dexe = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddbBINbminerb32dexe = -x
LEAT_ddbddbBINbminerb32dexe = $(LLATW32_ddbddbBINbminerb32dexe)
REAT_ddbddbBINbminerb32dexe = $(RLATW32_ddbddbBINbminerb32dexe)
BEAT_ddbddbBINbminerb32dexe = $(BLATW32_ddbddbBINbminerb32dexe)
CLATW16_ddbddblibbpicndlib = 
LLATW16_ddbddblibbpicndlib = 
RLATW16_ddbddblibbpicndlib = 
BLATW16_ddbddblibbpicndlib = 
CEAT_ddbddblibbpicndlib = $(CEAT_ddbddbBINbminerb32dexe) $(CLATW16_ddbddblibbpicndlib)
CNIEAT_ddbddblibbpicndlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_ddbddblibbpicndlib = -x
LEAT_ddbddblibbpicndlib = $(LEAT_ddbddbBINbminerb32dexe) $(LLATW16_ddbddblibbpicndlib)
REAT_ddbddblibbpicndlib = $(REAT_ddbddbBINbminerb32dexe) $(RLATW16_ddbddblibbpicndlib)
BEAT_ddbddblibbpicndlib = $(BEAT_ddbddbBINbminerb32dexe) $(BLATW16_ddbddblibbpicndlib)
CLATW16_FcbBC45bLIBbctl3d32dlib = 
LLATW16_FcbBC45bLIBbctl3d32dlib = 
RLATW16_FcbBC45bLIBbctl3d32dlib = 
BLATW16_FcbBC45bLIBbctl3d32dlib = 
CEAT_FcbBC45bLIBbctl3d32dlib = $(CEAT_ddbddbBINbminerb32dexe) $(CLATW16_FcbBC45bLIBbctl3d32dlib)
CNIEAT_FcbBC45bLIBbctl3d32dlib = -IF:\BC45\INCLUDE;..\..\INCLUDE -D
LNIEAT_FcbBC45bLIBbctl3d32dlib = -x
LEAT_FcbBC45bLIBbctl3d32dlib = $(LEAT_ddbddbBINbminerb32dexe) $(LLATW16_FcbBC45bLIBbctl3d32dlib)
REAT_FcbBC45bLIBbctl3d32dlib = $(REAT_ddbddbBINbminerb32dexe) $(RLATW16_FcbBC45bLIBbctl3d32dlib)
BEAT_FcbBC45bLIBbctl3d32dlib = $(BEAT_ddbddbBINbminerb32dexe) $(BLATW16_FcbBC45bLIBbctl3d32dlib)

#
# Dependency List
#
Dep_minerb32 = \
   ..\..\BIN\minerb32.exe

minerb32 : BccW32.cfg $(Dep_minerb32)
  echo MakeNode 

Dep_ddbddbBINbminerb32dexe = \
   B32\pic2list.obj\
   B32\mopng.obj\
   B32\mowavlet.obj\
   B32\mswavlet.obj\
   ..\..\lib\picn.lib\
   F:\BC45\LIB\ctl3d32.lib\
   B32\minerva.res\
   B32\mabout.obj\
   B32\mchild.obj\
   B32\mctrl.obj\
   B32\mdlg.obj\
   B32\merror.obj\
   B32\mframe.obj\
   B32\minerva.obj\
   B32\mmenu.obj\
   B32\mmisc.obj\
   B32\mmru.obj\
   B32\mobmp.obj\
   B32\moim4.obj\
   B32\moimstar.obj\
   B32\mopen.obj\
   B32\moprgjpg.obj\
   B32\moseqjpg.obj\
   B32\moutil81.obj\
   B32\msave.obj\
   B32\msimstar.obj\
   B32\msprgjpg.obj\
   B32\msseqjpg.obj\
   B32\msutil80.obj\
   B32\mtutil82.obj\
   B32\mxp2s.obj\
   B32\mxrotate.obj\
   B32\mxs2p.obj\
   B32\mxzoom.obj

..\..\BIN\minerb32.exe : $(Dep_ddbddbBINbminerb32dexe)
  $(TLINK32) @&&|
 /v $(IDE_LFLAGS32) $(LEAT_ddbddbBINbminerb32dexe) $(LNIEAT_ddbddbBINbminerb32dexe) +
F:\BC45\LIB\c0w32.obj+
B32\pic2list.obj+
B32\mopng.obj+
B32\mowavlet.obj+
B32\mswavlet.obj+
B32\mabout.obj+
B32\mchild.obj+
B32\mctrl.obj+
B32\mdlg.obj+
B32\merror.obj+
B32\mframe.obj+
B32\minerva.obj+
B32\mmenu.obj+
B32\mmisc.obj+
B32\mmru.obj+
B32\mobmp.obj+
B32\moim4.obj+
B32\moimstar.obj+
B32\mopen.obj+
B32\moprgjpg.obj+
B32\moseqjpg.obj+
B32\moutil81.obj+
B32\msave.obj+
B32\msimstar.obj+
B32\msprgjpg.obj+
B32\msseqjpg.obj+
B32\msutil80.obj+
B32\mtutil82.obj+
B32\mxp2s.obj+
B32\mxrotate.obj+
B32\mxs2p.obj+
B32\mxzoom.obj
$<,$*
..\..\lib\picn.lib+
F:\BC45\LIB\ctl3d32.lib+
F:\BC45\LIB\import32.lib+
F:\BC45\LIB\cw32.lib

|
   $(BRC32) B32\minerva.res $<

B32\pic2list.obj :  pic2list.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ pic2list.c
|

B32\mopng.obj :  mopng.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mopng.c
|

B32\mowavlet.obj :  mowavlet.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mowavlet.c
|

B32\mswavlet.obj :  mswavlet.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mswavlet.c
|

B32\minerva.res :  minerva.rc
  $(BRC32) $(IDE_RFLAGS32) $(REAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -R -FO$@ minerva.rc

B32\mabout.obj :  mabout.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mabout.c
|

B32\mchild.obj :  mchild.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mchild.c
|

B32\mctrl.obj :  mctrl.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mctrl.c
|

B32\mdlg.obj :  mdlg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mdlg.c
|

B32\merror.obj :  merror.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ merror.c
|

B32\mframe.obj :  mframe.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mframe.c
|

B32\minerva.obj :  minerva.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ minerva.c
|

B32\mmenu.obj :  mmenu.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mmenu.c
|

B32\mmisc.obj :  mmisc.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mmisc.c
|

B32\mmru.obj :  mmru.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mmru.c
|

B32\mobmp.obj :  mobmp.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mobmp.c
|

B32\moim4.obj :  moim4.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ moim4.c
|

B32\moimstar.obj :  moimstar.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ moimstar.c
|

B32\mopen.obj :  mopen.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mopen.c
|

B32\moprgjpg.obj :  moprgjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ moprgjpg.c
|

B32\moseqjpg.obj :  moseqjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ moseqjpg.c
|

B32\moutil81.obj :  moutil81.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ moutil81.c
|

B32\msave.obj :  msave.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ msave.c
|

B32\msimstar.obj :  msimstar.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ msimstar.c
|

B32\msprgjpg.obj :  msprgjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ msprgjpg.c
|

B32\msseqjpg.obj :  msseqjpg.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ msseqjpg.c
|

B32\msutil80.obj :  msutil80.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ msutil80.c
|

B32\mtutil82.obj :  mtutil82.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mtutil82.c
|

B32\mxp2s.obj :  mxp2s.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mxp2s.c
|

B32\mxrotate.obj :  mxrotate.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mxrotate.c
|

B32\mxs2p.obj :  mxs2p.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mxs2p.c
|

B32\mxzoom.obj :  mxzoom.c
  $(BCC32) -P- -c @&&|
 $(CEAT_ddbddbBINbminerb32dexe) $(CNIEAT_ddbddbBINbminerb32dexe) -o$@ mxzoom.c
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


