##############################################################################
#                                                                            #
#  (C) Copyright 1992 by Micrografx, Inc.  All rights reserved.              #
#                                                                            #
#  This material is confidential and considered a trade secret.              #
#   Permission to use this work for any purpose must by obtained in writing  #
#   from Micrografx Inc., 1303 Arapaho, Richardson, TX. 75081.               #
#                                                                            #
##############################################################################
#                                                                            #
#                     standard makefile insert                            	  #
#                                                                            #
##############################################################################
#
##############################################################################
#
#--- personalizable local information
#
!INCLUDE "ntwin32.mak"

##############################################################################
#
#--- Default variable setup
#
# top level directory of the project
!IFNDEF HOMEDIR
HOMEDIR=.
!ENDIF

# where any documentation about the project lives
!IFNDEF DOCDIR
DOCDIR=doc
!ENDIF

# where should any resulting object and/or precompiled headers should go
!IFNDEF OBJDIR
OBJDIR=obj
!ENDIF

# where should any resulting releases go
!IFNDEF DSTDIR
DSTDIR=dst
!ENDIF

# where should resulting resource files go
!IFNDEF RESDIR
RESDIR=res
!ENDIF

# where should resulting resource files go
!IFNDEF PCHDST
PCHDST=.
!ENDIF

# where should resulting resource files go
!IFNDEF PCHSRC
PCHSRC=.
!ENDIF

##############################################################################
#
#--- where to put listing files
#
!IFNDEF LISTFILE
LSTFILE=nul
!ENDIF

!IFNDEF MAPFILE
LSTFILE=nul
!ENDIF

!IFNDEF CTYPE
CTYPE=-Tp
!ENDIF


##############################################################################
#
#--- Required include for NT
#
!IFDEF RELEASE
AFXDEBUG=
AFXLIB=
!ELSE
AFXDEBUG=/D "_DEBUG"
AFXLIB=D
!ENDIF

!IF ("$(CPU)" == "i386")
MASM		= masm386
CODEGEN	= -G4
!ENDIF
ASMGEN		= -DC_CODE
RCFLAGS2	= -v

!IFDEF BROWSE
CODEGEN = $(CODEGEN) -Fr
!ENDIF


!IFDEF BUILDDLL
# don't add DLL or _DLL here, cause it fucks up the link
DLLFLAGS   = -D_WINDLL
!ELSE
CODEGEN=$(CODEGEN)
!ENDIF
LIBEXE   =$(implib)
LINK     =$(link)
RC       =$(rc)
THEFLAGS = $(AFXDEBUG) -DOLD_COLORMAP -D_WINDOWS -D_MBCS $(CODEGEN) -GX /MT $(cflags) $(cdebug) $(cvarsmt) $(FLAGS) $(DLLFLAGS) $(ASMGEN) 
COMPILEFLAGS=  $(THEFLAGS)  
RCFLAGS1 =-r
INCLUDEFLAGS=-I$(REFDIR)

##############################################################################
#
#--- additional flags for build types
#
PCHUSE=-Yu"$(PCHNAME).H"
PCHFILE=-Fp$(PCHDST)\$(PCHNAME).pch
PCHCREATE=-Yc"$(PCHNAME).H" 
!IFNDEF NOPCH
PCHFLAGS=$(PCHUSE) $(PCHFILE)
!ENDIF
PCHDEBUG=-Ob1i
!IFDEF NODEBUG
PCHCOMPILEFLAGS = $(THEFLAGS) $(PCHDEBUG) 
!ELSE
!IFDEF PROFILE
PCHCOMPILEFLAGS = $(THEFLAGS) $(PCHDEBUG) 
!ELSE
!IFDEF TUNE
PCHCOMPILEFLAGS = $(THEFLAGS) $(PCHDEBUG) 
!ELSE
PCHCOMPILEFLAGS = $(COMPILEFLAGS) -Yd
!ENDIF
!ENDIF
!ENDIF

default: $(REFDIR) $(DEFAULTBUILD)

!IFNDEF NOINSTALL
install: default $(INSTALLLIST)
	  
$(INSTALLLIST) : $$(@F)
	@copy $(@F) $@
!ELSE
install: default
!ENDIF

$(REFDIR):
	@md $(REFDIR)

$(OBJDIR):
	@md $(OBJDIR)

$(DSTDIR):
	@md $(DSTDIR)
 
##############################################################################
#
#--- Implicit Rules
#    make any you think you will need.  They can be identical in nature,
#    but NMAKE wants have at least one for each possible virtual combination
#
.c{$(OBJDIR)}.obj:
    @echo Compiling $(@B).c ...
	$(cc) /nologo -DSTRICT $(PROJFLAGS) $(COMPILEFLAGS) $(INCLUDEFLAGS) $(PCHFLAGS) -Fo$*.obj $(CTYPE) $<  > $(@B).err
    @type $(@B).err 

.cpp{$(OBJDIR)}.obj:
    @echo Compiling $(@B).cpp ...
	$(cc) /nologo -DSTRICT $(PROJFLAGS) $(COMPILEFLAGS) $(INCLUDEFLAGS) $(PCHFLAGS) -Fo$*.obj $<  > $(@B).err
    @type $(@B).err 

.asm{$(OBJDIR)}.obj:
    @echo Compiling $(@B).asm ...
	$(MASM) -Zi $<,$@; > $(@B).err 
    @type $(@B).err 

.rc.res:
	rc -r $(rcvars) $(RESINC) $<
	
$(OBJDIR)\$(PCHNAME).obj : {$(VPATH)}$(PCHNAME).cpp $(PCHSRC)\$(PCHNAME).h $(PCHDEPEND)
     @echo Compiling Precompiled Header ...
	$(cc) /nologo $(PROJFLAGS) $(INCLUDEFLAGS) $(PCHCOMPILEFLAGS) $(PCHCREATE) $(PCHFILE) -Fo$(OBJDIR)\$(@B).obj $(@B).cpp  > $(@B).err
    @type $(@B).err 
     
 

