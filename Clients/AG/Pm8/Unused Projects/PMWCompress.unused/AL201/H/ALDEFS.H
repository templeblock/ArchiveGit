/*
 * ALDEFS.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This is the setup file for ArchiveLib.  Everyone else has to include
 *  it, because all of the macros defined in here help to control
 *  how the library operates.  Normally you will never have to include
 *  this header file, because it is included by ARCLIB.H, and consequently,
 *  everyone else.
 *
 * MACROS
 *
 * These are all calculated automatically for supported compilers.
 *
 *  AL_WINDOWS         : This library can use some or all of the Windows API
 *  AL_FLAT_MODEL      : This library is a 32 bit library
 *  AL_LARGE_DATA      : Data uses 16:16 data pointer
 *  AL_SMALL_CODE      : Code uses 16 bit function pointers
 *  AL_OS2             : Some day.
 *  AL_UNIX            : Those days are gone!
 *  AL_KR              : Non-ANSI C, like the portable c compiler
 *
 *  AL_BUILDING_DLL    : Building the Windows DLL
 *  AL_USING_DLL       : Change all pointer args to FAR if set
 *  AL_USING_CRT_DLL   :
 *  AL_VB              : Building a Win16 DLL appropriate for VB 3.0
 *
 *  AL_SYMANTEC        : Compiler selection
 *  AL_BORLAND         : Compiler selection
 *  AL_MICROSOFT       : Compiler selection
 *  AL_WATCOM          : Compiler selection
 *  AL_SUN4            : No more!
 *
 *  AL_UNUSED_PARAMETER : A macro that lets me make warning messages
 *                        about unused parameters in C programs go away.
 *
 *  Based on those settings, we can set this:
 *
 *  AL_WIN32           : This is NT or Win32s
 *
 *                     AL_WINDOWS         AL_FLAT_MODEL
 * Vanilla DOS                NO               NO
 * Borland DPMI16             NO               NO
 * Borland DPMI32             NO               YES
 * Symantec DOSX              NO               YES
 * Win32s                     YES              YES
 * Vanilla Windows            YES              NO
 *
 *  Finally, we use those to create these things used in prototypes:
 *
 *  AL_CLASS_TYPE     : Sometimes "export", for classes in DLLs
 *  AL_PROTO          : Sometimes "export", for functions in DLLs
 *  AL_DLL_FAR        : Sometimes "far", for arguments being passed to DLLs
 *  AL_FUNCTION       : Vanilla exported functions are "_export pascal"
 *  AL_CFUNCTION      : Variable argument export functions are "_export cdecl"
 *  AL_EXPORT         : Used to define export functions under Windows, only
 *                      for examples and so on.  _export for Win16, blank for
 *                      Win32.
 *
 * PROTOTYPES:
 *
 *
 * ENUMERATED TYPES:
 *
 *  ALMonitorType                  : Used to indicate whether a monitor
 *                                   is watching objects or an entire job.
 *
 *  ALWindowsMessageType           : To indicate whether an object of
 *                                   class ALMonitor is supposed to be
 *                                   sending total byte counts or
 *                                   percentage complete ratios.
 *
 *  ALErrors                       : The global list of errors.
 *
 *  ALCase                         : Used by ALName to indicate case
 *                                   sensitivity/handling
 *
 *  ALGreenleafCompressionLevels   : The five levels used by ALGreenleafEngine.
 *
 *  ALTraverseSetting              : Used to indicate whether class
 *                                   ALWildCardExpander will traverse
 *                                   subdirectories when searching.
 *
 *  ALStorageType                  : The type of a storage engine.
 *
 *  ALCompressionType              : The type of a compression engine.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A   : First release
 *
 *  July 7, 1994  1.0B   : Added an #ifdef for Watcom 9.5.  It couldn't take
 *                         a pragma that 10.0 was happy with.
 *
 *  July 11, 1994 1.0B   : Added the AL_EXPORT constant.  This is mostly used
 *                         for my examples.
 *
 *  August 10, 1994 1.0B : Added AL_KR, AL_VB, AL_UNIX, AL_SUN4, and AL_GCC.
 *
 *  January 12, 1995  1.01B : Added AL_UNUSED_PARAMETR.
 *
 *  February 14, 1996  2.0A : New release
 *
 *  March 21, 1996     2.01A : Added AL_MIXED_UP to better support the
 *                             Windows 95 file system.
 */

