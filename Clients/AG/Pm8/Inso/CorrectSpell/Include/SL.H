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
*       SL.H - Standard Library typedefs & defines                      *
*                                                                       *
*-----------------------------------------------------------------------*


    Description:
        This is the base header for the Standard Library, which contains 
        all typedefs & defines to be used for all levels of 'C' code, in
        the standard library and in a particular product's files.
        This header will include all of the header files for the 
        compilation platform and define common token strings 
        according to that platform.

        Before this header file is included, the platform name (specific 
        operating system or compiler to which the code is targetted) must
        be #defined.  Most compilers will define a symbolic constant that
        can be used as a platform name.  This file switches on platform
        names to define attributes of the platform; subsequent values of
        typdefs & defines switch on the attributes.
        
        Currently, the following platform names are supported:

        MSDOS      - Defined by Microsoft (R) C compiler, -D switch w/Borland
        VAX        - Defined by the VAX/VMS C compiler
        UNIX       - Supplied with compiler command line -D option
        MAC        - Defined in SLpredef.h for the Think C
                        compiler and Apple MPW C compiler
        WINDOWS    - Supplied with compiler command line /D option

        NOTE: MSDOS will be undefined in WINDOWS after SL.H is included.
        Internally, SL considers MSDOS and WINDOWS separate environments 
        & will undefine MSDOS if WINDOWS is defined.  If you need MSDOS
        defined in your product even under WINDOWS, #define MSDOS at the
        bottom of this file.

        If a different operating system or compiler is being used, a
        platform attributes header file will need to be created to 
        support it.  Define a name for the new environment and its
        attributes file (SLATT???.H) and copy an attributes file from 
        an existing platform.  Define/undefine attributes as befit its 
        qualities.

        If existing attributes are not sufficient to distinguish the new
        environment, new attributes will have to be defined.  Determine 
        the significant new attributes and create appropriately named 
        attribute defines.  Place in all platforms' attributes sections.

        The basic layout of this file is:
        1. include system-independent (c-standard) header files
        2. undefine anything we duplicate that was defined in 1. or 
            on compile command line or by compiler
        3. define attribute-free token strings (same across all 
            platforms)
        4. switch on which compiler, define/undefine each attribute 
            in set and include auxiliary header files
        5. undefine then redefine attribute-free token strings 
            (possibly have been defined in any platform; we want to 
            fix their definition)
        6. for all token strings that vary by environment, by groups
            of related token strings, switch on attribute and define 
            attribute-dependent token strings

    Cautions:
        Any attributes and only those attributes that vary between any
        two or more environments should comprise the set of defined
        attributes.

        Includes a header file, slpredef.h, which contains #ifdef/#undef
        statements to resolve conflicts arising before or within the 
        platform attributes section.  MSDOS and WINDOWS, for instance, are 
        treated as separate environments but in our case at present, 
        both are defined upon entrance to this file.

        This file also includes a header file, slundef.h, which contains
        #ifdef/#undef statements for all defines used in this file, save 
        the platform (development environment) name.  This prevents
        redefinition warnings.

    Creation Date:
        18-jul-89        


*-----------------------------------------------------------------------*/

#ifndef SL_H

#define SL_H

/*                          Common to all systems                           *
 * The following #includes are independent of system attributes             *
*/

/*                  Includes of Standard Library Files                      */

#include <stdio.h>              /* Standard 'C' I/O Functions           */
#include <ctype.h>              /* Character Types and Macros           */
#include <string.h>             /* String Functions                     */

#include "slpredef.h"           /* undefine pre-attrib section defines  */

/*                          Common to all systems                           *
 * The following #defines are independent of system attributes; i.e., noone *
 * will redefine them.                                                      *
*/

/*      Defines for the Extended Character Set(s)                       */

/* Convert from the MCS to the native character set on a platform.
*/
#define DEF_TO_NAT      DEC_TO_PC

/* Convert from the native character set on a platform to the MCS or default 
 * character set.
*/
#define NAT_TO_DEF      PC_TO_DEC

/* The two conversion direction defines above are preferred since 'PC' and 
 * 'DEC' are not accurate if you're not using DEC MCS or on a PC.  These two
 * are kept for product backward compatibility.
*/
#define DEC_TO_PC       0       /* Convert DEC to PC                    */
#define PC_TO_DEC       1       /* Convert PC to DEC                    */

/*      Scalar Defines                                                  */

#define TRUE            1       /* True condition                       */
#define FALSE           0       /* False condition                      */

#define NEGONE          (-1)    /* Negative one                         */
#define ZERO            0       /* Zero                                 */
#define PLUSONE         (1)     /* Positive one                         */

/*      Exit Codes (assign to types RETCODE)                            */

#define OKRET           0       /* Successful return value              */
#define WARNRET         4       /* Successful return value, w/ Warning. */
#define ERRET           8       /* Unsuccessful return value            */
#define FATALRET        16      /* Fatal return value                   */

