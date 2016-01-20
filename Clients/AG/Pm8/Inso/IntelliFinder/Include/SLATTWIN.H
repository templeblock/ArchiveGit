/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*   SLATTWIN.H - ifdefs & undefs specific to the WINDOWS environment.   *
*                Define attributes for Windows.  Also define macros     *
*                that don't lend themselves well to the attribute       *
*                system. Include slwindef.h to shut off parts of        *
*                WINDOWS.H.                                             *
*-----------------------------------------------------------------------*


	Description:
	    slenvwin.h defines & undefines token strings that describe the
	    attributes of the Windows environment.

	Cautions:
	    This file should be updated if new attributes are added to any
	    other system, and should contain all SLA_* defines found in any
	    of the other systems.

	Creation Date:
	    23-sep-91       JCC

	Change Record:
	    16-aug-92       JCC     File handles are WORDs or INT2Bs

*-----------------------------------------------------------------------*/

#ifndef SLATTWIN_H

#define SLATTWIN_H

/*                              Attributes
*/

/* function declaration services
*/
#define SLA_PROTO_FD        /* Functions use ANSI prototyping? */
#undef  SLA_UMAIN_FD        /* main() function preceded by _ ? */
#define SLA_EXPORT_FD       /* Entry points are exported? */
#undef  SLA_CCPREC_FD       /* Calling convention precedes export keyword? */
#undef  SLA_DLLEXP_FD       /* Uses DLL Export Defines? */
#undef  SLA_LOADDS_FD       /* Entry points need the _loadds keyword? */

/* function calling conventions
*/
#define SLA_CPASC_FC        /* Can have either C- or Pascal-c.c.? */
#define SLA_UUCPASC_FC      /* Double underscore preceding cdecl & pascal?  */
#define SLA_SCHAR_STR_FC    /* Need signed chars in runtime funcs? */
#undef  SLA_CHKSTRTO_FC     /* Check char before passing to to{upper,lower}?*/

/* memory access
*/
#undef  SLA_GLOBEXT_MA      /* doesn't have sharable environment */
#undef  SLA_RODATA_MA       /* doesn't have read-only declarations */
/* far compile-time variable allocation (for, e.g., arrays) is not desired
 * under Windows because it forces placement of the memory area outside of
 * the default data segment.  This creates an additional data segment, which
 * is acceptable, except that the data segment must be fixed (which happens
 * by default in the Large Memory Model).
*/
#undef  SLA_FARCTALLOC_MA   /* don't want FAR allocation at compile time */
/* Cannot assume that DS == SS, so therefore far pointers must be used to
 * reference stack variables, and far functions must be used to access 
 * call-by-reference stack data.  See note at WFAR define in SL.H.
*/
#undef SLA_DSEQSS_MA

/* architecture of machine
*/
#define SLA_SEGMENT_AM      /* FAR/NEAR pointers apply */
#undef  SLA_4BYTEINT_AM     /* int is not a 4-byte quantity */
#define SLA_VIRTMEM_AM      /* Real-live virtual memory management? */
#undef  SLA_BIGENDIAN_AM    /* Most significant first, byte ordering? */
#define SLA_NEGBITSHIFT_AM  /* Negative number bit shifting allowed? */

/* memory management system
*/
#define SLA_HDL_MM          /* Enable handles for data              */
#undef  SLA_HDLPTR_MM       /* Handles are pointers to pointers     */

/* data types
*/
#define SLA_SCHAR_DT        /* has a signed char type                       */
#define SLA_HUGEPTR_DT      /* Huge Pointers used                           */

/* character sets
*/
#undef  SLA_IBMCHAR_CS      /* Use IBM code pages? */

/* file operations
*/
#undef  SLA_UNXOPENFLG_FO   /* Uses Unix-like file open flags? */
#undef  SLA_BINFILE_FO      /* Distinguishes bet. binary & other files? */
#define SLA_TXTFILE_FO      /* Distinguishes bet. text & other files? */
#define SLA_TXTSTR_FO       /* Need to specify "t" in file open string? */
#define SLA_WOCREAT_FO      /* Write only mode available? */
#undef  SLA_INTPATH_FO      /* Use integer, not char[], for file path? */
#undef  SLA_INT2BFHDL_FO    /* File handles are 2 bytes, not WORD? */