#ifndef _ALDEFS_H
#define _ALDEFS_H
/*
 *  The next long set of definitions and tests are all here simply to
 *  determine which compiler we are using, and what sort of target
 *  configuration we are trying to build/use.  It is an awful mess,
 *  sorry.
 */

#if defined(__BORLANDC__) || defined(__TURBOC__)
#if sizeof( int ) == 4
#  define AL_FLAT_MODEL
#endif
#if defined( _RTLDLL )
#  define AL_USING_CRT_DLL
#endif
#  if defined( __OS2__ )
#    define AL_OS2
#  endif
#  if defined( __WIN32__ ) /* && !defined( __DPMI32__ ) */
#    define AL_WINDOWS
#  endif
#  if !defined( __BORLANDC__ )
#      define AL_BORLAND __TURBOC__
#  else
#      define AL_BORLAND __BORLANDC__
#  endif
#  if sizeof( void * ) == 4
#    define AL_LARGE_DATA
#  endif
typedef void (*_alfnptr)(void);
#  if sizeof( _alfnptr ) == 2
#    define AL_SMALL_CODE
#  endif
#  if defined( _Windows ) /* && !defined( __DPMI16__ ) && !defined( __DPMI32__ ) */
#    define AL_WINDOWS
#    ifdef __DLL__
#      define AL_BUILDING_DLL
#    endif
#  elif defined( __OS2__ )
#    ifdef __DLL__
#      define AL_BUILDING_DLL
#    endif
#  endif
#elif defined( __SC__ )
#  define AL_SYMANTEC _MSC_VER
#  if defined( __NT__ )
#    define AL_WINDOWS
#    define AL_FLAT_MODEL
#  endif
#  if defined( _M_I86HM ) || defined( _M_I86CM ) || defined( _M_I86LM )
#    define AL_LARGE_DATA
#  endif
#  if defined( _M_I86TM ) || defined( _M_I86SM ) || defined( _M_I86CM )
#    define AL_SMALL_CODE
#  endif

/*
 * The _WINDOWS and _WINDLL macros are only documented in LIBRARY.TXT
 */
#  ifdef _WINDOWS
#    define AL_WINDOWS
#    ifdef _WINDLL
#      define AL_BUILDING_DLL
#      ifndef M_I86LM
         #error All DLLs must be built using Large Model!
#      endif
#    endif
#  endif
#elif defined( _MSC_VER ) && defined( _MSDOS )
#  define AL_MICROSOFT _MSC_VER
#if defined( _MT ) && defined( _DLL )
#  define AL_USING_CRT_DLL
#endif
#  if defined( _M_I86HM ) || defined( _M_I86CM ) || defined( _M_I86LM )
#    define AL_LARGE_DATA
#  endif
#  if defined( _M_I86TM ) || defined( _M_I86SM ) || defined( _M_I86CM )
#    define AL_SMALL_CODE
#  endif
#  if ( AL_MICROSOFT >= 800 )
/*
 * I really don't want MSC to tell me when it is using a precompiled
 * header file.  What is really dumb is that I probably do want it
 * to tell me when it is creating one, but they generate the same
 * warning!
 */
#    pragma warning( disable : 4699 )
/*
 * This error occurs if you have inline functions in a header file and
 * they don't get used in a particular file.  Bogus.
 */
#    pragma warning( disable : 4505 )
/*
 * This warning occurs if you are using assert() macros with NDEBUG and /Ox
 * it is bogus
 */
#    pragma warning( disable : 4705 )
/*
 * This gives a warning for cout << setw( x )
 */