/*       Modes of operation for calls to SLSeek()                       */

#define SEEK_BOF        0       /* Seek from Beginning Of File          */
#define SEEK_COF        1       /* Seek from Current position Of File   */
#define SEEK_EOF        2       /* Seek from End Of File                */

/*      Mode Bits for Generic Display Functions                         */

#define NORM_SCREEN     001     /* Normal output to screen              */
#define NORM_LOGFILE    002     /* Normal output to disk                */

/*  #defines for company storage standards                              */

/*      Maximum string length for path + filename                       */
#define SLPATHMAX       260

/*      Maximum string length for a filename                            */
#define SLFNAMEMAX      255

/*      Maximum length of a word array                                  */
#define SLWORDMAX       64

/*      Number of INT4Bs per map                                        */
#define SLMAP_PARTS     2

/*  #defines for company product functionality codes                    */

/*      Cap code defines                                                */

#define SLLOWCAP        0       /* All lower case letters                   */
#define SLINITCAP       1       /* Initial upper case letter                */
#define SLALLCAP        2       /* All upper case letters                   */
#define SLMAPCAP        3       /* Capitalization map is used               */
#define SLMUSTLOWCAP    4       /* Mustn't be INITCAP unless starts sentence*/
#define SLNOCAP         5       /* Capitalization not applicable            */

/*      Return codes from SLlisten()    */

#define SLNOTLISTEN     0       /* Non-list enumerators                 */
#define SLLISTEN        1       /* List enumerators                     */
#define SLPOSLISTEN     2       /* Possible list enumerators            */


/*                            Characters defines
*/

/*      C language characters with special meaning                      */

#define NEWLINE         '\n'            /* Newline; move to next line   */
#define EOS             '\0'            /* end of string                */


/*                            Variable Typedefs                         */

/*      Rename the ANSI C types to insure portability across platforms  */

typedef unsigned short int    UINT2B;   /* 2 byte unsigned int          */
typedef unsigned short int   *PUINT2B;  /* 2 byte unsgd int ptr         */

/* These types are already defined in the IBM Set++ compiler for OS/2   */
#ifndef __IBMC__
typedef unsigned char         UCHAR;    /* unsigned character           */
typedef unsigned char        *PUCHAR;   /* unsigned char ptr            */
typedef char                 *PCHAR;    /* character ptr                */
#endif /* __IBMC__ */

typedef char            CHAR;           /* plain character              */
typedef short int       INT2B;          /* 2 byte integer               */
typedef short int     * PINT2B;         /* 2 byte integer ptr           */
typedef char            FLAG;           /* flags                        */
typedef char          * PFLAG;          /* pointer to flags             */
typedef int             SWORD;          /* signed word on machine       */
typedef int           * PSWORD;         /* ptr to signed word           */
typedef unsigned int    UWORD;          /* unsigned machine word        */
typedef unsigned int  * PUWORD;         /* ptr to unsigned word         */
typedef short int       COUNT;          /* general counter var.         */
typedef int             RETCODE;        /* integer return code          */
typedef FILE          * PFILE;          /* ptr to file type             */

/*      Common Variable Modifiers                                       */

#define VOID    void
#define SLREG     register                /* register modifier            */
#define DSTATIC static  /* Used to declare Data as Static               */
#define FSTATIC static  /* Used to declear a Function as static         */

/*                     System Attributes Definitions                        *
 *                                and                                       *
 *                       System Specific #includes                          *
*/                                                                         

#ifdef  MSDOS                   /* IBM PC specific defines              */

#include "slattdos.h"

#endif

#ifdef  VAX                     /* VAX/VMS specific defines             */

#include "slattvms.h"

#endif                          /* End of VAX/VMS section               */

#ifdef  UNIX                    /* UNIX specific defines                */

#include "slattunx.h"

#endif                          /* End of UNIX section                  */

#ifdef  MAC                     /* Macintosh specific defines           */

#include "slattmac.h"

#endif                          /* End of MAC section                   */

#ifdef  WINDOWS                 /* WINDOWS specific defines             */

#include "slattwin.h"

#endif                          /* End of WINDOWS section               */

#ifdef  WIN32

#include "slattw32.h"           /* WIN32 specific defines               */

#endif 

#ifdef  OS2                     /* OS2 specific defines                 */

#include "slattos2.h"

#endif                          /* End of OS/2 section                  */


#ifdef POWERMAC                 /* Power Macintosh                      */

#include "slattpmc.h"

#endif


/*      The following defines are overrides for defines possibly found in
 *      any of the supported compilers.  We always undefine or redefine them,
 *      and must wait until after a compiler's auxillary header files have
 *      been included.
*/
#include "slundef.h"            /* undef stuff duped below; merge compilers */

/* null defines from SLMEMMAN.H preferred
*/
#define NULL    0               /* Generic null value.  See ANSI on NULL    */


