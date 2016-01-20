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
*       slcalls.h - C library calls for Standard Library.               *
*                                                                       *
*-----------------------------------------------------------------------*

    Description:
        This file defines all "C" library calls with a beginning 
        C character so that system-specific changes can be implemented
        throughout a system without code changes within source.
    
    Cautions:
        Place new macros under appropriate attributes.  If a new 
        attribute is added, update all SLATT*.H files.

    Creation Date:
        18-jul-89        Created by InfoSoft Software Engineering

    Change Record:
        17-sep-91   ECH     Cfcalloc, Cnmalloc defined for SLMM
        24-sep-91   JCC     Applied environment attributes defines
        11-mar-92   JES     Moved CTOI from slascii.h to here.
        15-apr-92   JES     Removed MoveHHi from MAC Lock Macros
        29-oct-92   RCN     CAllocHugeMem -> Challoc
        08-nov-92   JCC     Borland support
        17-dec-92   JCC     SLfstrchr
        17-may-93   JCC     Parenthesize dangerous macro arguments

*-----------------------------------------------------------------------*/
#ifndef SLCALLS_H

#define SLCALLS_H


/*                  Defines common to all environments                  */



/* Macros that are not redirected to functions
*/

#define Cabs(x)         ((x) < 0 ? -(x) : (x))      /* absolute value       */
#define Cmin(x,y)       (((x) > (y)) ? (y) : (x))   /* minimum value        */
#define Cmax(x,y)       (((x) > (y)) ? (x) : (y))   /* maximum value        */
#define CTOI(c)         ((c) - '0')                 /* convert ascii digit 
                                                    * char to integer
                                                    */

/* Function defines and macros for operating system and runtime calls
 *
 * NOTE: standard file routines are not reliable under THINK_C in a DA.
 *       Use SLFILCOV.C functions instead.
*/

#define Cassert         assert          /* Generic assert() call        */
#define Catoi           atoi            /* Generic atoi() call          */
#define Catol           atol            /* Generic atol() call          */
#define Cclose          close           /* Generic close() call         */
#define Cexit           exit            /* Generic exit() call          */
#define Cfclose         fclose          /* Generic fclose() call        */
#define Cfeof           feof            /* Generic feof() call          */
#define Cferror         ferror          /* Generic ferror() call        */
#define Cfflush         fflush          /* Generic fflush() call        */
#define Cfgetc          fgetc           /* Generic fgetc() call         */
#define Cfgets          fgets           /* Generic fgets() call         */
#define Cfopen          fopen
#define Cfprintf        fprintf         /* Generic fprintf() call       */
#define Cfputc          fputc           /* Generic fputc() call         */
#define Cfputs          fputs           /* Generic fputs() call         */
#define Cfread          fread           /* Generic fread() call         */
#define Cfscanf         fscanf          /* Generic fscanf() call        */
#define Cfseek          fseek           /* Generic fseek() call         */
#define Cftell          ftell           /* Generic ftell() call         */
#define Cfwrite         fwrite          /* Generic fwrite() call        */
#define Cgetc           getc            /* Generic getc() call          */
#define Cgetchar        getchar         /* Generic getchar() call       */
#define Cungetc         ungetc          /* Generic ungetc() call        */

#define Cgets           gets            /* Generic gets() call          */
#define Cisalnum        isalnum         /* Generic isalnum() call       */
#define Cisalpha        isalpha         /* Generic isalpha() call       */
#define Cisascii        isascii         /* Generic isascii() call       */
#define Ciscntrl        iscntrl         /* Generic iscntrl() call       */
#define Cisdigit        isdigit         /* Generic isdigit() call       */
#define Cislower        islower         /* Generic islower() call       */
#define Cisprint        isprint         /* Generic isprint() call       */
#define Cispunct        ispunct         /* Generic ispunct() call       */
#define Cisspace        isspace         /* Generic isspace() call       */
#define Cisupper        isupper         /* Generic isupper() call       */
#define Clseek          lseek           /* Generic lseek() call         */
#define Cprintf         printf          /* Generic printf() call        */
#define Cputc           putc            /* Generic putc() call          */
#define Cputchar        putchar         /* Generic putchar() call       */
#define Cputs           puts            /* Generic puts() call          */
#define Crealloc        realloc         /* Generic realloc() call       */
#define Cscanf          scanf           /* Generic scanf() call         */
#define Csscanf         sscanf          /* Generic sscanf() call        */
#define Ctime           time            /* Generic time() call          */