#    pragma warning( disable : 4270 )
/*
 * This is informational, it tells me when a function has been
 * chosen for inlining.
 */
#    pragma warning( disable : 4711 )
/*
 * This is informational, it tells me when a function has been
 * rejected for inlining.  The funny part is that it gives
 * me this message even if I don't select inlining for that
 * particular function???
 */
#    pragma warning( disable : 4710 )
#  else  /*#if ( AL_MICROSOFT >= 800 ) */
/*
 * Microsoft C 7.0 has a major linker problems if a symbol exceeds
 * 64 characters.  Unfortunately, with full decoration, we have
 * a couple of functions that hit that wall.  So I have to redefine
 * a couple of innocuous class names. I am trying to use similar
 * names so that if you hit them in the debugger you will be able
 * to understand what they mean.
 *
 *  #if defined( _WINDLL )
 *     #define ALWindowsMessage ALWinMsg_
 *      #define ALCompressionEngine ALEngine_
 *  #endif  NOTE: Fixing problem with /H64 in BUILD.INI! */
#  endif /*#if ( AL_MICROSOFT >= 800 ) ... #else */
#  ifdef _WINDOWS
#    define AL_WINDOWS
#    ifdef _WINDLL
#      define AL_BUILDING_DLL
#      ifndef M_I86LM
         #error All DLLs must be built using Large Model!
#      endif
#    endif
#  endif
#elif defined( _MSC_VER ) && !defined( _MSDOS ) /* Looks like NT */
#  define AL_MICROSOFT _MSC_VER
#  define AL_FLAT_MODEL
#  define AL_LARGE_DATA
#  define AL_WIN32
#  define AL_WINDOWS
/*
 * See the previous MSC standard section for the meanings of these pragmas.
 */
#  pragma warning( disable : 4699 )
#  pragma warning( disable : 4505 )
#  pragma warning( disable : 4705 )
#  pragma warning( disable : 4270 )
#  pragma warning( disable : 4711 )
#  pragma warning( disable : 4710 )
/*
 * We get this error in Microsoft's header files!
 */
# pragma warning( disable : 4201 )
#  ifdef _WINDOWS
#    define AL_WINDOWS
#    ifdef _WINDLL
#      define AL_BUILDING_DLL
#    endif
#  endif
#elif defined( __WATCOMC__ ) && defined( __386__ )
#  ifdef __WINDOWS__
#    define AL_WINDOWS
#  endif
#  define AL_FLAT_MODEL
#  ifdef __SW_ZU
#    define AL_BUILDING_DLL
#  endif
#  if __WATCOMC__ > 950
#    pragma warning 690 9 /*  Warning for AL_ASSERT() at /w3 */
#    pragma warning 549 9 /*  Warning for sizeof() on class */
#  endif
#elif defined( __WATCOMC__ ) && !defined( __386__ )
#  define AL_WATCOM
#  ifdef __WINDOWS__
#    define AL_WINDOWS
#    ifdef __SW_ZU
#      define AL_BUILDING_DLL
#      ifndef M_I86LM
         #error All DLLs must be built using Large Model!
#      endif
#    endif
#  endif
#  if defined( _M_I86HM ) || defined( _M_I86CM ) || defined( _M_I86LM )
#    define AL_LARGE_DATA
#  endif
#  if defined( _M_I86TM ) || defined( _M_I86SM ) || defined( _M_I86CM )
#    define AL_SMALL_CODE
#  endif
#  pragma warning 549 9 /*  Warning for sizeof() on class */
#elif defined( __IBMC__ ) || defined( __IBMCPP__ )
#  define AL_IBM
#  define AL_OS2
#  define AL_FLAT_MODEL
#elif defined( AL_SUN4 )
#  define AL_UNIX
#  if !defined( AL_GCC ) && !defined( __cplusplus )
#    define AL_KR
#  endif
#else
  #error "Unknown compiler!"
#endif

#if defined( AL_OS2 )
#define INCL_WIN
#include <os2.h>
#endif

