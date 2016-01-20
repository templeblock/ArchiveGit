# =========================================================================
# WIN16.MAK - Win16 application master NMAKE definitions file for the
#               Microsoft Win16 SDK for Windows programming samples
# -------------------------------------------------------------------------
# This files should be included at the top of all MAKEFILEs as follows:
#  !include <win16.mak>
# -------------------------------------------------------------------------
# NMAKE Options
#
# Use the table below to determine the additional options for NMAKE to
# generate various application debugging, profiling and performance tuning
# information.
#
# Application Information Type         Invoke NMAKE
# ----------------------------         ------------
# For No Debugging Info                nmake nodebug=1
# For Working Set Tuner Info           nmake tune=1
# For Call Attributed Profiling Info   nmake profile=1
#
# Note: Working Set Tuner and Call Attributed Profiling is for available
#       for the Intel x86 and Pentium systems.
#
# Note: The three options above are mutually exclusive (you may use only
#       one to compile/link the application).
#
# Note: creating the environment variables NODEBUG, TUNE, and PROFILE is an
#       alternate method to setting these options via the nmake command line.
#
# Additional NMAKE Options             Invoke NMAKE
# ----------------------------         ------------
# For No ANSI NULL Compliance          nmake no_ansi=1
# (ANSI NULL is defined as PVOID 0)
#
# =========================================================================

# -------------------------------------------------------------------------
# Get CPU Type - exit if CPU environment variable is not defined
# -------------------------------------------------------------------------

# Intel i386, i486, and Pentium systems
CPUTYPE = 1

# -------------------------------------------------------------------------
# Platform Dependent Binaries Declarations
#
# If you are using the old MIPS compiler then define the following:
#  cc     = cc
#  cvtobj = mip2coff
# -------------------------------------------------------------------------

# binary declarations for use on Intel i386, i486, and Pentium systems
cc     = cl
# for compatibility with older-style makefiles
cvtobj = REM !!! CVTOBJ is no longer necessary - please remove !!!

# binary declarations common to all platforms
link   = link
implib = lib
rc     = rc
cvtres = cvtres
hc     = hc

# -------------------------------------------------------------------------
# Platform Dependent Compile Flags - must be specified after $(cc)
#
# Note: Debug switches are on by default for current release
#
# These switches allow for source level debugging with WinDebug for local
# and global variables.
#
# Both compilers now use the same front end - you must still define either
# _X86_ or _MIPS_.  These have replaced the i386 and MIPS definitions which
# are not ANSI compliant.
#
# Common compiler flags:
#   -c   - compile without linking
#   -W3  - Set warning level to level 3
#   -Zi  - generate debugging information
#   -Od  - disable all optimizations
#   -Ox  - use maximum optimizations
#   -Zd  - generate only public symbols and line numbers for debugging
#
# i386 specific compiler flags:
#   -Gz  - stdcall
#
# -------------------------------------------------------------------------

# declarations common to all compiler options
ccommon = -c -W3

cflags = $(ccommon) -D_X86_=1
scall  = -Gz

# -------------------------------------------------------------------------
# To Not Include Debug Foundation library set AFXDEBUG and AFXLIB to NULL
AFXDEBUG=/D "_DEBUG"
AFXLIB=D
#AFXDEBUG=
#AFXLIB=

!IFDEF NODEBUG
cdebug = -Ox $(AFXDEBUG)
!ELSE
!IFDEF PROFILE
cdebug = -Gh -Zd -Ox $(AFXDEBUG)
!ELSE
!IFDEF TUNE
cdebug = -Gh -Zd -Ox $(AFXDEBUG)
!ELSE
cdebug = -Zi -Od $(AFXDEBUG)
!ENDIF
!ENDIF
!ENDIF

# -------------------------------------------------------------------------
# Target Module & Subsystem Dependent Compile Defined Variables - must be
#   specified after $(cc)
#
# The following table indicates the various acceptable combinations of
# the C Run-Time libraries LIBC, LIBCMT, and CRTDLL respect to the creation
# of a EXE and/or DLL target object.  The appropriate compiler flag macros
# that should be used for each combination are also listed.
#
# Link EXE    Create Exe    Link DLL    Create DLL
#   with        Using         with         Using
# ----------------------------------------------------
#  LIBC        CVARS          None        None      *
#  LIBC        CVARS          LIBC        CVARS
#  LIBC        CVARS          LIBCMT      CVARSMT
#  LIBCMT      CVARSMT        None        None      *
#  LIBCMT      CVARSMT        LIBC        CVARS
#  LIBCMT      CVARSMT        LIBCMT      CVARSMT
#  CRTDLL      CVARSDLL       None        None      *
#  CRTDLL      CVARSDLL       LIBC        CVARS
#  CRTDLL      CVARSDLL       LIBCMT      CVARSMT
#  CRTDLL      CVARSDLL       CRTDLL      CVARSDLL  *
#
# * - Denotes the Recommended Configuration
#
# Note: Any executable which accesses a DLL linked with CRTDLL.LIB must
#       also link with CRTDLL.LIB instead of LIBC.LIB or LIBCMT.LIB.
#       When using DLLs, it is recommended that all of the modules be
#       linked with CRTDLL.LIB.
#
# Note: The macros of the form xDLL are used when linking the object with
#       the DLL version of the C Run-Time (that is, CRTDLL.LIB).  They are
#       not used when the target object is itself a DLL.
#
# -------------------------------------------------------------------------

