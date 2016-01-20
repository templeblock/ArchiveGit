/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All rights reserved.

This work is protected by the Hungarian and International Copyright Laws.
Solely for use as permitted by the written license from MorphoLogic.
Disclosure of contents or parts of this material are prohibited.

Module:		he_defs.h
Version:	4.15
Purpose:	configuration control
Compiler:	Borland C++ 3.1
Created by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

#ifndef HE_DEFS_INCLUDED  /* { */
#define HE_DEFS_INCLUDED

/*==========================================================================*/
#include "slfun.h"
#include "icfun.h"

				/* Language: */
#ifdef ICS_HUNGARIAN
#define HE_HUNGARIAN     	/* wired in Hungarian rules */
#endif
#ifdef ICS_POLISH
#define HE_POLISH /* wired in Polish rules */
#endif
				/* Code Page */
/* #define HE_CP_ISL */         /* selected by Inso Corp.'s platform costants */
/* #define HE_CP_CWIL2 */	/* CWI-2 with Latin-2 extensions */
/* #define HE_CP_DOSL2 */	/* CP-852 with Latin-2 extensions */
/* #define HE_CP_WINL2 */	/* Windows Latin-2 (WinCP-1250) */
/* #define HE_CP_ISOL2 */	/* ISO Latin-2 */
/* #define HE_CP_MACL2 */	/* Macintosh Latin-2 */
#define HE_CP_ICSL2      	/* Inso Latin-2 used in ICS */

#if defined(WINDOWS) || defined(WIN32) /* { */
#define HE_WIN_INTL 	/* get international settings from SYSTEM.INI */
#endif

/* #define HE_NONSTD_CP */	/* store a secondary code page number */

				/* Code customization */
/*         none */		/* spell verification */
/* #define HE_PARSE */		/* char filtering, case conversions, abbrevs */

#ifdef ICS_CORRECTION
#define HE_SUGGEST 	/* correction suggestions */
#endif

#ifdef ICS_HYPHENATION
#define HE_HYPHENATE 	/* hyphenation */
#define HE_HYPH_RDONLY
#endif

/* #define HE_USERDICT */	/* user dictionaries */
/* #define HE_INFLDICT */	/* inflectional user dictionaries */
/* #define HE_ANALYZE */	/* morphological analysis */
/* #define HE_LEMMATIZE	*/	/* lemmatization */

				/* File I/O library: */
/*         none */		/* UNIX/DOS high and low level I/O */
/* #define HE_WIN_IO */		/* MS Windows SDK I/O */
/* #define HE_WIN32_IO */	/* MS Win32 SDK I/O */
#define HE_ISL_IO   		/* Inso SL I/O */
/* #define HE_ANSI_IO */	/* ANSI C high level I/O only */

				/* File system dependent EOLN */

#if defined (MSDOS) || defined(WINDOWS) || defined(WIN32) || defined(OS2)
#define HE_EOLN_CRLF     	/* DOS: CR+LF */
#elif defined(MAC)
#define HE_EOLN_CR         /* Macintosh: CR only */
#else
#define HE_EOLN_LF       	/* Unix: LF only */
#endif

				/* Memory management library: */
/*         none */		/* ANSI C memory management */
/* #define HE_WIN_MEM */	/* MS Windows SDK dynamic memory management */
#define HE_ISL_MEM 	/* Inso SL dynamic memory management */
/* #define HE_CL52_MEM */	/* CA Clipper 5.2 virtual memory management */

#if defined ( __BORLANDC__ )
#define HE_NO_CALLBACK		/* disable function pointers usage on callbacks 
                              we could not make it work under Borland */
#endif

#define HE_SPELL_F  spell_f	/* the name of the speller function for the suggestion module */
#define HE_RESULT_F result_f	/* the name of the result function for the suggestion module */

/*==========================================================================*/

/* Inso Latin-2 */

#if defined(HE_CP_ISL) /* { */
#if defined(SLA_IBMCHAR_CS) /* {{ */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)
#define HE_CP_DOSL2
#define HE_CPN_ISL HE_CPN_DOSL2
#endif
#if defined(HE_ENGLISH)
#define HE_CP_DOSL1
#define HE_CPN_ISL HE_CPN_DOSL1
#endif
#elif defined(WINDOWS) || defined(WIN32) /* }{ */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)
#define HE_CP_WINL2
#define HE_CPN_WINL2	1250
#define HE_CPN_ISL HE_CPN_WINL2
#endif
#if defined(HE_ENGLISH)
#define HE_CP_WINL1
#define HE_CPN_ISL HE_CPN_WINL1
#endif
#elif defined(MAC) || defined(POWERMAC) /* }{ */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)
#define HE_CP_MACL2
#define HE_CPN_ISL HE_CPN_MACL2
#endif
#if defined(HE_ENGLISH)
#define HE_CP_MACL1
#define HE_CPN_ISL HE_CPN_MACL1
#endif
#elif defined(UNIX) || defined(VAX) /* }{ */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)
#define HE_CP_ISOL2
#define HE_CPN_ISL HE_CPN_ISOL2
#endif
#if defined(HE_ENGLISH)
#define HE_CP_ISOL1
#define HE_CPN_ISL HE_CPN_ISOL1
#endif
#else /* }{ */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)
#define HE_CP_ICSL2
#define HE_CPN_ISL HE_CPN_ICSL2
#endif
#if defined(HE_ENGLISH)
#define HE_CP_ICSL1
#define HE_CPN_ISL HE_CPN_ICSL1
#endif
#endif	/* }} */
#endif	/* } */

#if !defined(HE_EOLN_CRLF) \
 && !defined(HE_EOLN_CR)   \
 && !defined(HE_EOLF_LF) /* { */
#if defined(__MSDOS__) || defined(MSDOS) \
 || defined(M_I86) || defined(M_I186)    \
 || defined(M_I286) || defined(M_I386)   \
 || defined(_M_I86) || defined(_M_I186)  \
 || defined(_M_I286) || defined(_M_I386) /* {{ */
#define HE_EOLN_CRLF
#elif defined(HE_WIN_IO) /* }{ */
#define HE_EOLN_CRLF
#elif defined(HE_ISL_IO)
#if defined(SLA_IBMCHAR_CS) \
 || defined(WINDOWS)        \
 || defined(WIN32) \
 || defined(OS2) \
 || defined(VAX) /* {{ */
#define HE_EOLN_CRLF
#elif defined(MAC) \
   || defined(POWERMAC) /* }{ */
#define HE_EOLN_CR
#elif defined(UNIX) /* }{ */
#define HE_EOLN_LF
#else /* }{ */
/* #error Unknown ISL target system. */
#endif  /* }} */
#endif	/* }} */
#endif	/* } */

/* Nonterminal suffix dictionary */

#if defined(HE_HUNGARIAN) /* { */
#define HE_NONT_DICT
#endif	/* } */


#endif  /* !HE_DEFS_INCLUDED */

/* end */