#  ifdef AL_FLAT_MODEL
#    define AL_HUGE
#  else
#    define AL_HUGE _huge
#  endif

#if defined( AL_WINDOWS )
#  if defined( AL_VB32 )
#    define INC_OLE2
#  endif
#  if !defined( STRICT )
#    define STRICT
#  endif
#  include <windows.h>
#endif

/*
 * I modified some assumptions here.  I now assume that all 32 bit DOS
 * Extenders are going to use the Win32 memory API.
 */
#if defined( AL_WINDOWS ) && defined( AL_FLAT_MODEL )
#  define AL_WIN32
#endif

#if defined( AL_BUILDING_DLL )
#if defined( AL_FLAT_MODEL ) && defined( AL_MICROSOFT )
#  if defined( AL_VB32 )
#    define AL_CLASS_TYPE
#    define AL_LINKAGE
#    define AL_PROTO
#    define AL_DLL_FAR
#    define AL_FUNCTION       _stdcall
#    define AL_CFUNCTION      _stdcall
#  else /* if defined( AL_VB32 ) */
#    define AL_CLASS_TYPE     __declspec( dllexport )
#    define AL_LINKAGE        __declspec( dllexport )
#    define AL_PROTO
#    define AL_DLL_FAR
#    define AL_FUNCTION
#    define AL_CFUNCTION
#  endif /* if defined( AL_VB32 ) */
#  elif defined( AL_IBM )
#    define AL_CLASS_TYPE     _Export
#    define AL_LINKAGE
#    define AL_PROTO          _Export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _Export /* should I use _Pascal? */
#    define AL_CFUNCTION      _Export __cdecl
#  elif defined( AL_FLAT_MODEL )
#    define AL_CLASS_TYPE     _export
#    define AL_LINKAGE
#    define AL_PROTO          _export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _export pascal
#    define AL_CFUNCTION      _export cdecl
#  elif defined( AL_WATCOM )
#    define AL_CLASS_TYPE
#    define AL_LINKAGE
#    define AL_PROTO          _export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _export _far pascal
#    define AL_CFUNCTION      _export _far cdecl
#  else
#    define AL_CLASS_TYPE     _export
#    define AL_LINKAGE
#    define AL_PROTO          _far
#    define AL_DLL_FAR        _far
#    define AL_FUNCTION       _export _far pascal
#    define AL_CFUNCTION      _export _far cdecl
#  endif
#elif defined( AL_USING_DLL )
#  if defined( AL_FLAT_MODEL ) && defined( AL_BORLAND ) && !defined( AL_OS2 )
#    define AL_CLASS_TYPE     _import
#    define AL_LINKAGE
#    define AL_PROTO          _import
#    define AL_DLL_FAR
#    define AL_FUNCTION       _import pascal
#    define AL_CFUNCTION      _import cdecl
#  elif defined( AL_FLAT_MODEL ) && defined( AL_BORLAND ) && defined( AL_OS2 )
#    define AL_CLASS_TYPE     _export
#    define AL_LINKAGE
#    define AL_PROTO          _export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _export pascal
#    define AL_CFUNCTION      _export cdecl
#  elif defined( AL_IBM )
#    define AL_CLASS_TYPE     _Export
#    define AL_LINKAGE
#    define AL_PROTO          _Export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _Export /* should I use _Pascal? */
#    define AL_CFUNCTION      _Export __cdecl
#  elif defined( AL_FLAT_MODEL ) && defined( AL_MICROSOFT )
#    define AL_CLASS_TYPE
#    define AL_LINKAGE        __declspec(dllimport)
#    define AL_PROTO
#    define AL_DLL_FAR
#    define AL_FUNCTION
#    define AL_CFUNCTION
#  elif defined( AL_FLAT_MODEL ) && defined( AL_SYMANTEC )
#    define AL_CLASS_TYPE     _export
#    define AL_LINKAGE
#    define AL_PROTO          _export
#    define AL_DLL_FAR
#    define AL_FUNCTION       _export pascal
#    define AL_CFUNCTION      _export cdecl
#  elif defined( AL_WATCOM )
#    define AL_CLASS_TYPE
#    define AL_LINKAGE
#    define AL_PROTO
#    define AL_DLL_FAR
#    define AL_FUNCTION       _far _pascal
#    define AL_CFUNCTION      _far cdecl
#  else
#    define AL_LINKAGE
#    define AL_PROTO          _far
#    define AL_DLL_FAR        _far
#    ifdef AL_BORLAND
#      if ( AL_BORLAND >= 0x450 )
#        define AL_CLASS_TYPE _import
#      else
#        define AL_CLASS_TYPE   _huge
#      endif
#    else
#      define AL_CLASS_TYPE   _export
#    endif
#    define AL_FUNCTION       _export _far _pascal
#    define AL_CFUNCTION      _export _far cdecl
#  endif
#else
#  define AL_PROTO
#  define AL_CLASS_TYPE
#  define AL_LINKAGE
#  define AL_DLL_FAR
#  define AL_FUNCTION
#  define AL_CFUNCTION
#endif

