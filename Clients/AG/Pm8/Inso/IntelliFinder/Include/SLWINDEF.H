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
*        SLwindef.h - #undefs and #defines for Windows.                 *
*                                                                       *
*-----------------------------------------------------------------------*

Description:
    This file defines some Windows constants that control
    which sections of windows.h are included.

Cautions:
    If you're making a test driver, you'll want to knock off some
    of the defines here.

Creation Date:
    12-jul-91           Created by InfoSoft Software Engineering

Change Record:
    Date        Author  Comment
    18-nov-91   JCC     Add #undef NULL to override one from stdlib.h
    20-nov-91   ECH     switch on SL_INTERNAL, else ignore most of the file.

*-----------------------------------------------------------------------*/
#ifndef SLWINDEF_H

#define SLWINDEF_H


/*    These only need to be defined for Windows    */

#if defined(WINDOWS) || defined(WIN32)

/* NULL is not defined in InfoSoft products and will be #undef'd again in 
 * slundef.h, after windows.h is included.  It's #undef'd here to avoid a
 * NULL redefinition error in windows.h
*/
#ifdef NULL
#undef NULL
#endif

/* If VOID is defined, WINNT.H (a Win32 include file) will not define VOID.
 * It also will not define CHAR, SHORT, or LONG if VOID is define, which
 * causes WINNT.H to generate errors during compilation.
*/
#ifndef _WINNT_     /* The following not necessary if WINNT already included */
#ifdef VOID
#undef VOID
#endif
#endif

/* If the program is a DLL (internal software, not UI), stub out a bunch of 
 * the windows definitions.
*/
#ifdef SL_INTERNAL

/* Ignore a few groups of references in windows.h                         */
#if 0

#define NOATOM                    /* Atom Manager routines                */
#define NOKANJI                   /* Kanji support stuff.                 */
#define NOMETAFILE                /* typedef METAFILEPICT                 */
#define NORASTEROPS               /* binary and tertiary raster ops       */
#define NOSOUND                   /* Sound driver routines                */

#define NOFONT                    /* typedef FONT and associated routines */
#define NOPEN                     /* typedef HPEN and associated routines */

#define NOGDI                     /* No graphic functions                 */
#define NOGDICAPMASKS             /* No cap masks                         */
#define NOWH                      /* No Windows hook                      */
#define NOPROFILER                /* No profile routines                  */

#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NONCMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NODRAWFRAME
#define NOMENUS
#define NOICON
#define NOKEYSTATE
#define NOSYSCOMMANDS
#define NOSHOWWINDOW
#define NOSYSMETRICS
#define NOBITMAP
#define NOBRUSH
#define NOCLIPBOARD
#define NOCOLOR
#define NOCREATESTRUCT
#define NOCTLMGR
#define NODRAWTEXT
#define NOHDC
#define NOMENUS
#define NOMINMAX
#define NOMSG
#define NOPOINT
#define NORECT
#define NOREGION
#define NOSCROLL
#define NOTEXTMETRIC
#define NOWINOFFSETS
#define NOWNDCLASS  
#define NOCOMM
#endif
#ifdef  NDEBUG

#define NOMB

#endif   /* NDEBUG */

#endif   /* SL_INTERNAL */

#endif   /* WINDOWS */

#endif /* SLWINDEF_H */

/* End of file - slwindef.h */