/*      The following defines depend on attributes found in each
 *      compiler/platform.  They'll be defined or undefined above depending 
 *      on the compiler/platform.
*/


/*                      Function Calling conventions
*/

#ifdef  SLA_CPASC_FC

#ifdef  SLA_UUCPASC_FC  /* double underscore */
#define PASCAL  __pascal    /* Used for pascal calling convention   */
#define _CDECL  __cdecl     /* Used for C calling convention        */
#else   /* !SLA_UUCPASC_FC */
#define PASCAL   _pascal    /* Used for pascal calling convention   */
#define _CDECL   _cdecl     /* Used for C calling convention        */
#endif

#else   /* !SLA_CPASC_FC */

#define PASCAL
#define _CDECL

#endif  /* SLA_CPASC_FC */



/*                      Function Declaration services
*/

/* Define main() routine
*/
#ifdef  SLA_UMAIN_FD
#define Cmain   _main
#else
#define Cmain   main
#endif  /* SLA_UMAIN_FD */

/* How do functions get prototyped?  ANSI?  Or old-style declarations?
*/
#ifdef SLA_PROTO_FD
#define PROTOTYPE       /* ANSI */
#else
#undef PROTOTYPE        /* Old-style */
#endif  /* SLA_PROTO_FD */

/* EXPORT is used to label functions that are part of an interface.  These
 * interface funcs are called from outside a library.  In order to find them
 * at call time, some platforms require a special label for them.
 *
 * You can use the SLAPI defines, that combine EXPORT with other modifiers to
 * create a package for interface functions.  Different platforms require 
 * that modifiers appear in different orders.  SLAPI defines make sure they
 * come in the right order.  They appear below.
*/
#ifdef SLA_EXPORT_FD


#ifdef SLA_LOADDS_FD

#define EXPORT __export __loadds  /* Need __loadds included with __export */

#else   /* SLA_EXPORT_FD !SLA_LOADDS_FD */

#ifdef SLA_DLLEXP_FD    /* Special syntax for DLLs */

#ifdef SL_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif /* SL_DLL */

#else   /* SLA_EXPORT_FD !SLA_LOADDS_FD !SLA_DLLEXP_FD */

#define EXPORT __export          /* Don't need __loadds - just use __export */

#endif  /* SLA_DLLEXP_FD */

#endif /* SLA_LOADDS_FD */


#else  /* !SLA_EXPORT_FD.  EXPORT isn't used; API functions have no special
        * label.
       */

#define EXPORT


#endif /* SLA_EXPORT_FD */

/* Package for API functions.  Use this label for functions that are entry
 * points to your product.  Your function declaration should be in the form
 *      <return type> SLAPI_CDECL function( ... )   for API functions using 
 *          the C calling convention and
 *      <return type> SLAPI_PASCAL function( ... )  for API functions using 
 *          the Pascal calling convention
 *
 * Note: Some compilers currently require programmers to manually
 * conform to a naming convention in the .DEF files that is implicitly 
 * defined by the calling convention you choose.  SLAPI_CDECL functions' 
 * names must appear with a preceding underscore, and SLAPI_PASCAL functions' 
 * names must appear in all uppercase.  For example, OS/2 .DEF files require
 * this syntax:
 *
 * Source:                  DLL's .DEF file:    App's .DEF file:    
 * -------                  ----------------    ----------------
 * VOID SLAPI_CDECL  func1  EXPORTS _func1      IMPORTS <DLLNAME>._func1
 * VOID SLAPI_PASCAL func2  EXPORTS FUNC2       IMPORTS <DLLNAME>.FUNC2
 *
 * where <DLLNAME> is the name of the DLL as specified in the LIBRARY 
 * directive of the DLL's .DEF file.
*/
#ifdef SLA_CCPREC_FD    /* Calling Convention PRECedes 'export' */

#define SLAPI_CDECL     FAR _CDECL EXPORT
#define SLAPI_PASCAL    FAR PASCAL EXPORT

#else

#define SLAPI_CDECL     EXPORT FAR _CDECL 
#define SLAPI_PASCAL    EXPORT FAR PASCAL 

#endif  /* SLA_CCPREC_FD */



/*                          Memory Access
*/

#ifdef SLA_GLOBEXT_MA

#define GLOBALDATA globaldef    /* Data is declared to be Global        */
#define SLEXTERN     globalref    /* External definition for Global Data  */

#else   /* !SLA_GLOBEXT_MA */

#define GLOBALDATA      /* no different than other declarations */
#define SLEXTERN  extern  /* External definition for Global Data          */

#endif  /* SLA_GLOBEXT_MA */

#ifdef SLA_RODATA_MA

#define READONLY   readonly     /* Global data is Readonly              */

#else   /* !SLA_RODATA_MA */

#define READONLY        /* No meaning - neutralize                      */

