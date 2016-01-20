/*.T(32)  STDTYPES    Standard Types for C Programs   */

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   Copyright (c) 1993-1995, Pegasus Imaging Corp.  All rights reserved.บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if defined(USEPICCLIB)
#include "picclib.h"
#endif

#if !defined(_STDTYPES)
#define _STDTYPES

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   The following types are independent of the model:                   บ
//  ณ                                                                       บ
//  ณ   CHAR  / BYTE        - signed / unsigned 8-bit value                 บ
//  ณ   SHORT / WORD        - signed / unsigned 16-bit value                บ
//  ณ   LONG  / DWORD       - signed / unsigned 32-bit value                บ
//  ณ                                                                       บ
//  ณ   The following type is dependent on the model:                       บ
//  ณ                                                                       บ
//  ณ   INT   / UINT        - signed / unsigned value (16 or 32 bits)       บ
//  ณ   BOOL                - signed int (FALSE = 0, TRUE = 1)              บ
//  ณ                                                                       บ
//  ณ   The enumeration type should NOT be used since some compilers requireบ
//  ณ   that this be an int, while others allow it to be 1-, 2-, or 4-bytes บ
//  ณ   depending on the values used by the enumeration elements.           บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if defined(WINDOWS)
    #pragma message ("WINDOWS defined")
		#include <windows.h>
    #define EXPORTWINAPI WINAPI
#if defined(__FLAT__)
    #define DLLEXPORTWINAPI WINAPI
    #define PICHUGE
    #define PICFAR
#else
    #define DLLEXPORTWINAPI WINAPI __loadds
    #define PICHUGE __huge
    #define PICFAR  __far
#endif
#else
#ifdef OS2
    #define INCL_DOS
    #define INCL_DOSPROCESS
    #include <os2.h>
		#if defined(__IBMC__)
        #define DLLEXPORTWINAPI _System
				#define EXPORTWINAPI    _System
    #else
        #pragma message ("OS2 defined")
        #define DLLEXPORTWINAPI _syscall
        #define EXPORTWINAPI    _syscall
    #endif
#else
    #pragma message ("WINDOWS NOT defined")
    typedef signed   int    BOOL;
    typedef unsigned char   BYTE;
    typedef signed long     LONG;
    typedef unsigned int    UINT;
    #define DLLEXPORTWINAPI    
    #define EXPORTWINAPI
#endif
    typedef unsigned short  WORD;
    typedef unsigned long   DWORD;
		typedef signed   long   LPARAM;
#if defined(MACINTOSH)
		#define __cdecl
#else
		#define FALSE           0
		#define TRUE            1
#endif
		#define PICHUGE
		#define PICFAR
#endif

typedef signed char  SBYTE;
#define CHAR    char
#define INT     int
#define SHORT   short
#define PRIVATE static
#define PUBLIC

#endif
