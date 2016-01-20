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
*   SLATTDOS.H - ifdefs & undefs specific to the MSDOS environment.     *
*                Define attributes for DOS.  Also define macros that    *
*                don't lend themselves well to the attribute system.    *
*                                                                       *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:
            slenvdos.h defines & undefines token strings that describe the
            attributes of the MS-DOS environment.

        Cautions:
            This file should be updated if new attributes are added to any
            other system, and should contain all SLA_* defines found in any
            of the other systems.

        Creation Date:
            23-sep-91       JCC

        Change Record:
            16-aug-92       JCC     File handles are WORDs or INT2Bs

*-----------------------------------------------------------------------*/

#ifndef SLATTDOS_H

#define SLATTDOS_H

/*                              Attributes
*/

/* function declaration services
*/
#define SLA_PROTO_FD        /* Functions use ANSI prototyping? */
#undef  SLA_UMAIN_FD        /* main() function preceded by _ ? */
#undef  SLA_EXPORT_FD       /* Entry points are exported? */
#undef  SLA_CCPREC_FD       /* Calling convention precedes export keyword? */
#undef  SLA_DLLEXP_FD       /* Uses DLL Export Defines? */
#undef  SLA_LOADDS_FD       /* Entry points need the _loadds keyword? */

/* function calling conventions
*/
#define SLA_CPASC_FC        /* Can have either C- or Pascal-c.c.? */
#define SLA_UUCPASC_FC      /* Double underscore preceding cdecl & pascal?  */
#undef  SLA_SCHAR_STR_FC    /* Need signed chars in runtime funcs? */
#undef  SLA_CHKSTRTO_FC     /* Check char before passing to to{upper,lower}?*/

/* memory access
*/
#undef  SLA_GLOBEXT_MA      /* doesn't have sharable environment */
#undef  SLA_RODATA_MA       /* doesn't have read-only declarations */
#define SLA_FARCTALLOC_MA   /* want FAR allocation at compile time */
#define SLA_DSEQSS_MA       /* DS == SS, so near stack pointers are OK */

/* architecture of machine
*/
#define SLA_SEGMENT_AM      /* FAR/NEAR pointers apply */
#undef  SLA_4BYTEINT_AM     /* int is not a 4-byte quantity */
#undef  SLA_VIRTMEM_AM      /* Real-live virtual memory management? */
#undef  SLA_BIGENDIAN_AM    /* Most significant first, byte ordering? */
#define SLA_NEGBITSHIFT_AM  /* Negative number bit shifting allowed? */

/* memory management system
*/
#undef  SLA_HDL_MM          /* no handles to data               */
#undef  SLA_HDLPTR_MM       /* Handles are pointers to pointers     */

/* data types
*/
#define SLA_SCHAR_DT        /* has a signed char type */
#define SLA_HUGEPTR_DT      /* Huge Pointers used     */

/* character sets
*/
#define SLA_IBMCHAR_CS      /* Use IBM code pages? */

/* file operations
*/
#define SLA_UNXOPENFLG_FO   /* Uses Unix-like file open flags? */
#define SLA_BINFILE_FO      /* Distinguishes bet. binary & other files? */
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
#undef  SLA_LOCALALLOC_LF   /* has its own, unique local alloc function? */
#define SLA_NMALLOC_LF      /* has nmalloc and nfree?  Mutual. exlus. w/above */
#undef  SLA_FARALLOC_LF     /* use alloc functions starting with 'far'? */
#undef  SLA_LFILE_LF        /* has far (long) Unix-like file functions? */
#undef  SLA_HFILE_LF        /* has huge Unix-like file functions? */
#define SLA_DOSFILE_LF      /* has _dos_ file functions? */
#define SLA_FARCHAR_LF      /* has far char lib functs (don't use some SL*)?*/
#undef  SLA_NOFCALLOC_LF    /* needs SLfcalloc()? */
#undef  SLA_SLSTRCMP_LF     /* need SLstrcmp and SLstrncmp? */
#undef  SLA_WSPRINTF_LF     /* use wsprintf */
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

/* file macros
*/
#define Ccreat(M_NAME,M_MODE)   (creat(M_NAME,M_MODE))
#define SLoutput(M_NAME)        (fopen(M_NAME,FOPN_CWT))
#define Copen(M_NAME,M_FLG)     (open(M_NAME, M_FLG, S_IREAD | S_IWRITE))

#endif /* SLATTDOS_H */

/* End of file - slattdos.h   */