/* cast these calls for signed char environments (don't hurt in any
 * environment).
*/
#define Cread(a,b,c)    read ((a),(PCHAR)(b),(c))   /* Generic read() call  */
#define Cwrite(a,b,c)   write((a),(PCHAR)(b),(c))   /* Generic write() call */



/*              Defines altered with an environment's attributes        */


/* Operating System calls
*/


#ifdef SLA_REMOVE_LF
#define Cremove         remove
#else
#define Cremove         unlink
#endif

#ifdef SLA_GETENV_LF    /* NOTE: undefined otherwise */
#define Cgetenv         getenv          /* Generic getenv() call        */
#endif


/* string & char array manipulation operations
*/

#ifdef SLA_CHKSTRTO_FC
#define Ctolower        tolower         /* Generic tolower() call       */
#define Ctoupper        toupper         /* Generic toupper() call       */
#else
#define Ctolower(a)     ((Cisupper( (a) ) ) ? tolower( (a) ) : (a))
#define Ctoupper(a)     ((Cislower( (a) ) ) ? toupper( (a) ) : (a))
#endif  /* SLA_CHKSTRTO_FC */


#ifdef SLA_SCHAR_STR_FC     /* signed char string function calls */

/* The PCHAR casts are here to suppress warnings in compilers where these
 * functions take signed strings as arguments.
*/
#define Cmemcmp(a,b,c)  memcmp    ((PCHAR) (a), (PCHAR) (b), (c))
#define Cmemcpy(a,b,c)  memcpy    ((PCHAR) (a), (PCHAR) (b), (c))
#define Cmemset(a,b,c)  memset    ((PCHAR) (a), (b), (c))
#define Cmemchr(a,b,c)  memchr    ((PCHAR) (a), (b), (c))
#define Cstrlen(a)      strlen    ((PCHAR) (a))
#define Cstrcpy(a,b)    strcpy    ((PCHAR) (a), (PCHAR) (b))
#define Cstrncpy(a,b,c) strncpy   ((PCHAR) (a), (PCHAR) (b), (c))
#define Cstrcat(a,b)    strcat    ((PCHAR) (a), (PCHAR) (b))
#define Cstrncat(a,b,c) strncat   ((PCHAR) (a), (PCHAR) (b), (c))
#define Cstrchr(a,b)    strchr    ((PCHAR) (a), (CHAR)  (b))
#define Cstrstr(a,b)    strstr    ((PCHAR) (a), (PCHAR) (b))
#ifdef SLA_SLSTRCMP_LF
/* SLstr functions take unsigned strings.
*/
#define Cstrcmp(a,b)    SLstrcmp  ((PUCHAR) (a), (PUCHAR) (b))
#define Cstrncmp(a,b,c) SLstrncmp ((PUCHAR) (a), (PUCHAR) (b), (c))
#else
#define Cstrcmp(a,b)    strcmp    ((PCHAR) (a), (PCHAR) (b))
#define Cstrncmp(a,b,c) strncmp   ((PCHAR) (a), (PCHAR) (b), (c))
#endif
#define Cstrpbrk(a,b)   strpbrk   ((PCHAR) (a), (PCHAR) (b))
#define Cstrspn(a,b)    strspn    ((PCHAR) (a), (PCHAR) (b))
#define Cstrcspn(a,b)   strcspn   ((PCHAR) (a), (PCHAR) (b))

#else