#endif  /* SLA_RODATA_MA */

/* The following attribute, SLA_DSEQSS_MA, is implicitly dependent upon 
 * SLA_SEGMENT_AM because DS and SS are present only in segmented architec-
 * tures.  To simplify ifdef nesting, its ifdef is presented separately.
 *
 * Typedefs below are used to make something far only under [windows].  Used
 * for stack variables that must be far [in windows DLLs] because the stack
 * belongs to the application, not the DLL.
 *
 * The prefix 'W' is historically for Windows, but since we no longer cater
 * to specific enivonments but rather their attributes, please ignore the
 * association.  Kept for posterity.
 *
 * E.g.: you have a stack variable.  Pass &var to function.  The prototype for
 * that parameter must be PW -type.  Need not be FAR in DOS, because DS==SS.
 * Only in these cases should one use the PW datatypes, and only PW -type 
 * variables should be passed to CW functions (types & functs. defined later).
 *
*/
#ifdef SLA_DSEQSS_MA
#define WFAR
#define WSTATIC
#else
#define WFAR    FAR
#define WSTATIC static
#endif


/*                      Architecture of Machine attributes              */

#ifdef  SLA_SEGMENT_AM

#define FAR     far     /* Used to declare Data as far                  */
#define NEAR    near    /* Used to declare Data as near                 */
#define FNEAR   near    /* Used to declare a Function as near           */

#ifdef  SLA_FARCTALLOC_MA   /* memory access attributes applicable only to 
                             * segmented architectures
                            */
/* NWFAR should only be used as a modifier for STATIC variables.
*/
#define NWFAR   FAR     /* allocate this data to far segment at comp. time */
#else
#define NWFAR
#endif  /* SLA_FARCTALLOC_MA */

#else   /* !SLA_SEGMENT_AM */

#define FAR             /* Has no meaning - neutralize                  */
#define NEAR            /* Has no meaning - neutralize                  */
#define FNEAR           /* Has no meaning - neutralize                  */
#define NWFAR           /* Has no meaning - neutralize                  */

#endif  /* SLA_SEGMENT_AM */

#ifdef SLA_HUGEPTR_DT
#define HUGE    huge    /* Used to declare Data as huge                 */
#else
#define HUGE            /* Has no meaning - neutralize                  */
#endif

/* Use an int instead of a long to make signed and unsigned 4-byte numeric 
 * type if ints are 32 bits, because some platforms don't have unsigned 
 * longs (VAX).
*/

#ifdef SLA_4BYTEINT_AM

typedef unsigned int    UINT4B;         /* 4 byte unsigned              */
typedef unsigned int  * PUINT4B;        /* 4 byte unsgd ptr             */
typedef int             INT4B;          /* 4 byte integer               */
typedef int           * PINT4B;         /* 4 byte integer ptr           */

#else

typedef long            INT4B;          /* 4 byte integer               */
typedef long          * PINT4B;         /* 4 byte integer ptr           */
typedef unsigned long   UINT4B;         /* 4 byte unsigned int          */
typedef unsigned long  *PUINT4B;        /* 4 byte unsgd int ptr         */

#endif /* SLA_4BYTEINT_AM */


/*                                Data Types                                */

/* Explicitly signed character data types
*/
#ifdef SLA_SCHAR_DT

typedef signed char           SCHAR;    /* explicitly signed character  */
typedef signed char         * PSCHAR;   /* explicitly signed char ptr   */
typedef signed char FAR     * PFSCHAR;  /* Signed far char ptr.         */
typedef signed char NEAR    * PNSCHAR;  /* Signed near char ptr.        */
typedef signed char         * PXSCHAR;  /* Signed char ptr.             */
typedef signed char WFAR    * PWFSCHAR; /* far explicitly signed ch ptr */

#else

typedef char                  SCHAR;    /* plain character              */
typedef char                * PSCHAR;   /* character ptr                */
typedef char FAR            * PFSCHAR;  /* Far character ptr.           */
typedef char NEAR           * PNSCHAR;  /* Near char ptr.               */
typedef char                * PXSCHAR;  /* Chararacter ptr.             */
typedef char WFAR           * PWFSCHAR; /* far character ptr            */

#endif   /* SLA_SCHAR_DT */

/*                             File Operations
*/

#ifdef  SLA_INTPATH_FO      /* use integer for file path */
typedef struct _SLPATH { INT2B path; } SLPATH;
#else
typedef struct _SLPATH { UCHAR path[SLPATHMAX + 1]; } SLPATH;
#endif  /* SLA_INTPATH_FO */

typedef SLPATH *PX_SLPATH;
typedef SLPATH FAR *PF_SLPATH;
typedef SLPATH NEAR *PN_SLPATH;

