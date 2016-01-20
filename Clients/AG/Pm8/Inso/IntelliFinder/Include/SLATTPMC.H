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
*  SLATTPMC.H - defines & undefs specific to the Power Macintosh        *
*  environment.                                                         *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:
		
			#defines and #undef of attributes appropriate to the
			Power Macintosh environment (MWERKS CodeWarrior Compiler)

        Cautions:
            This file should be updated if new attributes are added to any
            other system, and should contain all SLA_* defines found in any
            of the other systems.

        Creation Date:
            09-aug-94       TJD

*-----------------------------------------------------------------------*/
#ifndef SLATTPMC_H

#define SLATTPMC_H

/*                              Attributes
*/

/* function declaration services
*/
#define SLA_PROTO_FD        /* Functions use ANSI prototyping? */
#define SLA_UMAIN_FD        /* main() function preceded by _ ? */
#undef  SLA_EXPORT_FD       /* Entry points are exported? */
#undef  SLA_CCPREC_FD       /* Calling convention precedes export keyword? */
#undef  SLA_DLLEXP_FD       /* Uses DLL Export Defines? */
#undef  SLA_LOADDS_FD       /* Entry points need the _loadds keyword? */

/* function calling conventions
*/
#undef  SLA_CPASC_FC        /* Can have either C- or Pascal-c.c.? */
#undef  SLA_UUCPASC_FC      /* Double underscore preceding cdecl & pascal?  */
#undef  SLA_SCHAR_STR_FC    /* Need signed chars in runtime funcs? */
#undef  SLA_CHKSTRTO_FC     /* Check char before passing to to{upper,lower}?*/

/* memory access
*/
#undef  SLA_GLOBEXT_MA      /* no sharable environment */
#undef  SLA_RODATA_MA       /* no read-only declarations */
#undef  SLA_FARCTALLOC_MA   /* don't want FAR allocation at compile time */
#define SLA_DSEQSS_MA       /* DS == SS, so near stack pointers are OK */

/* architecture of machine
*/
#undef  SLA_SEGMENT_AM      /* no FAR/NEAR distinction */
#define SLA_4BYTEINT_AM     /* int is not a 4-byte quantity */
#undef  SLA_VIRTMEM_AM      /* Real-live virtual memory management? */
#define SLA_BIGENDIAN_AM    /* Most significant first, byte ordering? */
#define SLA_NEGBITSHIFT_AM  /* Negative number bit shifting allowed? */

/* memory management system
*/
#undef SLA_HDL_MM          /* Enable handles to data               */
#undef SLA_HDLPTR_MM       /* Handles are pointers to pointers     */

/* data types
*/
#define SLA_SCHAR_DT        /* has a signed char type */
#undef SLA_HUGEPTR_DT       /* Huge Pointers not used */

/* character sets
*/
#undef  SLA_IBMCHAR_CS      /* Use IBM code pages? */

/* file operations
*/
#define SLA_UNXOPENFLG_FO   /* Uses Unix-like file open flags? */
#undef  SLA_BINFILE_FO      /* Distinguishes bet. binary & other files? */
#undef  SLA_TXTFILE_FO      /* Distinguishes bet. text & other files? */
#undef  SLA_TXTSTR_FO       /* Need to specify "t" in file open string? */
#undef  SLA_WOCREAT_FO      /* Write only mode available? */
#undef  SLA_INTPATH_FO      /* Use integer, not char[], for file path? */
#undef  SLA_INT2BFHDL_FO    /* File handles are 2 bytes, not WORD? */

/* library functions
*/
#define SLA_INIMEMGR_LF     /* uses memory manager init functions? */
#undef  SLA_GETENV_LF       /* contains getenv function? */
#define SLA_REMOVE_LF       /* has 'remove' (directory) function? */
#undef  SLA_LOCALALLOC_LF   /* has its own, unique local alloc function? */
#undef  SLA_NMALLOC_LF      /* has nmalloc and nfree?  Mutual. exlus. w/above */
#undef  SLA_FARALLOC_LF     /* use alloc functions starting with 'far'? */
#undef  SLA_LFILE_LF        /* has far (long) Unix-like file functions? */
#undef  SLA_HFILE_LF        /* has huge Unix-like file functions? */
#undef  SLA_DOSFILE_LF      /* has _dos_ file functions? */
#undef  SLA_FARCHAR_LF      /* has far char lib functs (don't use some SL*)?*/
#undef  SLA_NOFCALLOC_LF    /* needs SLfcalloc()? */
#undef  SLA_SLSTRCMP_LF     /* need SLstrcmp and SLstrncmp? */
#undef  SLA_WSPRINTF_LF     /* use wsprintf */

/*                              Include files
*/
#include <stdlib.h>
#include <memory.h>
#include <unix.h>
#include <assert.h>             /* ANSI assert macro                    */

/* Defs for low-level UNIX-style IO funcs
*/
/* The Literal _MAX_LOC_STR is only defined in CW4
   and not defined in CW5.
   Since there is no literal that has the value
   of the compiler version, we are using this literal.
*/
   
#ifndef  _MAX_LOC_STR /* The compiler version is CW5 1.2 */
#define		Copen(M_NAME,M_FLG)     (open(M_NAME, M_FLG))   /* only 2 params */
#else       /* The compiler version is CW4 1.1 */
#define		Copen(M_NAME,M_FLG)     (__open(M_NAME, M_FLG))   /* only 2 params */
#define		close	__close
#define		read	__read
#define		write	__write
#define		lseek	__lseek
#endif      /* _MAX_LOC_STR */

#endif /* SLATTPMC_H */

/* End of file - slattpmc.h   */