#define Cmemcmp         memcmp          /* Generic memcmp()  call       */
#define Cmemcpy         memcpy          /* Generic memcpy()  call       */
#define Cmemset         memset          /* Generic memset()  call       */
#define Cmemchr         memchr          /* Generic memchr()  call       */
#define Cstrlen         strlen          /* Generic strlen()  call       */
#define Cstrcpy         strcpy          /* Generic strcpy()  call       */
#define Cstrncpy        strncpy         /* Generic strncpy() call       */
#define Cstrcat         strcat          /* Generic strcat()  call       */
#define Cstrncat        strncat         /* Generic strncat() call       */
#define Cstrchr         strchr          /* Generic strchr()  call       */
#define Cstrstr         strstr          /* Generic strstr()  call       */
#ifdef SLA_SLSTRCMP_LF
#define Cstrcmp         SLstrcmp        /* BSD strcmp() call            */
#define Cstrncmp        SLstrncmp       /* BSD strncmp() call           */
#else
#define Cstrcmp         strcmp          /* Generic strcmp() call        */
#define Cstrncmp        strncmp         /* Generic strncmp() call       */
#endif
#define Cstrpbrk        strpbrk         /* Generic strpbrk() call       */
#define Cstrspn         strspn          /* Generic strspn()  call       */
#define Cstrcspn        strcspn         /* Generic strcspn() call       */

#endif /* SLA_SCHAR_STR_FC */

/* string & char array manipulation operations continued ...
*/
/* These functions are for use in environments with segmented architectures.
 * They are specifically for accessing FAR data.
*/

#ifdef SLA_WSPRINTF_LF
#define Csprintf        wsprintf        /* Generic wsprintf() call      */
#else
#define Csprintf        sprintf         /* Generic sprintf() call       */
#endif

#ifdef  SLA_SEGMENT_AM

#ifdef  SLA_FARCHAR_LF
#define Cfstrcmp(a,b)         _fstrcmp  ((PFCHAR) (a), (PFCHAR) (b))
#define Cfstrncmp(a,b,c)      _fstrncmp ((PFCHAR) (a), (PFCHAR) (b), c)
#define Cfmemcpy(a,b,c)       _fmemcpy  ((PFCHAR) (a), (PFCHAR) (b), c)
#define Cfmemset(a,b,c)       _fmemset  ((PFCHAR) (a), b, c)
#define Cfmemchr(a,b,c)       _fmemchr  ((PFCHAR) (a), b, c)
#define Cfstrlen(a)           _fstrlen  ((PFCHAR) (a))
#define Cfstrcpy(a,b)         _fstrcpy  ((PFCHAR) (a), (PFCHAR) (b))
#define Cfstrncpy(a,b,c)      _fstrncpy ((PFCHAR) (a), (PFCHAR) (b), c)
#define Cfstrcat(a,b)         _fstrcat  ((PFCHAR) (a), (PFCHAR) (b))
#define Cfstrchr(a,b)         _fstrchr  ((PFCHAR) (a),  (b))
#define Cfstrstr(a,b)         _fstrstr  ((PFCHAR) (a), (PFCHAR) (b))
#else   /* Far functs needed but not available from compiler - use SL   */
#define Cfstrcmp        SLfstrcmp       /* Far strcmp() call            */
#define Cfstrncmp       SLfstrncmp      /* Far strncmp() call           */
#define Cfmemcpy        SLfmemcpy       /* Far memcpy() call            */
#define Cfmemset        SLfmemset       /* Far memset() call            */
#define Cfmemchr        SLfmemchr       /* Far memchr() call            */
#define Cfstrlen        SLfstrlen       /* Far strlen() call            */
#define Cfstrcpy        SLfstrcpy       /* Far strcpy() call            */
#define Cfstrncpy       SLfstrncpy      /* Far strncpy() call           */
#define Cfstrcat        SLfstrcat       /* Far strcat() call            */
#define Cfstrchr        SLfstrchr       /* Far strchr() call            */
#endif  /* SLA_FARCHAR_LF */

/* These functions have far versions in all compilers.
*/
#define Cfmemcmp        _fmemcmp        /* Far memcmp() call            */
#define Cfstrncat       _fstrncat       /* Far strncat() call           */