/* library functions
*/
#undef  SLA_INIMEMGR_LF     /* uses memory manager init functions? */
#define SLA_GETENV_LF       /* contains getenv function? */
#define SLA_REMOVE_LF       /* has 'remove' (directory) function? */
#define SLA_LOCALALLOC_LF   /* has its own, unique local alloc function? */
#undef  SLA_NMALLOC_LF      /* has nmalloc and nfree?  Mutual. exlus. w/above */
#undef  SLA_FARALLOC_LF     /* use alloc functions starting with 'far'? */
#define SLA_LFILE_LF        /* has far (long) Unix-like file functions? */
#define SLA_HFILE_LF        /* has huge Unix-like file functions? */
#undef  SLA_DOSFILE_LF      /* has _dos_ file functions? */
#define SLA_FARCHAR_LF      /* has far char lib functs (don't use some SL*)?*/
#undef  SLA_NOFCALLOC_LF    /* needs SLfcalloc()? */
#undef  SLA_SLSTRCMP_LF     /* need SLstrcmp and SLstrncmp? */
#define SLA_WSPRINTF_LF     /* use wsprintf */

/*                              Include files
*/
#include <fcntl.h>              /* File Open Mnemonics                  */
#include <share.h>              /* File Sharing                         */
#include <malloc.h>             /* Memory Allocation Functions          */
#include <stdlib.h>             /* Standard 'C' Library Functions       */
#include <io.h>                 /* Operating System I/O Functions       */
#include <dos.h>                /* DOS interface routines               */
#include <assert.h>             /* ANSI assert macro                    */
#include <sys\types.h>          /* Standard System Types                */
#include <sys\stat.h>           /* File Status Symbolic Constants       */

#include "slwindef.h"           /* shut off parts of windows.h          */

/* #ifndef test for something in windows.h before including?
*/
#include <windows.h>            /* Windows main include file            */


/* The Windows 3.0 SDK does not differentiate between handles as well
 * as the 3.1 SDK.  If we are building our products with the 3.0 SDK,
 * define the 3.1 handle types.  3.0 also doesn't have huge file functions
 * (e.g., _hread).
 *
 * The following lines can be removed when we do not support Windows 3.0 SDK.
*/

/* 3.1 WINDOWS.H defines WINVER to be 0x30A, but 3.0 WINDOWS.H does not
 * define WINVER at all.
*/
#if WINVER >= 0x30A

#else

#undef SLA_VIRTMEM_AM
#undef SLA_HFILE_LF

#define HINSTANCE       HANDLE
#define HGLOBAL         HANDLE
#define HLOCAL          HANDLE
#define HACCEL          HANDLE

#define HFILE           int
#define WPARAM          unsigned int
#define LPARAM          LONG
#define UINT            unsigned int
#define HINSTANCE_ERROR ((HINSTANCE)32)

typedef const char FAR* LPCSTR;
typedef LONG            LRESULT;
typedef FARPROC         DLGPROC;
typedef LONG            (_far _pascal * WNDPROC) (HWND, UINT, WPARAM, LPARAM);

#endif /* else of if WINVER >= 0x30A */

/* file macros
*/
#define Ccreat(M_NAME,M_MODE)   (creat(M_NAME,M_MODE))
#define SLoutput(M_NAME)        (fopen(M_NAME,FOPN_CWT))
/* read & write privs but not execute
*/
#define Copen(M_NAME,M_FLG)     (_lopen(M_NAME, M_FLG))



/* Environment compensation
*/
#ifdef SLA_FARSTR_LF
/* Not InfoSoft functions - left unprototyped by environment
*/
CHAR FAR * FAR _CDECL _fstrncpy(CHAR FAR *, const CHAR FAR *, size_t);
VOID FAR * FAR _CDECL _fmemset(VOID FAR *, SWORD, size_t);
VOID FAR * FAR _CDECL _fmemcpy(VOID FAR *, const VOID FAR*, size_t);
#endif  /* SLA_FARSTR_LF */

#endif /* SLATTWIN_H */

/* End of file - slattwin.h   */