/* SLA_INT2BFHDL_FO serves the Macintosh, which uses a short int for file
 * handles in its toolbox routines.  Our products have traditionally used a UWORD 
 * for file handles, which is a plain 'int', because the function 'open()' 
 * returns an int.  Using UWORD for file handles on the Mac works OK if the
 * compiler is Think C because 'int' is 16 bits with this compiler.  MPW,
 * however, treats 'int' as 32 bits (incidentally, more accurate than Think
 * C's implementation).
 *
 * When the _l file routines are used (_lopen, _lread), their argument for
 * file handle is defined as HFILE.
*/
#ifdef SLA_INT2BFHDL_FO

typedef INT2B SL_FHANDLE;

#else

#ifdef SLA_LFILE_LF
typedef HFILE SL_FHANDLE;
#else
typedef SWORD SL_FHANDLE;
#endif  /* SLA_LFILE_LF */

#endif  /* SLA_INT2BFHDL_FO */

typedef SL_FHANDLE FAR *PFSL_FHANDLE;

/* File Open Modes.  
 * These defines help standardize the flags passed to open() and fopen() or 
 * the variant routines we replace them with.
*/

/* File open sharing modes only used by UNIX and POWERMAC.
   Internal representations of file sharing and locking modes.
*/
#if defined(SLA_UNXOPENFLG_FO) && !defined(SLA_BINFILE_FO) && !defined(SLA_TXTFILE_FO)

#define SL_UNX_SH_READ  0x1000  /* Shared, read only open file mode. */
#define SL_UNX_SH_RDWR  0x2000  /* Shared, excl. read/write open file mode. */

#endif

/*      Binary file open modes
 * If this environment distinguished between text & binary files, make all
 * binary file-open flags specify binary.
*/
#ifdef  SLA_BINFILE_FO


/* fopen() flags
*/
#define FOPN_RB  "rb"   /* open existing file for read in binary mode   */
#define FOPN_RWB "r+b"  /* open existing file for r/w in binary mode    */
#define FOPN_CWB "wb"   /* create new file for writing in binary mode   */
#define FOPN_CWAB "ab"  /* write append to binary file, create if !exist*/
#define FOPN_CWEB "wb"  /* write to binary file, don't clear if exist*/

/* open() flags.  These defines split into 2 groups: environments where we
 * use a Unix-like open() and environments where we use _lopen/_lcreat or 
 * _dos_open/_dos_creat.  open takes Unix mode flags like O_RDONLY; 
 * _lopen & _dos_open take the flags READ, READ_WRITE, and WRITE.  open can
 * create a non-existant file, _lopen & _dos_open can't - _lcreat and 
 * _dos_creat are used instead.  creat will be used when create modes are
 * specified (OPEN_CWB) but they use yet another set of mode flags.
*/
#ifdef SLA_UNXOPENFLG_FO

#define OPEN_RB   (O_RDONLY | O_BINARY)
#define OPEN_RWB  (O_RDWR   | O_BINARY)

#ifdef  SLA_WOCREAT_FO      /* Environment has a 'write only' mode */
#define OPEN_CWB  (O_WRONLY | O_CREAT | O_BINARY | O_TRUNC) /* Truncate */
#define OPEN_CWEB (O_WRONLY | O_CREAT | O_BINARY)           /* Existing */
#define OPEN_CWAB (O_WRONLY | O_CREAT | O_BINARY | O_APPEND)/* Append */
#else                       /* No 'write only' - CWB will be 'read/write' */
#define OPEN_CWB  (O_RDWR | O_CREAT | O_BINARY | O_TRUNC)   /* Truncate */
#define OPEN_CWEB (O_RDWR | O_CREAT | O_BINARY)             /* Existing */
#define OPEN_CWAB (O_RDWR | O_CREAT | O_BINARY | O_APPEND)  /* Append */
#endif  /* SLA_WOCREAT_FO */

#define OPEN_SRO  (O_RDONLY | SH_DENYWR)   /* share, read only */
#define OPEN_SEXC (O_RDWR | SH_DENYRW)     /* share, deny other programs 
                                              read or write access 
                                           */

#else /* SLA_BINFILE_FO !SLA_UNXOPENFLG_FO */

#define OPEN_RB   OF_READ       /* use OF_READ (OpenFile) - READ n/a in W32 */
#define OPEN_RWB  OF_READWRITE
#define OPEN_WB   OF_WRITE
#define OPEN_CWB  5 /* In order for file cover routines to decide to use 
                     * creat instead of open, they check mode to see if it's 
                     * OPEN_CWB.  MAKE SURE OPEN_CWB HAS A VALUE UNIQUE FROM 
                     * OPEN_RB, OPEN_RWB, AND OPEN_WB!
                    */
#define OPEN_CWAB 6
#define OPEN_CWEB 7

/* shared file modes (not limited to a single application) */
#define OPEN_SRO  (OF_READ | OF_SHARE_DENY_WRITE)     /* read only */
#define OPEN_SEXC (OF_READWRITE | OF_SHARE_EXCLUSIVE) /* deny others 
                                                         read or write access 
                                                      */

