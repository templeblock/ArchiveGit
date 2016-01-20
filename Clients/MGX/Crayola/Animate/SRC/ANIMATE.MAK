####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################

.silent:

RES  = ..\res
OBJ  = ..\obj
DST  = ..\dst

#RAMDRIVE = d:

!if DEFINED(RAMDRIVE)
OBJ     = $(RAMDRIVE)
DST     = $(RAMDRIVE)
TMP     = $(RAMDRIVE)
!message Using ramdrive $(RAMDRIVE)
!endif

			 

####################################################################################

#do_optimize = 1
#cv = 1 
#do_debug = 1

####################################################################################

!if DEFINED(do_optimize)
cSLD = /Ozaxb2/Gr
!else
cSLD = /Od
!endif

####################################################################################


!ifdef cv
lSLD = /co
cSLD = $(cSLD) /Zi
!else
lSLD =
!endif

####################################################################################

!ifdef do_debug
mafxcw = mafxcwd
dodebug = /D_DEBUG

!else

mafxcw = mafxcw
dodebug =
!endif


####################################################################################


baseparms = $(dodebug) /AM /G2As /Zp /I$(INC) $(cSLD)
ccparms   = /c $(baseparms) /Fo$@
cc        = cl


####################################################################################


{.}.cpp{$(OBJ)\}.obj:
    echo Compiling %|fF.cpp...
    echo $(ccparms) >%|fF.opt
    $(cc) /nologo @%|fF.opt %|fF.cpp
    del %|fF.opt >nul
    echo Done.

####################################################################################


all: 	$(DST)\animate.lib    

##############################################################################

##############################################################################


objlist1=\
    $(OBJ)\animate.obj	\
    $(OBJ)\bitmaps.obj  \
    $(OBJ)\app.obj      \
    $(OBJ)\instr.obj

objlist2=\
    $(OBJ)\object.obj   \
    $(OBJ)\script.obj   \
    $(OBJ)\pixelpos.obj




$(OBJ)\animate.obj:  animate.cpp      \
                     object.h         \
                     animate.h        \
                     app.h

$(OBJ)\bitmaps.obj:  bitmaps.cpp      \
                     bitmaps.h        \
                     app.h

$(OBJ)\instr.obj:    instr.cpp        \
                     instr.h          \
                     app.h

$(OBJ)\object.obj:   object.cpp       \
                     object.h         \
                     animate.h        \
                     bitmaps.h        \
                     instr.h          \
                     app.h

$(OBJ)\pixelpos.obj: pixelpos.cpp     \
                     pixelpos.h       \
                     app.h

$(OBJ)\script.obj:   script.cpp        \
                     script.h          \
                     animate.h        \
                     app.h



####################################################################################


$(DST)\animate.lib::    animate.mak    \
                        $(objlist1)    \
                        $(objlist2)
	del $(DST)\animate.lib						
    echo Building %|fF.lib...
	lib /NOI /NOE $@ @<<	
$(objlist1) $(objlist2);
<<
	copy $(DST)\animate.lib \crayola\ref
    echo Done.

##############################################################################
