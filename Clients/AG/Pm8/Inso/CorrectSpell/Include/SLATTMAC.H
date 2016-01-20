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
*  SLATTMAC.H - defines & undefs specific to the Macintosh environment. *
*               Define attributes for Mac & Mac compilers. Also define  *
*               macros that don't lend themselves well to the attribute *
*               system.                                                 *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:
            slattmac.h defines & undefines token strings that describe the
            attributes of the Mac and Mac-compiler environment.

            ThinkC is the default environment.  Other compilers, such as
            MPW, that clash with ThinkC, can resolve differences in 
            SLUNDEF.H.  However, any files to be #included by any Mac 
            compilers should be done here, separated with #ifdefs if
            necessary.

        Cautions:
            This file should be updated if new attributes are added to any
            other system, and should contain all SLA_* defines found in any
            of the other systems.

        Creation Date:
            23-sep-91       JCC

        Change Record:
            19-may-92       JES     Added Errors.h to applec includes.
            16-aug-92       JCC     File handles are WORDs or INT2Bs
            15-apr-93       JCC     Move MPC/THINK_C conflicts to SLUNDEF.H

*-----------------------------------------------------------------------*/

#ifndef SLATTMAC_H

#define SLATTMAC_H

/* These constants are not defined in the Mac standard library
   headers.  They have been defined here using available
   bits in the file open mode param
*/
#define SH_DENYRW	(1<<12)
#define SH_DENYWR	(1<<13)

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
#define SLA_SCHAR_STR_FC    /* Need signed chars in runtime funcs? */
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
#undef  SLA_4BYTEINT_AM     /* int is not a 4-byte quantity */
#undef  SLA_VIRTMEM_AM      /* Real-live virtual memory management? */
#define SLA_BIGENDIAN_AM    /* Most significant first, byte ordering? */
#define SLA_NEGBITSHIFT_AM  /* Negative number bit shifting allowed? */

/* memory management system
*/
#define SLA_HDL_MM          /* Enable handles to data               */
#define SLA_HDLPTR_MM       /* Handles are pointers to pointers     */

/* data types
*/
#define SLA_SCHAR_DT        /* has a signed char type */
#undef  SLA_HUGEPTR_DT      /* Huge Pointers not used */

/* character sets
*/
#undef  SLA_IBMCHAR_CS      /* Use IBM code pages? */

/* file operations
*/
#define SLA_UNXOPENFLG_FO   /* Uses Unix-like file open flags? */
#define SLA_BINFILE_FO      /* Distinguishes bet. binary & other files? */
#define SLA_TXTFILE_FO      /* Distinguishes bet. text & other files? */
#undef  SLA_TXTSTR_FO       /* Need to specify "t" in file open string? */
#undef  SLA_WOCREAT_FO      /* Write only mode available? */
#define SLA_INTPATH_FO      /* Use integer, not char[], for file path? */
#define SLA_INT2BFHDL_FO    /* File handles are 2 bytes, not WORD? */
#define SLA_FILELOCK_FO     /* Files opened for exclusive write need unlock afterwards? */

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
#define SLA_SLSTRCMP_LF     /* need SLstrcmp and SLstrncmp? */
#undef  SLA_WSPRINTF_LF     /* use wsprintf */
#undef  SLA_WDCHAR_LF       /* wide character support */

/*                              Include files
*/

#ifdef   THINK_C  /* Think C include files */

#include <fcntl.h>              /* File Open Mnemonics                  */
#include <stdlib.h>             /* High Level Memory Functions          */
#include <unix.h>               /* Unix Definitions                     */
#include <assert.h>             /* ANSI assert macro                    */

#endif            /* End of Think C section */

#ifdef   applec   /* Apple MPW C include files */

#include <Devices.h>
#include <Errors.h>
#include <FCntl.h>
#include <Files.h>
#include <IOCtl.h>
#include <Memory.h>
#include <Osutils.h>
#include <StandardFile.h>
#include <StdIO.h>
#include <StdLib.h>
#include <String.h>
#include <Traps.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <Assert.h>             /* ANSI assert macro                    */

#endif            /* End of Apple MPW C section */

#ifdef __MWERKS__ /* Metrowerks C include files */

#include <assert.h>             /* ANSI assert macro                    */
#include <Devices.h>
#include <Errors.h>
#include <Files.h>
#include <Memory.h>
#include <Osutils.h>
#include <StandardFile.h>
#include <stdio.h>              /* File I/O                             */
#include <stdlib.h>             /* High Level Memory Functions          */
#include <String.h>
#include <Traps.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <unix.h>               /* Unix Definitions                     */

#endif            /* End of Metrowerks C section */

/* file macros
*/
#define Ccreat(M_NAME,M_MODE)   (creat(M_NAME,M_MODE))
#define SLoutput(M_NAME)        (fopen(M_NAME,FOPN_CWT))
#define Copen(M_NAME,M_FLG)     (open(M_NAME, M_FLG))   /* only 2 params */

#endif /* SLATTMAC_H */

/* End of file - slattmac.h   */
