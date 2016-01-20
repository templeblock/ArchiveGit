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
!IF ("$(BUILD)" == "NT")
!INCLUDE "$(MAKDIR)\win32.mak"    
!ELSE  
!INCLUDE "$(MAKDIR)\win16.mak"    
!ENDIF

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

!if "$(MAC)" == "1"
OBJDIR = $(OBJDIR)\mac
!endif


# where should any resulting releases go
!IFNDEF DSTDIR
DSTDIR=dst
!ENDIF

# where should resulting resource files go
!IFNDEF RESDIR
RESDIR=res
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
!IF ("$(BUILD)" == "NT")
!IF ("$(CPU)" == "i386")
MASM		= masm386
CODEGEN	= -G4
!ENDIF
ASMGEN		= -DC_CODE
RCFLAGS2	= -v
!ELSE
MASM     	= masm
!IFDEF BUILDDLL
CODEGEN	= -G3 -Gs -GD -ALw -Gy -D_WINDLL -D_AFXDLL
!ELSE
CODEGEN	= -G3 -Gs -GA -AL -Gy
!ENDIF
RCFLAGS2	= -k -t -v
!ENDIF

!IFDEF BROWSE
CODEGEN = $(CODEGEN) -Fr
!ENDIF

!if "$(MAC)" == "1"
CODEGEN  = /nologo /W3 /c     /YX /O2 /AL /Q68s  /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MAC"  /FR /DC_CODE /DSTRICT /I $(MACINC) /I $(WIN32INC)
!else
CODEGEN  = /nologo /W3 /c     /YX /O2 /Zi /D "NDEBUG" /D "_WINDOWS" /D "WIN32"  /FR /DC_CODE /DSTRICT /I $(WIN32INC)
!endif


!IFDEF BUILDDLL
DLLFLAGS   = -DDLL -D_DLL
!ENDIF
LIBEXE   =$(implib)
LINK     =$(link)
RC       =$(rc)
THEFLAGS = $(CODEGEN) $(cflags) $(cdebug) $(cvarsmt) $(FLAGS) $(DLLFLAGS) $(ASMGEN) 
COMPILEFLAGS=  $(THEFLAGS)  
RCFLAGS1 =-r
INCLUDEFLAGS=-I$(REFDIR)

##############################################################################
#
#--- additional flags for build types
#
PCHUSE=-Yu"PP.H"
PCHFILE=-Fp$(PCHDST)\pp.pch
PCHCREATE=-Yc"PP.H" 
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

#!IFNDEF NOINSTALL
#install: $(INSTALLLIST)
#	  
#$(INSTALLLIST) : $$(@F)
#	@copy $(@F) $@
#!ELSE
#install: default
#
#!ENDIF

$(REFDIR):
	@md $(REFDIR)

$(OBJDIR):
	@md $(OBJDIR)
	
cc = cl

##############################################################################
#
#--- Implicit Rules
#    make any you think you will need.  They can be identical in nature,
#    but NMAKE wants have at least one for each possible virtual combination
#
.c{$(OBJDIR)}.obj:
    @echo Compiling $(@B).c ...
    $(CC) /nologo @<< $(CTYPE) $< > $(@B).err
-DSTRICT $(PROJFLAGS) $(COMPILEFLAGS) $(INCLUDEFLAGS) $(PCHFLAGS) -Fo$*.obj
<<
    @type $(@B).err 

.cpp{$(OBJDIR)}.obj:
    @echo Compiling $(@B).cpp ...
	$(cc) /nologo -DSTRICT $(PROJFLAGS) $(COMPILEFLAGS) $(INCLUDEFLAGS) $(PCHFLAGS) -Fo$*.obj $<  > $(@B).err
    @type $(@B).err 

.asm{$(OBJDIR)}.obj:
    @echo Compiling $(@B).asm ...
	$(MASM) -Zi $<,$@; > $(@B).err 
    @type $(@B).err 