#endif /* else of ifdef SLA_UNXOPENFLG_FO */


#else /* !SLA_BINFILE_FO */


/* Environment doesn't distinguish between binary & text, so make no 
 * distinction for binary in the file open flags.
*/

/* fopen() flags
*/
#define FOPN_CWB    "w"
#define FOPN_CWAB   "a" /* write append to file, create if !exist*/
#define FOPN_CWEB   "w" /* write to file, don't clear if exist*/
#define FOPN_RB     "r"
#define FOPN_RWB    "r+"

/* open() flags.  See comment above on SLA_UNXOPENFLG_FO
*/
#ifdef SLA_UNXOPENFLG_FO

#define OPEN_RB    (O_RDONLY)       
#define OPEN_RWB   (O_RDWR)

#ifdef  SLA_WOCREAT_FO      /* Environment has a 'write only' mode */
#define OPEN_CWB    (O_WRONLY | O_TRUNC | O_CREAT )         /* Truncate */
#define OPEN_CWEB   (O_WRONLY | O_CREAT)                    /* Existing */
#define OPEN_CWAB   (O_WRONLY | O_CREAT | O_APPEND)         /* Append */
#else                       /* No 'write only' - CWB will be 'read/write' */
#define OPEN_CWB    (O_RDWR | O_TRUNC | O_CREAT )           /* Truncate */
#define OPEN_CWEB   (O_RDWR | O_CREAT)                      /* Existing */
#define OPEN_CWAB   (O_RDWR | O_CREAT | O_APPEND)           /* Append */
#endif  /* SLA_WOCREAT_FO */

#define OPEN_SRO  (O_RDONLY | SL_UNX_SH_READ)   /* share, read only */
#define OPEN_SEXC (O_RDWR | SL_UNX_SH_RDWR)     /* share, deny other programs 
                                                   read or write access 
                                                */

#else /* !SLA_BINFILE_FO !SLA_UNXOPENFLG_FO */

#define OPEN_RB   OF_READ       /* use OF_READ (OpenFile) - READ n/a in W32 */
#define OPEN_RWB  OF_READWRITE
#define OPEN_WB   OF_WRITE
#define OPEN_CWB  5 /* In order for file cover routines to decide to use 
                     * creat instead of open, they check mode to see if it's 
                     * OPEN_CWB.  MAKE SURE OPEN_CWB HAS A VALUE UNIQUE FROM 
                     * OPEN_RB, OPEN_RWB, AND OPEN_WB!
                    */
#define OPEN_CWAB 6
#define OPEN_CWEB 7

/* shared file modes (not limited to a single application) */
#define OPEN_SRO  (OF_READ | OF_SHARE_DENY_WRITE)     /* read only */
#define OPEN_SEXC (OF_READWRITE | OF_SHARE_EXCLUSIVE) /* deny others 
                                                         read or write access 
                                                      */

#endif /* else of ifdef SLA_UNXOPENFLG_FO */
#endif  /* SLA_BINFILE_FO */


/*      Text file open modes
 * If this environment distinguished between text & binary files, make all
 * text file-open flags specify text.
*/
#ifdef  SLA_TXTFILE_FO


/* fopen() flags
*/
#define FOPN_CWT    "wt"   /* create new file for writing in text mode      */
#define FOPN_CWAT   "at"   /* open for write append, create if !exist       */   
#define FOPN_CWET   "wt"   /* open for write, don't clear if exist          */
#define FOPN_RT     "rt"   /* open existing file for read in text mode      */
/* open existing file for r/w in text mode
*/
#ifdef  SLA_TXTSTR_FO
#define FOPN_RWT "r+t"
#else
#define FOPN_RWT "r+"
#endif  /* SLA_TXTSTR_FO */

/* open() flags.  See comment in SLA_BINFILE_FO section about Unix & non-Unix
 * file open modes.
*/
#ifdef SLA_UNXOPENFLG_FO

#define OPEN_RT   (O_RDONLY | O_TEXT)
#define OPEN_RWT  (O_RDWR   | O_TEXT)

#ifdef  SLA_WOCREAT_FO      /* Environment has a 'write only' mode */
#define OPEN_CWT    (O_WRONLY | O_CREAT | O_TEXT | O_TRUNC)     /* Truncate */
#define OPEN_CWET   (O_WRONLY | O_CREAT | O_TEXT)               /* Existing */
#define OPEN_CWAT   (O_WRONLY | O_CREAT | O_TEXT | O_APPEND)    /* Append   */
#else                       /* No 'write only' - CWB will be 'read/write' */
#define OPEN_CWT    (O_RDWR | O_TRUNC | O_CREAT | O_TEXT)       /* Truncate */
#define OPEN_CWET   (O_RDWR | O_CREAT | O_TEXT)                 /* Existing */
#define OPEN_CWAT   (O_RDWR | O_CREAT | O_TEXT | O_APPEND)      /* Append   */
#endif  /* SLA_WOCREAT_FO */