/*
 * I was trying to build a DLL-enabled small
 * model library for Borland.  Couple of problems.
 * First, some intrinsic fn defs don't match header
 * file defs.  For ex. strcpy() takes far args when
 * using the RTL DLL, but the intrinsic doesn't.
 * Second, a couple of functions have different
 * definitions depening on which include file you
 * select.  I think this is bad, don't you?
 * Anwyay, for now I gave up.
 */

#if defined( AL_WINDOWS ) && !defined( AL_WIN32 )
#define AL_EXPORT _export
#else
#define AL_EXPORT
#endif

#if defined( AL_MICROSOFT )
# define AL_UNUSED_PARAMETER( a ) ( a = a ) /* Tag internal macro */
#else
# define AL_UNUSED_PARAMETER( a ) (void) a
#endif

#if defined( AL_IBM ) && ( defined( AL_BUILDING_DLL ) || defined( AL_USING_DLL ) )
#  define AL_INLINE_PROTO
#else
#  define AL_INLINE_PROTO AL_PROTO
#endif

/*
 * This is used in example programs only.
 */
#if defined( AL_BORLAND ) && ( AL_BORLAND >= 0x500 )
#  define AL_WIN_MAIN_FAR FAR
#else
#  define AL_WIN_MAIN_FAR
#endif

/*
 * All objects of type ALMonitor are set up to monitor jobs or objects.
 * This enum is passed to the constructor to select which one is desired.
 */
enum ALMonitorType {  /* Tag public type */
    AL_MONITOR_OBJECTS,
    AL_MONITOR_JOB
};

/*
 * An ALMonitor object can either send out byte counts or percentage
 * complete ratios.  When constructing the object, this enum indicates
 * which strategy is going to be used.
 */
enum ALWindowsMessageType {  /* Tag public type */
    AL_SEND_BYTE_COUNT,
    AL_SEND_RATIO
};

/*
 * Global enumerated error codes
 */
enum ALErrors {  /* Tag public type */
        AL_CANT_OPEN_BUFFER = -1200,
        AL_CANT_ALLOCATE_MEMORY,
        AL_CANT_CREATE_ENGINE,
        AL_CANT_CREATE_STORAGE_OBJECT,
        AL_RENAME_ERROR,
        AL_CANT_OPEN_FILE,
        AL_SEEK_ERROR,
        AL_READ_ERROR,
        AL_WRITE_ERROR,
        AL_DELETE_ERROR,
        AL_ILLEGAL_PARAMETER,
        AL_INTERNAL_ERROR,
        AL_USER_ABORT,
        AL_SERVER_NOT_PRESENT,
        AL_COMPRESSION_TYPE_MISMATCH,
        AL_NEED_LENGTH,
        AL_CRC_ERROR,
        AL_COMPARE_ERROR,
        AL_UNKNOWN_COMPRESSION_TYPE,
        AL_UNKNOWN_STORAGE_OBJECT,
        AL_INVALID_ARCHIVE,
        AL_LOGIC_ERROR,
        AL_BACKUP_FAILURE,
        AL_GETSEL_ERROR,
        AL_DUPLICATE_ENTRY,
        AL_END_OF_FILE = -1,
        AL_SUCCESS = 0
};