#else   /* SLA_SEGMENT_AM - non-segmented architecture; no FAR distinction */

#define Cfstrcmp        Cstrcmp         /* Generic strcmp() call        */
#define Cfstrncmp       Cstrncmp        /* Generic strncmp() call       */
#define Cfmemcpy        Cmemcpy         /* Generic memcpy() call        */
#define Cfmemset        Cmemset         /* Generic memset() call        */
#define Cfmemchr        Cmemchr         /* Generic memchr() call        */
#define Cfstrlen        Cstrlen         /* Generic strlen() call        */
#define Cfstrcpy        Cstrcpy         /* Generic strcpy() call        */
#define Cfstrncpy       Cstrncpy        /* Generic strncpy() call       */
#define Cfstrcat        Cstrcat         /* Generic strcat() call        */
#define Cfstrchr        Cstrchr         /* Generic strchr() call        */
#define Cfstrstr        Cstrstr         /* Generic strstr() call        */

#define Cfmemcmp        Cmemcmp         /* Generic memcmp() call        */
#define Cfstrncat       Cstrncat        /* Generic strncat() call       */

#endif /* SLA_SEGMENT_AM */

/* string & char array manipulation operations continued ...
*/
/* Functions for use with stack variables that must sometimes be referred to
 * by far pointers, sometimes by near.  Please refer to the comment in SL.H
 * where WFAR is defined.  The note on ifdef nesting & implicit dependence 
 * applies here as well).
*/
#ifdef  SLA_DSEQSS_MA

#define CWfmemcpy       Cmemcpy         /* Generic memcpy() call        */
#define CWfmemset       Cmemset         /* Generic memset() call        */
#define CWfmemchr       Cmemchr         /* Generic memchr() call        */
#define CWfstrlen       Cstrlen         /* Generic strlen() call        */
#define CWfstrcpy       Cstrcpy         /* Generic strcpy() call        */
#define CWfstrncpy      Cstrncpy        /* Generic strncpy() call       */
#define CWfstrcat       Cstrcat         /* Generic strcat() call        */
#define CWfstrcmp       Cstrcmp         /* Generic strcmp() call        */
#define CWfstrncmp      Cstrncmp        /* Generic strncmp() call       */

#else

#define CWfmemset(a,b,c)  _fmemset  ((PFCHAR) (a), (b), (c))
#define CWfmemcpy(a,b,C)  _fmemcpy  ((PFCHAR) (a), (PFCHAR) (b), (c))
#define CWfmemchr(a,b,c)  _fmemchr  ((PFCHAR) (a), (b), (c))
#define CWfstrlen(a)      lstrlen   ((PFCHAR) (a))
#define CWfstrcpy(a,b)    lstrcpy   ((PFCHAR) (a), (PFCHAR) (b))
#define CWfstrncpy(a,b,c) _fstrncpy ((PFCHAR) (a), (PFCHAR) (b), (c))
#define CWfstrcat(a,b)    lstrcat   ((PFCHAR) (a), (PFCHAR) (b))
#define CWfstrcmp(a,b)    _fstrcmp  ((PFCHAR) (a), (PFCHAR) (b))
#define CWfstrncmp(a,b,c) _fstrncmp ((PFCHAR) (a), (PFCHAR) (b), (c))
#endif  /* SLA_DSEQSS_MA */



/* Bare Memory Management Functions
 *
 * These functions have no concept of object referencers.  They're
 * equivalent to immediate object or object plug calls.  They are best used 
 * for small, transient memory allocation.  They all have counterparts
 * in SLMEMCOV.H, which can be used for easier cross-platform
 * coding.  These remain for backward compatibility.
 *
 * Cautions:
 * The nesting of #ifdefs below is not particularly straightforward.
 * Not all of the following macros are defined in all environments.
 *
*/

/* calls to memory manager of operating system
*/
#ifdef SLA_INIMEMGR_LF
#define CMemInit        MaxApplZone /* Expands heap to heap limit   */
#define CMoreMasters    MoreMasters /* Allocate more master pointers*/
#else
#define CMemInit        ;
#define CMoreMasters    ;
#endif