#else /* SLA_TXTFILE_FO !SLA_UNXOPENFLG_FO */

#define OPEN_RT   OF_READ       /* use OF_READ (OpenFile) - READ n/a in W32 */
#define OPEN_RWT  OF_READWRITE
#define OPEN_WT   OF_WRITE
#define OPEN_CWT  5 /* In order for file cover routines to decide to use 
                     * creat instead of open, they check mode to see if it's 
                     * OPEN_CWB.  MAKE SURE OPEN_CWB HAS A VALUE UNIQUE FROM 
                     * OPEN_RB, OPEN_RWB, AND OPEN_WB!
                    */
#define OPEN_CWAT 6
#define OPEN_CWET 7

/* shared file modes (not limited to a single application) */
#define OPEN_SRO  (OF_READ | OF_SHARE_DENY_WRITE)     /* read only */
#define OPEN_SEXC (OF_READWRITE | OF_SHARE_EXCLUSIVE) /* deny others 
                                                         read or write access 
                                                      */

#endif /* else of ifdef SLA_UNXOPENFLG_FO */


#else /* !SLA_TXTFILE_FO */


/* Environment doesn't distinguish between binary & text, so make no 
 * distinction for text in the file open flags.
*/

/* fopen() flags
*/
#define FOPN_CWT   "w"
#define FOPN_CWET  "w"
#define FOPN_CWAT  "a"
#define FOPN_RT    "r"
/* open existing file for r/w in text mode
*/
#ifdef  SLA_TXTSTR_FO   /* both the same since text not specified */
#define FOPN_RWT "r+"
#else
#define FOPN_RWT "r+"
#endif  /* SLA_TXTSTR_FO */

/* open() flags.  See comment above on SLA_UNXOPENFLG_FO.
*/
#ifdef SLA_UNXOPENFLG_FO

#define OPEN_RT    (O_RDONLY)
#define OPEN_RWT   (O_RDWR)                        

#ifdef  SLA_WOCREAT_FO      /* Environment has a 'write only' mode */
#define OPEN_CWT    (O_WRONLY | O_CREAT | O_TRUNC)              /* Truncate */
#define OPEN_CWET   (O_WRONLY | O_CREAT)                        /* Existing */
#define OPEN_CWAT   (O_WRONLY | O_CREAT | O_APPEND)             /* Append   */
#else                       /* No 'write only' - CWB will be 'read/write' */
#define OPEN_CWT    (O_RDWR | O_CREAT | O_TRUNC)                /* Truncate */
#define OPEN_CWET   (O_RDWR | O_CREAT)                          /* Existing */
#define OPEN_CWAT   (O_RDWR | O_CREAT | O_APPEND)               /* Append   */
#endif  /* SLA_WOCREAT_FO */

#if !defined(SLA_BINFILE_FO)
#define OPEN_SRO  (O_RDONLY | SL_UNX_SH_READ)   /* share, read only */
#define OPEN_SEXC (O_RDWR | SL_UNX_SH_RDWR)     /* share, deny other programs 
                                                   read or write access 
                                                */
#else
#define OPEN_SRO  (O_RDONLY | SH_DENYWR)   /* share, read only */
#define OPEN_SEXC (O_RDWR | SH_DENYRW)     /* share, deny other programs 
                                              read or write access 
                                           */
#endif

#else /* !SLA_TXTFILE_FO !SLA_UNXOPENFLG_FO */

#define OPEN_RB   OF_READ       /* use OF_READ (OpenFile) - READ n/a in W32 */
#define OPEN_RWT  OF_READWRITE
#define OPEN_WT   OF_WRITE
#define OPEN_CWT  5 /* In order for file cover routines to decide to use 
                     * creat instead of open, they check mode to see if it's 
                     * OPEN_CWB.  MAKE SURE OPEN_CWB HAS A VALUE UNIQUE FROM 
                     * OPEN_RB, OPEN_RWB, AND OPEN_WB!
                    */
#define OPEN_CWAT 6
#define OPEN_CWET 7

/* shared file modes (not limited to a single application) */
#define OPEN_SRO  (OF_READ | OF_SHARE_DENY_WRITE)     /* read only */
#define OPEN_SEXC (OF_READWRITE | OF_SHARE_EXCLUSIVE) /* deny others 
                                                         read or write access 
                                                      */

#endif /* else of ifdef SLA_UNXOPENFLG_FO */
#endif  /* SLA_TXTFILE_FO */



/*          Types that depend on segmented architecture attribute.
*/

typedef char FAR            * PFCHAR;   /* Far character ptr.           */
typedef char NEAR           * PNCHAR;   /* Near character ptr.          */
typedef char                * PXCHAR;   /* Character ptr.               */