!IFDEF NO_ANSI
noansi = -DNULL=0
!ENDIF

# for Windows applications that use the C Run-Time libraries
cvars      = -DWIN16 $(noansi)
cvarsmt    = $(cvars)
cvarsdll   = $(cvarsmt) -D_DLL

# for compatibility with older-style makefiles
cvarsdll   = $(cvarsmt) -D_DLL

# for POSIX applications
psxvars    =

# resource compiler
rcvars = -DWIN16 $(noansi)

# -------------------------------------------------------------------------
# Platform Dependent Link Flags - must be specified after $(link)
#
# Note: $(DLLENTRY) should be appended to each -entry: flag on the link
#       line.
#
# Note: When creating a DLL that uses C Run-Time functions it is
#       recommended to include the entry point function of the name DllMain
#       in the DLL's source code.  Also, the MAKEFILE should include the
#       -entry:_DllMainCRTStartup$(DLLENTRY) option for the creation of
#       this DLL.  (The C Run-Time entry point _DllMainCRTStartup in turn
#       calls the DLL defined DllMain entry point.)
#
# -------------------------------------------------------------------------

# declarations common to all linker options
#lcommon =	/B /NOD /F /M:FULL /PACKF
lcommon =	/B /NOD /M:FULL /PACKF

# declarations for use on Intel i386, i486, and Pentium systems
DLLENTRY =
lflags   = $(lcommon) /A:64

# -------------------------------------------------------------------------
# Target Module Dependent Link Debug Flags - must be specified after $(link)
#
# These switches allow the inclusion of the necessary symbolic information
# for source level debugging with WinDebug, profiling and/or performance
# tuning.
#
# Note: Debug switches are on by default.
# -------------------------------------------------------------------------

!IFDEF NODEBUG
ldebug =
!ELSE
!IFDEF PROFILE
ldebug = /CO
!ELSE
!IFDEF TUNE
ldebug = /CO
!ELSE
ldebug = /CO
!ENDIF
!ENDIF
!ENDIF

# for compatibility with older-style makefiles
linkdebug = $(ldebug)

# -------------------------------------------------------------------------
# Subsystem Dependent Link Flags - must be specified after $(link)
#
# These switches allow for source level debugging with WinDebug for local
# and global variables.  They also provide the standard application type and
# entry point declarations.
# -------------------------------------------------------------------------

# for Windows applications that use the C Run-Time libraries
conlflags = $(lflags) 
guilflags = $(lflags)

# for POSIX applications
psxlflags = $(lflags)

# for compatibility with older-style makefiles
conflags  = $(conlflags)
guiflags  = $(guilflags)
psxflags  = $(psxlflags)

# -------------------------------------------------------------------------
# C Run-Time Target Module Dependent Link Libraries
#
# Below is a table which describes which libraries to use depending on the
# target module type, although the table specifically refers to Graphical
# User Interface apps, the exact same dependencies apply to Console apps.
# That is, you could replace all occurrences of 'GUI' with 'CON' in the
# following:
#
# Desired CRT  Libraries   Desired CRT  Libraries
#   Library     to link      Library     to link
#   for EXE     with EXE     for DLL     with DLL
# ----------------------------------------------------
#   LIBC       GUILIBS       None       None       *
#   LIBC       GUILIBS       LIBC       GUILIBS
#   LIBC       GUILIBS       LIBCMT     GUILIBSMT
#   LIBCMT     GUILIBSMT     None       None       *
#   LIBCMT     GUILIBSMT     LIBC       GUILIBS
#   LIBCMT     GUILIBSMT     LIBCMT     GUILIBSMT
#   CRTDLL     GUILIBSDLL    None       None       *
#   CRTDLL     GUILIBSDLL    LIBC       GUILIBS
#   CRTDLL     GUILIBSDLL    LIBCMT     GUILIBSMT
#   CRTDLL     GUILIBSDLL    CRTDLL     GUILIBSDLL *
#
# * - Recommended Configurations.
#
# Note: Any executable which accesses a DLL linked with CRTDLL.LIB must
#       also link with CRTDLL.LIB instead of LIBC.LIB or LIBCMT.LIB.
#
# Note: For POSIX applications, link with $(psxlibs).
#
# -------------------------------------------------------------------------

# optional profiling and tuning libraries
!IFDEF PROFILE
optlibs =
!ELSE
!IFDEF TUNE
optlibs =
!ELSE
optlibs =
!ENDIF
!ENDIF

# basic subsystem specific libraries, less the C Run-Time
baselibs   = libw.lib $(optlibs)
winlibs    = $(baselibs)

# for Windows applications that use the C Run-Time libraries
conlibs    = $(baselibs)
conlibsmt  = $(baselibs)
conlibsdll = $(baselibs)
guilibs    = $(winlibs)
guilibsmt  = $(winlibs)
guilibsdll = $(winlibs)

# for POSIX applications
psxlibs    =