/* Distance-independent (no near/far distinction) alloc/free functions
*/
#define Ccalloc(s,n)    calloc(s,n)     /* Generic calloc() call        */
#define Cmalloc         malloc          /* Generic malloc() call        */
#define Cfree           free            /* Generic free() call          */

/* Specifically near alloc functions.
*/
#ifdef SLA_LOCALALLOC_LF

#define Cnmalloc(size)  (VOID near *) LocalAlloc((WORD) LMEM_FIXED,(WORD) (size))
#define Cnfree(ptr)     LocalFree((HANDLE) (ptr))

#else

#ifdef  SLA_NMALLOC_LF
#define Cnmalloc        _nmalloc        /* Near malloc() call           */
#define Cnfree          _nfree          /* Near free() call             */
#else
#define Cnmalloc        malloc          /* Near malloc() call           */
#define Cnfree          free            /* Near free() call             */
#endif

#endif  /* SLA_LOCALALLOC_LF */

/* near/far alloc/free functions
*/
#ifdef  SLA_SEGMENT_AM

#ifdef __BORLANDC__
#define Cfcalloc(s,n)   farcalloc(s,n)
#define Cfmalloc        farmalloc
#define Cffree          farfree
#define Cfrealloc       farrealloc
#else
#define Cffree          _ffree          /* Far free() call              */
#define Cfmalloc        _fmalloc        /* Far malloc() call            */
#define Cfrealloc       _frealloc       /* Far realloc() call           */
#ifdef  SLA_NOFCALLOC_LF
#define Cfcalloc        SLfcalloc       /* Far calloc() call            */
#else
#define Cfcalloc        _fcalloc        /* Far calloc() call            */
#endif  /* SLA_NOFCALLOC_LF */
#endif  /* __BORLANDC__	*/

#else   /* not segmented */

#define Cfmalloc        Cmalloc         /* Generic malloc() call        */
#define Cffree          Cfree           /* Generic free() call          */
#define Cfcalloc(s,n)   Ccalloc(s,n)    /* Generic calloc() call        */
#define Cfrealloc       Crealloc        /* Generic realloc() call      */

#endif  /* SLA_SEGMENT_AM */

/* handle/pointer-only specific functions
*/
#ifdef SLA_HDL_MM

#undef CAllocMem        /* NOT DEFINED */
#undef CFreeMem         /* NOT DEFINED */

#ifdef SLA_HDLPTR_MM
#ifdef SL_MAC_NOANSI_MEM   /* Use Mac Toolbox instead of ANSI lib */
#undef  Crealloc
#undef  Cmalloc
#undef  Cnmalloc
#undef  Ccalloc
#undef  Cfree
#undef  Cnfree

#define  Crealloc(p,s)         SLrealloc(&p,s)
#define  Cmalloc(s)          (void *)NewPtr((Size)(s))
#define  Cnmalloc(s)         (void *)NewPtr((Size)(s))
#define  Ccalloc(s,n)     (void *)NewPtrClear((Size)((s) * (n)))
#define  Cfree(p)            DisposePtr((Ptr)(p))
#define  Cnfree(p)           DisposePtr((Ptr)(p))
#endif   /* SL_MAC_NOANSI_MEM */
#define CAllocHandle(size)      NewHandle((Size)(size))
#define CFreeHandle(h)          DisposHandle((Handle)(h))
#define CLockHandle(h,p)        HLock((Handle)(h)),(p) = *(h)
#define CLockHugeHandle(h,p)    HLock((Handle)(h)),(p) = *(h)
#define CUnlockHandle(h)        HUnlock((Handle)(h))
#else
#define CAllocHandle(size)      GlobalAlloc((WORD) GHND, (DWORD) (size))
#define CFreeHandle(h)          GlobalFree((HANDLE) (h))
#define CLockHandle(h,p)        (p) = (VOID FAR *) GlobalLock((HANDLE) (h))
#define CLockHugeHandle(h,p)    (p) = (VOID HUGE *) GlobalLock((HANDLE) (h))
#define CUnlockHandle(h)        GlobalUnlock((HANDLE) (h))
#endif /* SLA_HDLPTR_MM */