/*
 * Enum used by ALName guys.  Names can either be forced to upper
 * or lower case, or support normal mixed case representations.  Objects
 * like MS-DOS file names should be forced to upper or lower, since the
 * O/S keeps the names as case insensitive.
 */

enum ALCase {  /* Tag public type */
    AL_UPPER,
    AL_LOWER,
    AL_MIXED,
    AL_MIXED_UP    /* This is for Windows 95.  It supports mixed  */
                   /* case names, but does not allow two files to */
                   /* have names that differ only in case!        */
};

/*
 * Compressing levels used by the Greenleaf Engine.  In the archive, this
 * ratio is stored in the engine private data.
 */
enum ALGreenleafCompressionLevels {  /* Tag public type */
    AL_GREENLEAF_COPY    = -1,
    AL_GREENLEAF_LEVEL_0 = 0,
    AL_GREENLEAF_LEVEL_1,
    AL_GREENLEAF_LEVEL_2,
    AL_GREENLEAF_LEVEL_3,
    AL_GREENLEAF_LEVEL_4
};

#define AL_DEFAULT (-2) /* Used by C to define default args */

/*
 * Used when constructing ALWildCardExpander objects.  It decides whether
 * the expander will traverse the entire subdirectory tree, or just stay
 * on the current level.
 */

enum ALTraverseSetting {  /* Tag public type */
    AL_TRAVERSE = 1,
    AL_DONT_TRAVERSE = 0
};

/*
 * The type of storage object.  This is stored in the base class, ALStorage,
 * and is initialized in the constructor.
 */
enum ALStorageType {  /* Tag public type */
    AL_UNDEFINED     = -2,
    AL_STORAGE_DEFAULT = -1,
    AL_MEMORY_OBJECT = 0,
    AL_FILE_OBJECT   = 1 };

/*
 * The type of compression engine.  This is stored in the base class,
 * ALEngine, and is initialized in the constructor.
 */
enum ALCompressionType {  /* Tag public type */
    AL_COMPRESSION_DEFAULT = -1,
    AL_COMPRESSION_COPY = 0,
    AL_COMPRESSION_GREENLEAF = 1,
    AL_COMPRESSION_DEFLATE_COPY = 100,
    AL_COMPRESSION_DEFLATE_SHRUNK = 101,
    AL_COMPRESSION_DEFLATE_REDUCE_1 = 102,
    AL_COMPRESSION_DEFLATE_REDUCE_2 = 103,
    AL_COMPRESSION_DEFLATE_REDUCE_3 = 104,
    AL_COMPRESSION_DEFLATE_REDUCE_4 = 105,
    AL_COMPRESSION_DEFLATE_IMPLODE  = 106,
    AL_COMPRESSION_DEFLATE = 108
};

#if !defined( __cpluscplus )

#define DECLARE_AL_HANDLE( x )                             \
           struct x##_bogus_struct {                       \
               int x##_bogus_unused_member;                \
           };                                              \
           typedef struct x##_bogus_struct AL_DLL_FAR * x

DECLARE_AL_HANDLE( hALCompressed );
DECLARE_AL_HANDLE( hALMonitor );
DECLARE_AL_HANDLE( hALArchive );
DECLARE_AL_HANDLE( hALDecompressor );
DECLARE_AL_HANDLE( hALCompressor );
DECLARE_AL_HANDLE( hALStorage );
DECLARE_AL_HANDLE( hALEntryList );
DECLARE_AL_HANDLE( hALEntry );
DECLARE_AL_HANDLE( hALExpander );      /* ALWildCardExpander */
DECLARE_AL_HANDLE( hALToolKit );

#endif  /*  #if !defined( __cpluscplus ) ... */

#endif