typedef unsigned char FAR   * PFUCHAR;  /* Unsigned far char ptr.       */
typedef unsigned char NEAR  * PNUCHAR;  /* Unsigned near char ptr.      */
typedef unsigned char HUGE  * PHUCHAR;  /* Unsigned huge char ptr.      */
typedef unsigned char       * PXUCHAR;  /* Unsigned char ptr.           */

typedef int FAR             * PFSWORD;  /* Far int ptr.                 */
typedef int NEAR            * PNSWORD;  /* Near int ptr.                */
typedef int                 * PXSWORD;  /* Int ptr.                     */

typedef unsigned int  FAR   * PFUWORD;  /* Far unsigned int ptr.        */
typedef unsigned int  NEAR  * PNUWORD;  /* Near unsigned int ptr.       */
typedef unsigned int        * PXUWORD;  /* Unsigned int ptr.            */

typedef short int FAR       * PFINT2B;  /* Far 2-byte int ptr.          */
typedef short int NEAR      * PNINT2B;  /* Near 2-byte int ptr.         */
typedef short int           * PXINT2B;  /* 2-byte int ptr.              */

typedef unsigned short int FAR  * PFUINT2B; /* Far unsigned 2-byte int. */
typedef unsigned short int NEAR * PNUINT2B; /* Near unsigned 2-byte int.*/
typedef unsigned short int      * PXUINT2B; /* Unsigned 2-byte int.     */

typedef long FAR            * PFINT4B;  /* Far 4-byte int ptr.          */
typedef long NEAR           * PNINT4B;  /* Near 4-byte int ptr.         */
typedef long                * PXINT4B;  /* 4-byte int ptr.              */

typedef unsigned long FAR   * PFUINT4B; /* Far 4-byte int ptr.          */
typedef unsigned long NEAR  * PNUINT4B; /* Near 4-byte int ptr.         */
typedef unsigned long       * PXUINT4B; /* 4-byte int ptr.              */

typedef char FAR            * PFFLAG;   /* Far pointer to flags.        */
typedef char NEAR           * PNFLAG;   /* Near pointer to flags.       */
typedef char                * PXFLAG;   /* Pointer to flags.            */

/*              Types that depend on DS==SS attribute
 * (please see note above where WFAR is defined.  The note on ifdef nesting
 * & implicit dependence applies here as well).
*/
typedef char               WFAR * PWFCHAR;   /* far character ptr           */
typedef unsigned char      WFAR * PWFUCHAR;  /* unsigned far char ptr       */
typedef int                WFAR * PWFSWORD;  /* far ptr to signed int       */
typedef unsigned int       WFAR * PWFUWORD;  /* far ptr to unsigned int     */
typedef short int          WFAR * PWFINT2B;  /* far ptr to 2 byte int       */
typedef unsigned short int WFAR * PWFUINT2B; /* far ptr to 2 byte unsgnd int*/
typedef long               WFAR * PWFINT4B;  /* far ptr to 4 byte int       */
typedef unsigned long      WFAR * PWFUINT4B; /* far ptr to 4 byte unsgnd int*/

typedef char               WFAR * PWFFLAG;   /* far pointer to flags        */


/*                            Memory Management
 *          These defines are provided here for backward compatibility in 
 *          products that do not use the memory management macros in 
 *          SLMEMMAN.H (when those macros are used, the defines below are 
 *          not necessary).
*/

#ifdef  SLA_HDL_MM      /* System uses handles                          */

#define USE_HANDLES

#ifdef  SLA_HDLPTR_MM   /* Handles are pointers to pointers             */
#define MAC_HANDLES
#else
#define WINDOWS_HANDLES
#endif  /* SLA_HDLPTR_MM */

#endif  /* SLA_HDL_MM   */


/* Data types for wide character support. */

typedef unsigned short int ICHAR;  /* Unsigned 2-byte character     */
typedef ICHAR *PICHAR;             /* Unsigned 2-byte character ptr */
typedef ICHAR HUGE *PHICHAR;       /* Unsigned huge 2-byte char ptr */

#ifndef WEOF                       /* end of file for wide characters */
#define WEOF (ICHAR)(0xFFFF)
#endif



#if defined(WINDOWS) || defined(WIN32) || defined(_WINDOWS)
#define SL_ALIGN pack(4)        /* 4 byte alignment */
#elif defined(OS2)
#ifdef __BORLANDC__
#define SL_ALIGN option -a      /* word align to even boundaries */
#endif
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(_WINDOWS)
#define SL_STOPALIGN pack()     /* revert to default */
#elif defined(OS2)
#ifdef __BORLANDC__
#define SL_STOPALIGN option -a. /* revert to default */
#endif
#endif

/* Define MSDOS here if you're compiling under WINDOWS but need MSDOS
 * defined in your product.
*/

#endif /* SL_H */

/*      End of file - sl.h      */