#else   /* SLA_HDL_MM  - Pointer-only environments */

#undef  CAllocHandle         /* NOT DEFINED */
#undef  CFreeHandle          /* NOT DEFINED */
#define CLockHandle(h,p) ;              /* Null statement */
#define CLockHugeHandle(h,p) ;          /* Null statement */
#define CUnlockHandle(h) ;              /* Null statement */

#ifdef  SLA_SEGMENT_AM
#ifdef  SLA_NOFCALLOC_LF
#define CAllocMem       SLfcalloc       /* Far calloc() call            */
#else
#define CAllocMem       _fcalloc        /* Far calloc() call            */
#endif  /* SLA_NOFCALLOC_LF */
#define CFreeMem        _ffree          /* Far free() call              */
#else
#define CAllocMem       calloc          /* Generic calloc() call        */
#define CFreeMem        free            /* Generic free() call          */
#endif  /* SLA_SEGMENT_AM */

#ifdef SLA_HUGEPTR_DT
#define Challoc         halloc          /* Huge malloc() call           */
#define Chfree          hfree           /* Huge free() call             */
#else
#define Challoc         calloc          /* Generic calloc() call        */
#define Chfree          free            /* Generic free() call          */
#endif

#endif  /* SLA_HDL_MM */


/*                              file operations
*/


/* These functions are for use in environments with segmented architectures.
 * They are specifically for accessing FAR data.
*/

/* Use Long (far) FILE functions if available.  They take far pointers 
 * to file names and buffers.  Seek & close take no pointers so it can 
 * be generic except under SLA_LFILE_LF where they must be 
 * paired with the other _l file functions.
*/
#ifdef SLA_LFILE_LF
#define Cfaropen            _lopen                      /* Far open() call  */
#define Cfarread(fd,b,btr)  ((SWORD)_lread (fd,b,btr))  /* Far read() call  */
#define Cfarwrite(fd,b,btw) ((SWORD)_lwrite(fd,b,btw))  /* Far write() call */
#define Cfarlseek           _llseek                     /* Far lseek() call */
#define Cfarclose           _lclose                     /* Far close() call */

#else

#ifdef SLA_SEGMENT_AM
#define Cfaropen        SLfaropen                       /* Far open() call  */
#define Cfarread        SLfarread                       /* Far read() call  */
#define Cfarwrite       SLfarwrite                      /* Far write() call */
#define Cfarlseek       SLfarlseek                      /* Far lseek() call */
#define Cfarclose       SLfarclose                      /* Far close() call */
#else
#define Cfaropen        Copen           /* Generic open() call          */
#define Cfarread        Cread           /* Generic read() call          */
#define Cfarwrite       Cwrite          /* Generic write() call         */
#define Cfarlseek       Clseek          /* Generic lseek() call         */
#define Cfarclose       Cclose          /* Generic close() call         */
#endif /* SLA_SEGMENT_AM */

#endif /* SLA_LFILE_LF */

/* Functions for use with stack variables that must sometimes be referred to
 * by far pointers, sometimes by near.  Please refer to the comment in SL.H
 * where WFAR is defined.  The note on ifdef nesting & implicit dependence 
 * applies here as well).
*/
#ifdef  SLA_DSEQSS_MA

#define CWfarread       Cread           /* Generic read() call          */
#define CWfarwrite      Cwrite          /* Generic write() call         */
#define CWfaropen       Copen           /* Generic open() call          */

#else

#define CWfarread       _lread          /* Far read() call              */
#define CWfarwrite      _lwrite         /* Far write() call             */
#define CWfaropen       _lopen          /* Far open() call              */

#endif  /* SLA_DSEQSS_MA */

#endif /* SLCALLS_H */

/* End of file - slcalls.h */
